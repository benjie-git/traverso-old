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
 
    $Id: IEMessage.cpp,v 1.1 2006/04/20 14:51:39 r_sijrier Exp $
*/

#include "IEMessage.h"

// Always put me below _all_ includes, this is needed
// in case we run with memory leak detection enabled!
#include "Debugger.h"


IEMessage::IEMessage()
{
        IEMessage(false, -1, -1, -1);
}

IEMessage::IEMessage(int mousex, int mousey)
{
        IEMessage(false, -1, mousex, mousey);
}

IEMessage::IEMessage( bool beginHold, int collectedNumber, int mousex, int mousey )
{
        this->beginHold = beginHold;
        this->collectedNumber = collectedNumber;
        this->posx = mousex;
        this->posy = mousey;
}

IEMessage::~IEMessage()
{}

int IEMessage::mousex()
{
        return posx;
}

int IEMessage::mousey()
{
        return posy;
}


// EOF


