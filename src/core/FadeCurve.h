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

$Id: FadeCurve.h,v 1.19 2008/01/21 16:22:14 r_sijrier Exp $
*/

#ifndef FADE_CURVE_H
#define FADE_CURVE_H

#include "Curve.h"

#include <QString>
#include <QStringList>
#include <QList>
#include <QPointF>

class Sheet;
class AudioClip;
class AudioBus;

class FadeCurve : public Curve, public APILinkedListNode
{
	Q_OBJECT
	
	Q_CLASSINFO("toggle_bypass", tr("Toggle Bypass"))
	Q_CLASSINFO("set_mode", tr("Cycle Shape"))
	Q_CLASSINFO("reset", tr("Remove Fade"))
	Q_CLASSINFO("toggle_raster", tr("Toggle Raster"))
	
public:
	static QStringList defaultShapes;
	
	FadeCurve(AudioClip* clip, Sheet* sheet, QString type);
	~FadeCurve();
	
	enum FadeType {
		FadeIn,
		FadeOut
	};
	
	QDomNode get_state(QDomDocument doc);
	int set_state( const QDomNode & node );
	
	void process(AudioBus* bus, nframes_t nframes);
	
	float get_bend_factor() {return m_bendFactor;}
	float get_strength_factor() {return m_strenghtFactor;}
	int get_mode() const {return m_mode;}
	int get_raster() const {return m_raster;}
	
	void set_shape(QString shapeName);
	void set_bend_factor(float factor);
	void set_strength_factor(float factor);
	
	FadeType get_fade_type() const {return m_type;}
	QList<QPointF> get_control_points();
	
	bool is_bypassed() const {return m_bypass;}
	bool is_smaller_then(APILinkedListNode* node) {Q_UNUSED(node); return false;}
	
	void set_range(double pos);
	void set_mode(int m);

private:
	AudioClip*	m_clip;
	float 		m_bendFactor;
	float 		m_strenghtFactor;
	bool		m_bypass;
	int 		m_mode;
	int		m_raster;
	FadeType	m_type;
	QString		m_sType;
	QList<QPointF> 	m_controlPoints;
	
	QPointF get_curve_point(float f);
	void init();
	
public slots:
	void solve_node_positions();
	
	Command* toggle_bypass();
	Command* set_mode();
	Command* reset();
	Command* toggle_raster();
	
signals:
	void modeChanged();
	void bendValueChanged();
	void strengthValueChanged();
	void rasterChanged();
	void rangeChanged();
};

#endif

//eof

