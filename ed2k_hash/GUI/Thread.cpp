
/**
 * @file Thread.cpp
 * @brief Implementation of class Thread
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

#include "Thread.h"

#ifdef __WIN32__
static DWORD WINAPI thread_routine(LPVOID);
#else
static void* pthread_routine(void*);
#endif

Thread::Thread()
{
#ifdef __WIN32__
    // nothing
#else
    // nothing
#endif
}

Thread::~Thread()
{
#ifdef __WIN32__
    CloseHandle(_thr);
#else
    // nothing
#endif
}

void Thread::start()
{
#ifdef __WIN32__
    DWORD id;
    CreateThread(NULL,
                 0,
                 (LPTHREAD_START_ROUTINE)&thread_routine,
                 (LPVOID)this,
                 0,
                 &id);
#else
    pthread_create(&_thr, NULL, &pthread_routine, this);
#endif
}

void Thread::join()
{
#ifdef __WIN32__
    WaitForSingleObject(_thr, INFINITE);
#else
    pthread_join(_thr, NULL);
#endif
}

#ifdef __WIN32__
DWORD WINAPI thread_routine(LPVOID p)
{
    return (DWORD)(((Thread*)p)->run());
}
#else
void* pthread_routine(void *p)
{
    return (void*)(((Thread*)p)->run());
}
#endif

static const char* _rcsid_Thread __attribute__((unused)) = "$Id$";
