/***************************************************************************
                          reg.h  -  registry convenience functions
                             -------------------
    begin                : Sun Jan 12 14:02:43  2003
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

#ifndef _REG_H
#define _REG_H

#include <windows.h>

/* Opens a key, creating it if necessary (under HKCU) */
HKEY open_key(const char*); /* key name */

/* Closes an open key */
void close_key(HKEY);

/* Write an integer value */
void write_integer(HKEY,
                   const char*,     /* value name */
                   int);            /* value */

/* Reads an integer value */
int read_integer(HKEY,
                 const char*,       /* value name */
                 int);              /* default value */

#endif
