
/**
 * @file JobQueue.h
 * @brief Declaration of class JobQueue
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

#ifndef _JOBQUEUE_H
#define _JOBQUEUE_H

#include "Thread.h"
#include "Mutex.h"
#include "Semaphore.h"

#include <list>
#include <string>

/* structure to store options */

typedef struct
{
      int htmllink;
      int htmlfull;
      int recursive;
} OPT_STRUCT;

#ifdef __WIN32__

typedef struct
{
    OPT_STRUCT opts;
    char str[1];
} MSG_DATA_STRUCT;

#endif

class JobQueue : public Thread
{
  public:
    JobQueue();
    virtual ~JobQueue();

    void addJob(const char*,        // filename
                OPT_STRUCT*);       // options for this file

    void cancel();
    void pause();
    void resume();

    bool isPaused();

    int run();

  private:
    Mutex _mtx;
    Semaphore _sem;
    std::list<std::pair<std::string, OPT_STRUCT> > _queue;
};

#endif /* _JOBQUEUE_H */
