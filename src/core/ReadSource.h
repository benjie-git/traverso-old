/*
    Copyright (C) 2006 Remon Sijrier 
 
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
 
    $Id: ReadSource.h,v 1.1 2006/04/20 14:51:40 r_sijrier Exp $
*/

#ifndef READSOURCE_H
#define READSOURCE_H

#include "AudioSource.h"

class ReadSource : public AudioSource
{
public :
        ReadSource(const QDomNode node);
        ReadSource(uint channel, QString dir, QString name);
        ReadSource(AudioSource* source);
        ~ReadSource();

        int rb_read(audio_sample_t* dst, nframes_t start, nframes_t cnt);
        int rb_file_read(audio_sample_t* dst, nframes_t cnt);
        void rb_seek_to_file_position(nframes_t position);
	int process_ringbuffer(audio_sample_t* framebuffer);

        int file_read(audio_sample_t* dst, nframes_t start, nframes_t cnt) const;

        int init();
        void set_source_start_offset(nframes_t offset);
        void set_rb_ready(bool ready);
	void set_active();
	void set_inactive();

        bool need_sync();
        void sync();


private:
        mutable float*	 		tmpbuf;
        mutable nframes_t 		tmpbufsize;
        mutable uint32_t 		m_read_data_count;
        nframes_t			rbFileReadPos;
        nframes_t			rbRelativeFileReadPos;
        nframes_t			sourceStartOffset;
        nframes_t			syncPos;
        bool					needSync;
        bool					rbReady;

        void start_resync(nframes_t position);

};

#endif




