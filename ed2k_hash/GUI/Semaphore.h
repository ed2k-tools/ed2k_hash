
/**
 * @file Semaphore.h
 * @brief Declaration of class Semaphore
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

#ifndef _SEMAPHORECL_H
#define _SEMAPHORECL_H

#ifdef __WIN32__
#include <windows.h>
#else
#include <semaphore.h>
#endif

class Semaphore
{
  public:
    Semaphore(unsigned int = 0);
    ~Semaphore();

    void acquire();
    void release();

  private:
#ifdef __WIN32__
    HANDLE _sem;
#else
    sem_t _sem;
#endif
};

#endif /* _SEMAPHORE_H */
