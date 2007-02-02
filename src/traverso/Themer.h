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
 
    $Id: Themer.h,v 1.1 2007/02/02 00:02:24 r_sijrier Exp $
*/

#ifndef COLORMANAGER_H
#define COLORMANAGER_H

#include <QObject>
#include <QColor>
#include <QHash>
#include <QString>
#include <QFileSystemWatcher>

class Themer : public QObject
{
	Q_OBJECT
public:
        void save();
        void load();
        
        void set_theme_path(const QString& path);
        
	QColor get_color(const QString& name) const;
        QFont get_font(const QString& fontname) const;
        QVariant get_property(const QString& propertyname, const QVariant& defaultValue=0) const;

private:
        Themer();
        Themer(const Themer&) : QObject() {}

        QHash<QString , QColor>		m_colors;
	QHash<QString, QFont>		m_fonts;
	QHash<QString, QVariant> 	m_properties;
        QFileSystemWatcher		m_watcher;
        QString				m_themefile;

        // allow this function to create one instance
        friend Themer& themer();
       
private slots:
	void reload_on_themefile_change(const QString&);
	
signals:
	void themeLoaded();
};

// use this function to get the Colormanager object
Themer& themer();

#endif
