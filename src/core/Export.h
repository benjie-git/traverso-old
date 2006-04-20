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
 
    $Id: Export.h,v 1.1 2006/04/20 14:51:39 r_sijrier Exp $
*/

#ifndef EXPORT_H
#define EXPORT_H

#include <QThread>
#include <QByteArray>

#include <sndfile.h>
#include <samplerate.h>

#include "defines.h"
#include "gdither.h"

class Project;

struct ExportSpecification
{

        nframes_t      		sample_rate;

        int                 		src_quality;
        uint32_t       		channels;
        nframes_t      		start_frame;
        nframes_t      		end_frame;
        GDitherType         	dither_type;

        /* used exclusively during export */

        float*              		dataF;
        nframes_t			blocksize;
        uint32_t        		data_width;

        nframes_t      		total_frames;
        int		             	format;
        nframes_t      		pos;
        QByteArray		extension;

        /* shared between UI thread and audio thread */

        int 				progress;  /* audio thread sets this */
        bool  			stop;      /* UI sets this */
        bool  			running;   /* audio thread sets to false when export is done */

        int   				status;
        bool				allSongs;
        QString			exportdir;
        QString			name;
};


class ExportThread : public QThread
{
        Q_OBJECT

public:
        ExportThread(Project* project, ExportSpecification* spec);
        ~ExportThread()
        {}

        void run();

private:
        Project*				m_project;
        ExportSpecification*	m_spec;
};

#endif
