
/**
 * @file Semaphore.cpp
 * @brief Implementation of class Semaphore
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

#include "Semaphore.h"

Semaphore::Semaphore(unsigned int i)
{
#ifdef __WIN32__
    _sem = CreateSemaphore(NULL, i, 10000, NULL);
#else
    sem_init(&_sem, 0, i);
#endif
}

Semaphore::~Semaphore()
{
#ifdef __WIN32__
    CloseHandle(_sem);
#else
    sem_destroy(&_sem);
#endif
}

void Semaphore::acquire()
{
#ifdef __WIN32__
    WaitForSingleObject(_sem, INFINITE);
#else
    sem_wait(&_sem);
#endif
}

void Semaphore::release()
{
#ifdef __WIN32__
    ReleaseSemaphore(_sem, 1, NULL);
#else
    sem_post(&_sem);
#endif
}

static const char* _rcsid_Semaphore __attribute__((unused)) = "$Id$";
