/***************************************************************************
                  win32main.h  -  defines & global variables
                             -------------------
    begin                : Sun Jan 12 12:35:45  2003
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

#ifndef WIN32MAIN_H
#define WIN32MAIN_H

#include <windows.h>

/* Files */
#define IDM_QUIT          200
#define IDM_ADDFILES      201
#define IDM_ADDDIR        202
#define IDM_RECURSE       203
#define IDM_CANCEL        204

/* Edit */

#define IDM_COPY          300
#define IDM_CLEAR         301
#define IDM_CONFIG        302

/* Help */
#define IDM_ABOUT         400

/* Config */
#define IDB_CF_RECURSE    500
#define IDB_CF_HTML       501
#define IDB_CF_FULL       502

/* globals */
extern HWND hMainWindow;
extern HWND hStatusBar;
extern HWND hFileList;

/* messages */
#define WM_USER_THREAD_TERMINATED                WM_USER + 1

#endif
