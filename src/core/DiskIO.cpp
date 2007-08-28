/*
Copyright (C) 2006-2007 Remon Sijrier

This file is part of Traverso

Traverso is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA.

*/

#include "DiskIO.h"
#include "Song.h"
#include <QThread>

#if defined (Q_WS_X11)

#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>
		
#if defined(__i386__)
# define __NR_ioprio_set	289
# define __NR_ioprio_get	290
# define IOPRIO_SUPPORT		1
#elif defined(__ppc__) || defined(__powerpc__) || defined(__PPC__)
# define __NR_ioprio_set	273
# define __NR_ioprio_get	274
# define IOPRIO_SUPPORT		1
#elif defined(__x86_64__)
# define __NR_ioprio_set	251
# define __NR_ioprio_get	252
# define IOPRIO_SUPPORT		1
#elif defined(__ia64__)
# define __NR_ioprio_set	1274
# define __NR_ioprio_get	1275
# define IOPRIO_SUPPORT		1
#else
# define IOPRIO_SUPPORT		0
#endif

enum {
	IOPRIO_CLASS_NONE,
	IOPRIO_CLASS_RT,
	IOPRIO_CLASS_BE,
	IOPRIO_CLASS_IDLE,
};

enum {
	IOPRIO_WHO_PROCESS = 1,
	IOPRIO_WHO_PGRP,
	IOPRIO_WHO_USER,
};

const char *to_prio[] = { "none", "realtime", "best-effort", "idle", };
#define IOPRIO_CLASS_SHIFT	13
#define IOPRIO_PRIO_MASK	0xff

#endif // endif Q_WS_X11

#include "AbstractAudioReader.h"
#include "AudioSource.h"
#include "ReadSource.h"
#include "WriteSource.h"
#include "AudioDevice.h"
#include "RingBuffer.h"
#include "Config.h"

// Always put me below _all_ includes, this is needed
// in case we run with memory leak detection enabled!
#include "Debugger.h"


#define UPDATE_INTERVAL		20


// DiskIOThread is a private class to be used by
// DiskIO only for processing read/write buffers
// in a seperate thread.
class DiskIOThread : public QThread
{
public:
	DiskIOThread(DiskIO* diskio)
	: QThread(diskio),
	  m_diskio(diskio)
	{
#ifndef Q_WS_MAC
// 		setStackSize(20000);
#endif
	}

	DiskIO*		m_diskio;

protected:
	void run()
	{
#if defined (Q_WS_X11) 
	if (IOPRIO_SUPPORT) {
// When using the cfq scheduler we are able to set the priority of the io for what it's worth though :-) 
		int ioprio = 0, ioprio_class = IOPRIO_CLASS_RT;
		int value = syscall(__NR_ioprio_set, IOPRIO_WHO_PROCESS, getpid(), ioprio | ioprio_class << IOPRIO_CLASS_SHIFT);
		
		if (value == -1) {
			ioprio_class = IOPRIO_CLASS_BE;
			value = syscall(__NR_ioprio_set, IOPRIO_WHO_PROCESS, getpid(), ioprio | ioprio_class << IOPRIO_CLASS_SHIFT);
		}
		
		if (value == 0) {
			ioprio = syscall (__NR_ioprio_get, IOPRIO_WHO_PROCESS, getpid());
			ioprio_class = ioprio >> IOPRIO_CLASS_SHIFT;
			ioprio = ioprio & IOPRIO_PRIO_MASK;
			PMESG("Using prioritized disk I/O (Only effective with the cfq scheduler)");
			PMESG("%s: prio %d", to_prio[ioprio_class], ioprio);
		}
	}
#endif
		exec();
	}
};

/************** END DISKIO THREAD ************/



/** 	\class DiskIO 
 *	\brief handles all the read's and write's of AudioSources in it's private thread.
 *	
 *	Each Song class has it's own DiskIO instance. 
 * 	The DiskIO manages all the AudioSources related to a Song, and makes sure the RingBuffers
 * 	from the AudioSources are processed in time. (It at least tries very hard)
 */
DiskIO::DiskIO(Song* song)
	: m_song(song)
{
	m_diskThread = new DiskIOThread(this);
	cpuTimeBuffer = new RingBuffer(128);
	lastCpuReadTime = get_microseconds();
	m_stopWork = m_seeking = m_sampleRateChanged = 0;
	m_readBufferFillStatus = 0;
	m_hardDiskOverLoadCounter = 0;
	
	// TODO This is a LARGE buffer, any ideas how to make it smaller ??
	framebuffer[0] = new audio_sample_t[audiodevice().get_sample_rate() * writebuffertime];
	
	m_decodebuffer = new DecodeBuffer;

	// Move this instance to the workthread
	moveToThread(m_diskThread);
	m_workTimer.moveToThread(m_diskThread);

	connect(&m_workTimer, SIGNAL(timeout()), this, SLOT(do_work()));
	
	m_diskThread->start();
}

