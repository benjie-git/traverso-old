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
 
    $Id: AudioClipManager.cpp,v 1.10 2007/03/31 03:53:27 benjie Exp $
*/
 
#include "AudioClipManager.h"

#include "Song.h"
#include "AudioClip.h"
#include "Track.h"
#include "commands.h"
#include "SnapList.h"
#include "Utils.h"
#include "Debugger.h"

AudioClipManager::AudioClipManager( Song* song )
	: ContextItem(song)
{
	PENTERCONS;
	m_song = song;
	set_history_stack( m_song->get_history_stack() );
	lastFrame = 0;
}

AudioClipManager::~ AudioClipManager( )
{
	PENTERDES;
}

void AudioClipManager::add_clip( AudioClip * clip )
{
	PENTER;
	if (m_clips.contains(clip)) {
		PERROR("Trying to add clip %s, but it's allready in my list!!", QS_C(clip->get_name()));
	}
	m_clips.append( clip );
	
	connect(clip, SIGNAL(trackEndFrameChanged()), this, SLOT(update_last_frame()));
	connect(clip, SIGNAL(positionChanged(Snappable*)), m_song->get_snap_list(), SLOT(mark_dirty(Snappable*)));
	
	update_last_frame();
}

void AudioClipManager::remove_clip( AudioClip * clip )
{
	PENTER;
	disconnect(clip, SIGNAL(positionChanged(Snappable*)), m_song->get_snap_list(), SLOT(mark_dirty(Snappable*)));
	if (m_clips.removeAll(clip) == 0) {
		PERROR("Clip %s was not in my list, couldn't remove it!!", QS_C(clip->get_name()));
	}
	update_last_frame();
}


void AudioClipManager::update_last_frame( )
{
	PENTER;
	
	lastFrame = 0;
	
	foreach(AudioClip* clip, m_clips) {
		if (clip->get_track_end_frame() >= lastFrame)
			lastFrame = clip->get_track_end_frame();
	}
	
	emit m_song->lastFramePositionChanged();
}

nframes_t AudioClipManager::get_last_frame( )
{
	return lastFrame;
}

nframes_t AudioClipManager::get_start_frame( )
{
	nframes_t startframe = UINT_MAX;
	
	foreach(AudioClip* clip, m_clips) {
		if (clip->get_track_start_frame() < startframe)
			startframe = clip->get_track_start_frame();
	}
	
	return startframe;
}

void AudioClipManager::get_selected_clips_state( QList< AudioClip * > & list )
{
	foreach(AudioClip* clip, clipselection) {
		list.append(clip);
	}
}

void AudioClipManager::set_selected_clips_state( QList< AudioClip * > & list )
{
	clipselection.clear();
	
	foreach(AudioClip* clip, m_clips) {
		clip->set_selected(false);
	}
	
	foreach(AudioClip* clip, list) {
		add_to_selection( clip );
	}
}

void AudioClipManager::remove_from_selection( AudioClip * clip )
{
	int index = clipselection.indexOf( clip );
	
	if (index >= 0) {
		clipselection.takeAt( index );
		clip->set_selected( false );
	}
}

void AudioClipManager::add_to_selection( AudioClip * clip )
{
	if ( ! clipselection.contains( clip ) ) {
		clipselection.append( clip );
		clip->set_selected( true );
	}
}

void AudioClipManager::toggle_selected( AudioClip * clip )
{
	PENTER;
	
	if (clip->is_selected()) {
		remove_from_selection(clip);
	} else {
		add_to_selection(clip);
	}
}

void AudioClipManager::select_clip(AudioClip* clip)
{
	PENTER;
	
	foreach(AudioClip* c, m_clips) {
		if (c == clip) {
			continue;
		}
		remove_from_selection( c );
	}
	

	if (clip->is_selected()) {
		remove_from_selection(clip);
	} else {
		add_to_selection(clip);
	}
	
}

QList<AudioClip* >* AudioClipManager::get_clip_list()
{
	return &m_clips;
}

/****************************** SLOTS ***************************/
/****************************************************************/


Command* AudioClipManager::select_all_clips()
{
	PENTER;
	
	return new ClipSelection(m_clips, this, "add_to_selection", tr("Selection: Add Clip"));
}

Command* AudioClipManager::deselect_all_clips()
{
	PENTER;
	
	return new ClipSelection(m_clips, this, "remove_from_selection", tr("Selection: Remove Clip"));
}

Command* AudioClipManager::invert_clip_selection()
{
	PENTER;
	
	return new ClipSelection(m_clips, this, "toggle_selected", tr("Selection: Invert"));
}

Command* AudioClipManager::delete_selected_clips()
{
	PENTER;
	CommandGroup* group = new CommandGroup(this, tr("Remove Clip(s)"));
	foreach(AudioClip* clip, clipselection) {
		group->add_command(clip->get_track()->remove_clip(clip));
	}
	return group;
}


//eof
