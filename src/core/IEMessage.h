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
 
    $Id: IEMessage.h,v 1.1 2006/04/20 14:51:39 r_sijrier Exp $
*/

#ifndef IEMESSAGE_H
#define IEMESSAGE_H


/// store information about jmb action performed
class IEMessage
{
public:

        /// Constructs a jmb message to be brodcast with standard values
        IEMessage();

        /// Constructs a jmb message to be broadcast.
        /// @param beginHold true if this message was broadcast when beggining a hold fact
        /// @param collectedNumber if any
        /// @param mousex x where it happened
        /// @param mousey y where it happened
        IEMessage(bool beginHold, int collectedNumber, int mousex, int mousey);
        IEMessage(int mousex, int mousey);
        ~IEMessage();

        int mousex();
        int mousey();

        bool beginHold;
        int collectedNumber;
        int posx;
        int posy;
};

#endif

// eof

