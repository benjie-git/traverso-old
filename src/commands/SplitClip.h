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
 
    $Id: SplitClip.h,v 1.1 2006/04/20 14:51:13 r_sijrier Exp $
*/

#ifndef SPLITCLIPACTION_H
#define SPLITCLIPACTION_H

#include "Command.h"
#include "defines.h"

class AudioClip;
class Song;
class Track;

class SplitClip : public Command
{
public :
        SplitClip(Song* song, AudioClip* clip);
        ~SplitClip();

        int prepare_actions();
        int do_action();
        int undo_action();

private :
        Song* m_song;
        Track* m_track;
        AudioClip* m_clip;
        AudioClip* leftClip;
        AudioClip* rightClip;
        nframes_t	splitPoint;

};

#endif
