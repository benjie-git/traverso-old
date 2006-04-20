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
 
    $Id: SongView.cpp,v 1.1 2006/04/20 14:54:03 r_sijrier Exp $
*/

#include <QPainter>

#include "SongView.h"
#include "TrackView.h"

#include "Song.h"
#include "Track.h"
#include "ViewItem.h"
#include "Peak.h"
#include "Cursor.h"
#include "LocatorView.h"
#include "ViewPort.h"

#include <commands.h>

// Always put me below _all_ includes, this is needed
// in case we run with memory leak detection enabled!
#include "Debugger.h"

SongView::SongView(Song* song, ViewPort* vp)
                : ViewItem(vp, (ViewItem*) 0, song), m_song(song), trackViewList()
{
        PENTERCONS;

        m_cursor = new Cursor(this, m_vp, m_song);
        m_locator = new LocatorView(this, m_vp);
        verticalScrollAmount=20;
        currentCursorMapIndex = 0;

        cursorMap[CURSOR_FLOAT] = QCursor( QPixmap(":/cursorFloat") );
        cursorMap[CURSOR_FLOAT_OVER_CLIP] = QCursor( QPixmap(":/cursorFloatOverClip") );
        cursorMap[CURSOR_FLOAT_OVER_TRACK] = QCursor( QPixmap(":/cursorFloatOverTrack") );
        cursorMap[CURSOR_HOLD_UD] = QCursor( QPixmap(":/cursorHoldUd") );
        cursorMap[CURSOR_HOLD_LR] = QCursor( QPixmap(":/cursorHoldLr") );
        cursorMap[CURSOR_HOLD_LRUD] = QCursor( QPixmap(":/cursorHoldLrud") );
        cursorMap[CURSOR_DRAG] = QCursor( QPixmap(":/cursorDrag") );
        cursorMap[CURSOR_SELECT] = QCursor( QPixmap(":/cursorSelect") );
        cursorMap[CURSOR_MAGIC_ZOOM] = QCursor( QPixmap(":/cursorMagicZoom") );

        connect(m_song, SIGNAL(trackCreated(Track* )), this, SLOT(add_new_trackview(Track* )));
        connect(m_song, SIGNAL(hzoomChanged( )), m_locator, SLOT(hzoom_changed( )));
        connect(m_song, SIGNAL(firstBlockChanged()), m_locator, SLOT(schedule_for_repaint()));
        connect(m_vp, SIGNAL(resized()), this, SLOT(resize()));
        connect(m_vp, SIGNAL(pointChanged( ) ), this, SLOT(set_context()));

        init_context_menu( this );
}

SongView::~SongView()
{
        PENTERDES;
        delete m_cursor;
        delete m_locator;
}

void SongView::set_context()
{
        PENTER4;
        m_vp->schedule_for_repaint(m_cursor);

        // FIXME Bareuh, not a better way to do this?
        m_song->set_cursor_pos(cpointer().x());

        if (ie().is_jogging()) {
                return;
        }

        QList<ViewItem* > list;
        m_vp->get_pointed_view_items(list);

        if ( list.size() == 0) {
                return;
        }

        ViewItem* view = list.at(0);
        int type = view->type();

        switch (type) {
        case	 AUDIOCLIPVIEW:
                if (currentCursorMapIndex != CURSOR_FLOAT_OVER_CLIP) {
                        m_vp->setCursor(cursorMap[CURSOR_FLOAT_OVER_CLIP]);
                        currentCursorMapIndex = CURSOR_FLOAT_OVER_CLIP;
                }
                break;
        case TRACKVIEW	:
                if (currentCursorMapIndex != CURSOR_FLOAT_OVER_TRACK) {
                        m_vp->setCursor(cursorMap[CURSOR_FLOAT_OVER_TRACK]);
                        currentCursorMapIndex = CURSOR_FLOAT_OVER_TRACK;
                }
                break;
        default			:
                if (currentCursorMapIndex != CURSOR_FLOAT) {
                        m_vp->setCursor(cursorMap[CURSOR_FLOAT]);
                        currentCursorMapIndex = CURSOR_FLOAT;
                }
        }

}


