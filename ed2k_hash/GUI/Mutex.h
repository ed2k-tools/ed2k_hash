
/**
 * @file Mutex.h
 * @brief Declaration of class Mutex
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

#ifndef _MUTEX_H
#define _MUTEX_H

#ifdef __WIN32__
#include <windows.h>
#else
#include <pthread.h>
#endif

class Mutex
{
  public:
    Mutex();
    ~Mutex();

    void lock();
    void unlock();

  private:
#ifdef __WIN32__
    HANDLE _mtx;
#else
    pthread_mutex_t _mtx;
#endif
};

#endif /* _MUTEX_H */
