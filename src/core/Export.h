/*
Copyright (C) 2005-2006 Remon Sijrier 

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

$Id: Export.h,v 1.19 2009/05/03 21:47:32 n_doebelin Exp $
*/

#ifndef EXPORT_H
#define EXPORT_H

#include <QThread>
#include <QString>
#include <QMap>

#include <samplerate.h>

#include "defines.h"
#include "gdither.h"

class Project;
class ExportThread;
class Marker;

struct ExportSpecification
{
	ExportSpecification();
	
	int is_valid();
	
	enum RenderPass {
		CALC_NORM_FACTOR,
  		WRITE_TO_HARDDISK,
    		CREATE_CDRDAO_TOC
	};
	
	int      	sample_rate;
	int             src_quality;
	int       	channels;
	TimeRef		startLocation;
	TimeRef		endLocation;
        TimeRef		trackStart;
        TimeRef		trackEnd;
        GDitherType     dither_type;

	/* used exclusively during export */

	QString		writerType;
	float*          dataF;
	int		blocksize;
	int	        data_width;

	TimeRef      	totalTime;
	TimeRef      	pos;
	QMap<QString, QString>	extraFormat;

	/* shared between UI thread and audio thread */

	int 		progress;  /* audio thread sets this */
	bool  		stop;      /* UI sets this */
	bool		breakout;
	bool  		running;   /* audio thread sets to false when export is done */

	int   		status;
	bool		allSheets;
	int		isRecording;
	QString		exportdir;
	QString		basename;
	QString		name;
	QString		tocFileName;
	QString		cdrdaoToc;
	bool		writeToc;
	bool		normalize;
	int		renderpass;
	float		peakvalue;
	float 		normvalue;
	bool 		resumeTransport;
	TimeRef		resumeTransportLocation;
	bool		renderfinished;
	bool		isCdExport;
        QList<Marker*>  markers;
	
	ExportThread* 	thread;
};


class ExportThread : public QThread
{
	Q_OBJECT

public:
	ExportThread(Project* project);
	~ExportThread()
	{}

	void run();
	void sleep_for(uint msecs) {
		msleep(msecs);
	}
	void set_specification(ExportSpecification* spec);

private:
	Project*		m_project;
	ExportSpecification*	m_spec;
};


#endif
