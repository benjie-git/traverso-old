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
 
    $Id: TrackGain.h,v 1.1 2006/04/20 14:51:13 r_sijrier Exp $
*/

#ifndef TRACKGAIN_H
#define TRACKGAIN_H

#include "Command.h"

class Song;
class Track;

class TrackGain : public Command
{
public :
        TrackGain(Track* track, Song* song);
        ~TrackGain();

        int begin_hold();
        int finish_hold();
        int prepare_actions();
        int do_action();
        int undo_action();

        int jog();

private :
        Song* m_song;
        Track* m_track;
        float origGain;
        float newGain;
        int origY;
};

#endif