DiskIO::~DiskIO()
{
	PENTERDES;
	stop();
	delete cpuTimeBuffer;
	delete [] framebuffer[0];
	delete m_decodebuffer;
}

/**
* 	Seek's all the ReadSources readbuffers to the new position.
*	Call prepare_seek() first, to interupt do_work() if it was running.
* 
* @param position The position to seek too 
*/
void DiskIO::seek()
{
	PENTER;
	
//	Q_ASSERT_X(m_song->threadId != QThread::currentThreadId (), "DiskIO::seek", "Error, running in gui thread!!!!!");

	mutex.lock();
	
	m_stopWork = 0;
	m_seeking = true;
	
	TimeRef location = m_song->get_new_transport_location();

	foreach(ReadSource* source, m_readSources) {
		if (m_sampleRateChanged) {
			source->output_rate_changed();
			source->prepare_buffer();
		}
		source->rb_seek_to_file_position(location);
	}
	
	m_sampleRateChanged = false;
	
	mutex.unlock();

	// Now, fill the buffers like normal
	do_work();
	
	m_seeking = false;

	emit seekFinished();
}


void DiskIO::output_rate_changed()
{
	m_sampleRateChanged = true;
}


// Internal function
void DiskIO::do_work( )
{
//	Q_ASSERT_X(m_song->threadId != QThread::currentThreadId (), "DiskIO::do_work", "Error, running in gui thread!!!!!");

	QMutexLocker locker(&mutex);
	
	int whilecount = 0; 
	m_hardDiskOverLoadCounter = 0;
	cycleStartTime = get_microseconds();
	
	while (there_are_processable_sources()) {
		
		for (int i=0; i<m_processableReadSources.size(); ++i) {
			ReadSource* source = m_processableReadSources.at(i);
	
			if (m_stopWork) {
				update_time_usage();
				return;
			}
	
			source->process_ringbuffer(m_decodebuffer, m_seeking);
		}
		
		for (int i=0; i<m_processableWriteSources.size(); ++i) {
			WriteSource* source = m_processableWriteSources.at(i);
			source->process_ringbuffer(framebuffer[0]);
		}
		
		if (whilecount++ > 1000) {
			printf("DiskIO::do_work -> probably detected a loop here, or do_work() is REALLY buzy!!\n");
			break;
		}
		
	}
	
	update_time_usage();
}


// Internal function
int DiskIO::there_are_processable_sources( )
{
	m_processableReadSources.clear();
	m_processableWriteSources.clear();
	QList<ReadSource* > syncSources;
		
	for (int i=(bufferdividefactor-2); i >= 0; --i) {
		
		for (int j=0; j<m_writeSources.size(); ++j) {
			WriteSource* source = m_writeSources.at(j); 
			int space = source->get_processable_buffer_space();
			int prio = (int) (space  / source->get_chunck_size());
			
			// If the source stopped recording, it will write it's remaining samples in the next 
			// process_buffers call, and unregister itself from this DiskIO instance!
			if ( (prio > i) || ( ! source->is_recording()) ) {
				
				if ((source->get_buffer_size() - space) < 8192) {
					if (! m_hardDiskOverLoadCounter++) {
						emit writeSourceBufferOverRun();
					}
				}
				
				if (space > t_atomic_int_get(&m_writeBufferFillStatus)) {
					t_atomic_int_set(&m_writeBufferFillStatus, space);
				}
				
				m_processableWriteSources.append(source);
			}
		}
		
		for (int j=0; j<m_readSources.size(); ++j) {

			ReadSource* source = m_readSources.at(j);
			BufferStatus* status = source->get_buffer_status();
			
			if (status->priority > i && source->is_active() && !status->needSync ) {
				
				if ( (! m_seeking) && status->bufferUnderRun ) {
					if (! m_hardDiskOverLoadCounter++) {
						printf("DiskIO:: BuferUnderRun detected\n");
						emit readSourceBufferUnderRun();
					}
				}
				
				if (status->fillStatus > t_atomic_int_get(&m_readBufferFillStatus)) {
					t_atomic_int_set(&m_readBufferFillStatus, status->fillStatus);
				}
				
				m_processableReadSources.append(source);
			
			} else if (status->needSync) {
// 				printf("status == bufferUnderRun\n");
				if (syncSources.size() == 0) {
					syncSources.append(source);
				}
			}
		}
		
		if (m_processableReadSources.size() > 0 || m_processableWriteSources.size() > 0) {
			return 1;
		}
	}
				
	
	if (syncSources.size() > 0) { 
		syncSources.at(0)->sync(m_decodebuffer);
		return 1;
	}
	
	
	return 0;
}