QRect SongView::draw( QPainter& p )
{
        PENTER;
        paint_splitter(p);
        clear_root_space(p);
        return QRect();
}


void SongView::add_new_trackview( Track* track )
{
        PENTER;
        TrackView* trackView = new TrackView(m_vp, this, track);
        trackViewList.append(trackView);
}


void SongView::paint_splitter( QPainter & painter )
{
        painter.setPen(QColor(190,190,190));
        painter.drawLine(TrackView::TRACKPANELWIDTH, 0, TrackView::TRACKPANELWIDTH, m_vp->height());
        painter.setPen(QColor(220,220,220));
        painter.drawLine(TrackView::TRACKPANELWIDTH+1, 0, TrackView::TRACKPANELWIDTH+1, m_vp->height());
        painter.setPen(QColor(250,250,250));
        painter.drawLine(TrackView::TRACKPANELWIDTH+2, 0, TrackView::TRACKPANELWIDTH+2, m_vp->height());
        painter.setPen(QColor(220,220,220));
        painter.drawLine(TrackView::TRACKPANELWIDTH+3, 0, TrackView::TRACKPANELWIDTH+3, m_vp->height());
        painter.setPen(QColor(190,190,190));
        painter.drawLine(TrackView::TRACKPANELWIDTH+4, 0, TrackView::TRACKPANELWIDTH+4, m_vp->height());
}

void SongView::clear_root_space( QPainter & p )
{
        int lasty;
        if (m_song->get_numtracks() > 0)
                lasty = m_song->get_track(m_song->get_numtracks()-1)->real_baseY() + m_song->get_track(m_song->get_numtracks()-1)->get_height();
        else
                lasty =0;
        p.fillRect(0, lasty, TrackView::TRACKPANELWIDTH, m_vp->height(), QColor(0, 0, 0));
        p.fillRect(TrackView::CLIPAREABASEX, lasty, m_vp->width(), m_vp->height() - lasty, QColor(0, 0, 0));
}

void SongView::schedule_for_repaint( )
{
        m_vp->schedule_for_repaint(this);
}


Command* SongView::zoom()
{
        return new Zoom(this);
}

Command* SongView::hzoom_out()
{
        PENTER;
        m_song->set_hzoom(m_song->get_hzoom() + 1);
        center();
        return (Command*) 0;
}


Command* SongView::hzoom_in()
{
        PENTER;
        m_song->set_hzoom(m_song->get_hzoom() - 1);
        center();
        return (Command*) 0;
}


Command* SongView::vzoom_out()
{
        PENTER;
        int lasty=LocatorView::LOCATOR_HEIGHT;
        foreach(Track* track, m_song->get_tracks()) {
                track->vzoom_out(lasty);
                lasty += track->get_height();
        }
        return (Command*) 0;
}


Command* SongView::vzoom_in()
{
        PENTER;
        int lasty=LocatorView::LOCATOR_HEIGHT;
        foreach(Track* track, m_song->get_tracks()) {
                track->vzoom_in(lasty);
                lasty += track->get_height();
        }
        return (Command*) 0;
}

Command* SongView::center()
{
        PENTER2;
        TrackView* view = (TrackView*)trackViewList.at(0);
        int w = view->cliparea_width();
        int half = w/2;
        nframes_t minimumBlockToCenter =  w * Peak::zoomStep[m_song->get_hzoom()] / 2;
        if ( m_song->get_working_block() >= minimumBlockToCenter ) {
                int x = m_song->block_to_xpos(m_song->get_working_block());
                if (x<half) {
                        scrollAmount = half - x;
                        scroll_left();
                } else {
                        scrollAmount = x-half;
                        scroll_right();
                }
        }
        return (Command*) 0;
}


