
/**
 * @file JobQueue.cpp
 * @brief Implementation of class JobQueue
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

#include "JobQueue.h"
#include "MainWindow.h"
#include "Event.h"
#include "options.h"

using namespace std;

extern "C" int main_entry_point(int, char**); /* in oldmain.cpp */
extern MainWindow *AppWindow; /* in main.cpp */

bool _cancelled = false;
bool _paused = false;
Event _resume_event;

JobQueue::JobQueue()
    : _mtx(),
      _sem()
{
}

JobQueue::~JobQueue()
{
}

void JobQueue::addJob(const char *fn, OPT_STRUCT *opt)
{
    _mtx.lock();
    _queue.push_back(make_pair(fn, *opt));
    _cancelled = false;
    _mtx.unlock();
    _sem.release();
}

void JobQueue::cancel()
{
    _mtx.lock();
    _cancelled = true;
    _paused = false;
    _resume_event.set();
    _mtx.unlock();
    _sem.release();
}

void JobQueue::pause()
{
    _paused = true;
}

void JobQueue::resume()
{
    _paused = false;
    _resume_event.set();
}

bool JobQueue::isPaused()
{
    return _paused;
}

int JobQueue::run()
{
    while (true)
    {
       _sem.acquire();
       _mtx.lock();

       if (_cancelled)
       {
          _queue.clear();
          _mtx.unlock();
       }
       else
       {
          if (_queue.size() > 0)
          {
             // Process one file
             pair<string, OPT_STRUCT> p = *(_queue.begin());
             _queue.erase(_queue.begin());
             _mtx.unlock();

             option_htmllink = p.second.htmllink;
             option_htmlfull = p.second.htmlfull;
             option_recursive = p.second.recursive;

             char **argv = (char**)malloc(2 * sizeof(char*));
             argv[1] = strdup(p.first.c_str());
             argv[0] = 0;
             main_entry_point(2, argv);
             free(argv[0]);
             free(argv);
          }
          else
          {
             _mtx.unlock();
          }
       }

       AppWindow->setProgress("Ready", 0);
    }

    return 0;
}

static const char* _rcsid_JobQueue __attribute__((unused)) = "$Id$";
