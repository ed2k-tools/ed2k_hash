
/**
 * @file Event.h
 * @brief Declaration of class Event
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

#ifndef _EVENT_H
#define _EVENT_H

#ifdef __WIN32__
#include <windows.h>
#else
#include <pthread.h>
#endif

class Event
{
  public:
    Event();
    ~Event();

    void set();
    void wait();

  private:
#ifdef __WIN32__
    HANDLE _evt;
#else
    pthread_cond_t _cnd;
    pthread_mutex_t _mtx;
#endif
};

#endif /* _EVENT_H */
