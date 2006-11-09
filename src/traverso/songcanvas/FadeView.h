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

$Id: FadeView.h,v 1.1 2006/11/08 14:45:22 r_sijrier Exp $
*/

#ifndef FADE_VIEW_H
#define FADE_VIEW_H

#include "ViewItem.h"

class FadeCurve;
class FadeContextDialog;
class AudioClipView;

class FadeView : public ViewItem
{
	Q_OBJECT
public:
	FadeView(SongView* sv, AudioClipView* parent, FadeCurve* fadeCuve);
	~FadeView();
	
	enum {Type = UserType + 4};
	
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
	void calculate_bounding_rect();
	
	FadeCurve* get_fade() const {return m_fadeCurve;}
	
	int type() const;

private:
	AudioClipView*		m_clipView;
	FadeCurve*		m_fadeCurve;
	FadeContextDialog* 	m_dialog;
	QPainterPath 		m_path;
	
public slots:
	void state_changed();
	
	Command* edit_properties();
};

inline int FadeView::type() const {return Type;}

#endif

//eof

