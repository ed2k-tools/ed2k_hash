
/**
 * @file ui.cpp
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
// $Id: ui.cpp,v 1.2 2003/05/25 13:17:59 fraca7 Exp $

extern "C" {

#include "ui.h"
#include "options.h"
#include "processfile.h"

}

#include "MainWindow.h"
#include "Event.h"

#include <string.h> // memset
#include <stdarg.h>
#include <stdio.h>

extern MainWindow* AppWindow; /* in main.cpp */
extern bool _cancelled; /* in JobQueue.cpp */
extern bool _paused; /* in JobQueue.cpp */
extern Event _resume_event; /* in JobQueue.cpp */

int ui_init()
{
    return 1;
}

int ui_cleanup()
{
    return 1;
}

int ui_print(const char* fmt, ...)
{
    int ret = 0;

    static char bf[4096];
    va_list args;

    va_start(args,fmt);
    ret = vsnprintf(bf,4096,fmt,args);
    va_end(args);

    AppWindow->ui_print(bf);
    return ret;
}

int ui_printerr(const char* fmt, ...)
{
    int ret = 0;

    static char bf[4096];
    va_list args;

    va_start(args,fmt);
    ret = vsnprintf(bf,4096,fmt,args);
    va_end(args);

    AppWindow->ui_printerr(bf);
    return ret;
}

int ui_setoptions(int argc, char *argv[])
{
    return 1;
}

/* Copy/past from cl_ui.c
 */

int ui_run (SList *filelist)
{
    SList *node = filelist;

    /* no files specified on the command line? show help and exit */
    if (!filelist)
    {
       ui_printerr ("You haven't specified any file(s) or directory to hash, have you?\n");
       options_print_help_screen_and_exit ();
    }

    while (node)
    {
       if (node->data)
       {
          fileinfo	info;
          
          if ((option_debug)||(option_verbose))
             ui_print ("hashing file          : %s\n", (char*)node->data);

          /* note: if process_file() returns 0, that might mean that an error
             has occured (e.g. filesize==0 or I/O error), _OR_ that ui_update()
             has requested us to stop processing. You will need to deal with
             that via some global variabel or something here in order to
             distinguish those two events for a GUI
          */
          memset (&info, 0x00, sizeof(info));
          if (process_file ((char*)node->data, &info))
          {
             if (!option_htmllink)
             {
                ui_print ("ed2k://|file|%s|%u|%s|\t@b%s",
                          info.basename,
                          info.size,
                          info.ed2k_hash_str,
                          info.basename);
             } else {
                ui_print ("<a href=\"ed2k://|file|%s|%u|%s|\">%s</a>\t@b%s",
                          info.basename,
                          info.size,
                          info.ed2k_hash_str,
                          (option_htmlfull) ? info.filepath : info.basename,
                          info.basename);
             }
          } else ui_printerr ("while processing file '%s'\n", (char*)node->data);
          process_file_free_info_structure_content(&info);
       }
       node = (SList*)node->next;
    }

    return 1;
}

int ui_update(char *filepath, unsigned int size, unsigned int done)
{
    if (!_cancelled)
    {
       if (_paused)
       {
          _resume_event.wait();
       }
       else
       {
          AppWindow->setProgress(filepath, 100.0 * done / size);
       }

       return !_cancelled;
    }

    return 0;
}

static const char* _rcsid_ui __attribute__((unused)) = "$Id: ui.cpp,v 1.2 2003/05/25 13:17:59 fraca7 Exp $";