Command* SongView::scroll_right()
{
        PENTER3;
        if (scrollAmount != 0)
                m_song->set_first_block(m_song->get_firstblock() + scrollAmount * Peak::zoomStep[m_song->get_hzoom()]);
        scrollAmount = 20;
        return (Command*) 0;
}


Command* SongView::scroll_left()
{
        PENTER3;
        int frame = m_song->get_firstblock() - scrollAmount * Peak::zoomStep[m_song->get_hzoom()];
        if (frame < 0)
                frame  = 0;
        if (scrollAmount != 0)
                m_song->set_first_block(frame);
        scrollAmount = 20;
        return (Command*) 0;
}

Command* SongView::scroll_down()
{
        foreach(ViewItem* view, trackViewList) {
                ((TrackView*)view)->get_track()->set_baseY(((TrackView*)view)->get_base_y() - verticalScrollAmount);
        }
        return (Command*) 0;
}


Command* SongView::scroll_up()
{
        foreach(ViewItem* view, trackViewList) {
                ((TrackView*)view)->get_track()->set_baseY(((TrackView*)view)->get_base_y() + verticalScrollAmount);
        }
        return (Command*) 0;
}

Command* SongView::shuttle()
{
        return new Shuttle(this, m_vp);
}

Command* SongView::vertical_scroll()
{
        if (ie().is_holding()) {
                verticalScrollTotalHeight = m_song->get_floorY();
                origY = cpointer().y();
                // 		ie().set_jogging(true);
        }
        return (Command*) 0;
}


void SongView::update_shuttle()
{
        float maxAmount =  ((float) SHUTTLE_SENSIBILITY / 100.0f) * 30.0f;
        scrollAmount = (int) ( (float)(shuttleFactor/100.f) * maxAmount );
        if (scrollAmount != 0) {
                if (shuttleFactor < 0) {
                        scrollAmount *= -1;
                        scroll_left ();
                } else
                        scroll_right();
        }
}

Command* SongView::jog_vertical_scroll()
{
        float f;
        int amount;
        int y = cpointer().y();
        int vsy = origY - y;
        float limiter = 1.0f;
        if (vsy < -12 || vsy > 12) {
                float dy = (float)MOUSE_SENSIBILITY*(((float)vsy) / m_vp->width());
                if (dy < 0.0f)
                        dy *= -1.0f;
                if (dy > 0.6f)
                        dy = 0.6f;
                limiter -= dy;
        }
        if (vsy < -MOUSE_SENSIBILITY) {
                f = (float) vsy / (m_vp->height());
                amount = (int) (f * verticalScrollTotalHeight * VERTICAL_SCROLL_SENSIBILITY / 100.0f * limiter);
                mtaBaseY += amount;
                origY += MOUSE_SENSIBILITY;
                if (mtaBaseY < -(verticalScrollTotalHeight - m_vp->height()) + 1)
                        mtaBaseY=-(verticalScrollTotalHeight - m_vp->height() + 1);
                if (verticalScrollTotalHeight < m_vp->height())
                        mtaBaseY = 0;
        }
        if (vsy > MOUSE_SENSIBILITY) {
                f = (float) vsy / (m_vp->height());
                amount = (int) (f * verticalScrollTotalHeight * VERTICAL_SCROLL_SENSIBILITY / 100.0f * limiter);
                mtaBaseY+=amount;
                origY -=MOUSE_SENSIBILITY;
                if (mtaBaseY > 0)
                        mtaBaseY = 0;
        }
        return (Command*) 0;
}

Command* SongView::goto_begin()
{
        m_song->set_work_at(0);
        m_song->set_first_block(0);
        return (Command*) 0;
}


Command* SongView::goto_end()
{
        m_song->set_work_at(m_song->get_last_block());
        center();
        return (Command*) 0;
}

void SongView::resize()
{
        schedule_for_repaint();
}

int SongView::cliparea_width( ) const
{
        return ( m_vp->width() - TrackView::CLIPAREABASEX );
}

//eof
