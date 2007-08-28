/*
Copyright (C) 2007 Remon Sijrier

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

#ifndef PEAK_DATA_READER_H
#define PEAK_DATA_READER_H

#include <AbstractAudioReader.h>


class PeakDataReader : public AbstractAudioReader
{
	public:
		PeakDataReader(QString filename);
		~PeakDataReader();

		QString decoder_type() const {return "sndfile";}

		static bool can_decode(QString filename);

	protected:
		bool seek_private(nframes_t start);
		nframes_t read_private(DecodeBuffer* buffer, nframes_t frameCount);

	
	private:
		FILE*		m_file;

};

#endif
