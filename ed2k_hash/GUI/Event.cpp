
/**
 * @file Event.cpp
 * @brief Implementation of class Event
 */

/**********************************************************************

  Created: 18 May 2003

    Copyright (C) 2003 fraca7@free.fr

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License as
    published by the Free Software Foundation; either version 2 of the
    License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
    02111-1307 USA

**********************************************************************/
// $Id$

#include "Event.h"

Event::Event()
{
#ifdef __WIN32__
    _evt = CreateEvent(NULL, FALSE, FALSE, NULL);
#else
    pthread_mutex_init(&_mtx, NULL);
    pthread_cond_init(&_cnd, NULL);
#endif
}

Event::~Event()
{
#ifdef __WIN32__
    CloseHandle(_evt);
#else
    pthread_cond_destroy(&_cnd);
#endif
}

void Event::set()
{
#ifdef __WIN32__
    SetEvent(_evt);
#else
    pthread_cond_signal(&_cnd);
#endif
}

void Event::wait()
{
#ifdef __WIN32__
    WaitForSingleObject(_evt, INFINITE);
#else
    pthread_cond_wait(&_cnd, &_mtx);
#endif
}

static const char* _rcsid_Event __attribute__((unused)) = "$Id$";