// Internal function
int DiskIO::stop( )
{
	PENTER;
	int res = 0;

	// Stop any processing in do_work()
	m_stopWork = 1;

	// Exit the diskthreads event loop
	m_diskThread->exit(0);

	// Wait for the Thread to return from it's event loop. 1000 ms should be (more then) enough,
	// if not, terminate this thread and print a warning!
	if ( ! m_diskThread->wait(1000) ) {
		qWarning("DiskIO :: Still running after 1 second wait, terminating!");
		m_diskThread->terminate();
		res = -1;
	}

	return res;
}

/**
 *      Registers the ReadSource source. The source's RingBuffer will be initialized at this point.
 *
 *	Note: This function is thread save. 
 * @param source The ReadSource to register
 */
void DiskIO::register_read_source (ReadSource* source )
{
	PENTER2;
	
	QMutexLocker locker(&mutex);

	m_readSources.append(source);
	source->prepare_buffer();
}

/**
 *      Registers the WriteSource source. The source's RingBuffer will be initialized at this point.
 *
 *	Note: This function is thread save. 
 * @param source The WriteSource to register
 */
void DiskIO::register_write_source( WriteSource * source )
{
	PENTER2;
	
	source->set_diskio(this);
	source->prepare_buffer();
	
	QMutexLocker locker(&mutex);

	m_writeSources.append(source);
}

/**
 * 	Unregisters the ReadSource from this DiskIO instance
 *
 *	Note: This function is Thread save.
 * @param source The ReadSource to be removed from the DiskIO instance.
 */
void DiskIO::unregister_read_source( ReadSource * source )
{
	QMutexLocker locker(&mutex);
	
	m_readSources.removeAll(source);
}


// internal function
void DiskIO::unregister_write_source( WriteSource * source )
{
	m_writeSources.removeAll(source);
}

/**
 *      Interupts any pending AudioSource's buffer processing, and returns from do_work().
 *	Use this before calling seek() to shorten the seek process.
 */
void DiskIO::prepare_for_seek( )
{
	PENTER;
	// Stop any processing in do_work()
	m_stopWork = 1;
}

// Internal function
void DiskIO::update_time_usage( )
{
	audio_sample_t runcycleTime = get_microseconds() - cycleStartTime;
	cpuTimeBuffer->write((char*)&runcycleTime, 1 * sizeof(audio_sample_t));

}

/**
 *
 * @return Returns the CPU time consumed by the DiskIO work thread 
 */
trav_time_t DiskIO::get_cpu_time( )
{
	trav_time_t currentTime = get_microseconds();
	float totaltime = 0;
	float value = 0;
	int read = cpuTimeBuffer->read_space() / sizeof(audio_sample_t);

	while (read != 0) {
		read = cpuTimeBuffer->read((char*)&value, 1 * sizeof(audio_sample_t));
		totaltime += value;
	}

	audio_sample_t result = ( (totaltime  / (currentTime - lastCpuReadTime) ) * 100 );

	lastCpuReadTime = currentTime;

// 	if (result > 95) {
// 		qWarning("DiskIO :: consuming more then 95 Percent CPU !!");
// 	}

	return result;
}


/**
 * 	Get the status of the writebuffers.
 *
 * @return The status in procentual amount of the smallest remaining space in the writebuffers  
 *		that could be used to write 'recording' data too.
 */
int DiskIO::get_write_buffers_fill_status( )
{
	int space = t_atomic_int_get(&m_writeBufferFillStatus);
	int size = audiodevice().get_sample_rate() * writebuffertime;
	int status = (int) (((float)(size - space) / size) * 100);
	t_atomic_int_set(&m_writeBufferFillStatus, 0);
	
	return status;
}

/**
 * 	Get the status of the readbuffers.
 *
 * @return The status is the procentual amount of the buffer which was most empty since the last call to this function
 */
int DiskIO::get_read_buffers_fill_status( )
{
	int status = 100 - t_atomic_int_get(&m_readBufferFillStatus);
	t_atomic_int_set(&m_readBufferFillStatus, 0);
	
	return status;
}

void DiskIO::start_io( )
{
//	Q_ASSERT_X(m_song->threadId != QThread::currentThreadId (), "DiskIO::start_io", "Error, running in gui thread!!!!!");
	m_workTimer.start(UPDATE_INTERVAL);
}

void DiskIO::stop_io( )
{
//	Q_ASSERT_X(m_song->threadId != QThread::currentThreadId (), "DiskIO::stop_io", "Error, running in gui thread!!!!!");
// 	m_workTimer.stop();
}

//eof
