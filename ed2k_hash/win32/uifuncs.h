/***************************************************************************
                          uifuncs.h  -  description
                             -------------------
    begin                : Sun Jan 12 15:50:55  2003
    copyright            : (C) 2003 by Jérome Laheurte
    email                : fraca7@free.fr
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef _UIFUNCS_H
#define _UIFUNCS_H

/* "standard" ui funcs are still declared in cl_ui.h */

/* Cancels the current task */
void cancel_task(void);

/* Adds a single file/directory */
void add_file(const char*);

/* Adds multiple files. The first parameter is the buffer in the
   OPENFILENAME struct, the second is the nFileOffset member */
void add_files(const char*, int);

/* Returns 0 if the thread isn't running, !=0 otherwise */
int is_running(void);

/* Does some cleanup */
void cleanup_thread(void);

#endif
