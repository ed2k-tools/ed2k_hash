/***************************************************************************
                          uifuncs.c  -  ui functions
                             -------------------
    begin                : Sun Jan 12 12:46:13  2003
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

#if defined(__WIN32)

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#include <windows.h>
#include <commctrl.h>

#include "global.h"

#include "cl_ui.h"
#include "options.h"
#include "processfile.h"
#include "win32main.h"

extern int main_entry_point(int, char**); /* in main.c */

static int cancelled = 0;
static HANDLE hThread = NULL;

static HANDLE hMutex = NULL;

/* The queue is filled by the main thread and consumed
   by the hashing thread. The latter stops when the queue is
   empty.
*/

static char** _queue = NULL;
static int _queue_bl = 0;
static int _queue_sz = 0;

static void launch_thread();

/* ui_init
 *
 */

int
ui_init (void)
{
    if (!hMutex)
    {
       hMutex = CreateMutex(NULL, FALSE, NULL);
    }

    return 1;
}


/* ui_cleanup
 *
 */

int
ui_cleanup (void)
{
    return 1;
}


/* ui_print
 *
 * displays a normal message to the user, takes arguments like printf()
 *
 */

int
ui_print (const char *format, ...)
{
    int		ret = 0;

    /* where's vasprintf() ? I wish I had the GNU libc... */
    char bf[4096];
    va_list args;

    if (!option_quiet)
    {
       va_start(args,format);
       ret = vsnprintf(bf,4096,format,args);
       va_end(args);

       SendMessage(hStatusBar,
                   SB_SETTEXT,
                   (WPARAM)(255 | SBT_POPOUT),
                   (LPARAM)bf);
    }

    return ret;
}


/* ui_printerr
 *
 * displays an error message to the user, takes arguments like printf()
 *
 */

int
ui_printerr (const char *format, ...)
{
    va_list  args;
    char	*errformat;
    int		 ret;
    char         bf[4096];

    if (option_silent)
       return 0;

    if (!format)
       return -1;

    errformat = (char*)malloc(strlen(PRINTERR_PREFIX)+strlen(format)+2);
    if (!errformat)
       return -1;

    snprintf (errformat, strlen(PRINTERR_PREFIX)+strlen(format)+1, "%s%s",
              PRINTERR_PREFIX, format);

    va_start(args,format);
    ret = vsnprintf(bf,4096,errformat,args);
    va_end(args);

    MessageBox(hMainWindow,
               bf,
               PACKAGE,
               MB_OK);

    free(errformat);

    return ret;
}


/* ui_setoptions
 *
 * parses the options given on the command line
 *
 * returns 0 on failure
 * returns the first argument number that is presumably a filename or directory name
 *
 */

int
ui_setoptions (int argc, char *argv[])
{
    /* dirty trick, argv is specifically build to hold only arguments */
    return 1;
}


/* ui_run
 *
 * is called with a list of files passed via the command line and processes each
 * of those files. We do it this way so the ui stuff can be callback mechanisms as
 * used for GUI stuff as well (so files could be selected via GUI instead of CL)
 *
 * returns 0 on error, otherwise 1
 *
 */

int
ui_run (SList *filelist)
{
	SList *node = filelist;

	/* no files specified on the command line? show help and exit */
	if (!filelist)
	{
		ui_printerr ("You haven't specified any file(s) or directory to hash, have you?\n");
		options_print_help_screen_and_exit ();
	}

	while (node && !cancelled)
	{
		if (node->data)
		{
			fileinfo	info;

			if ((option_debug)||(option_verbose))
				ui_print ("hashing file          : %s\n", (char*)node->data);

			memset (&info, 0x00, sizeof(info));
			if (process_file ((char*)node->data, &info))
			{
                                char *bf = (char*)malloc(30 + 
                                                         strlen(info.basename) +
                                                         1 + 32 + 1 +
                                                         strlen(info.ed2k_hash_str) + 3 +
                                                         strlen(info.filepath));
                                LVITEM item;
                                int idx;

				if (!option_htmllink)
				{
                                        sprintf(bf,
                                                "ed2k://|file|%s|%u|%s|",
                                                info.basename,
                                                info.size,
                                                info.ed2k_hash_str);
				} else {
                                        sprintf(bf,
                                                "<a href=\"ed2k://|file|%s|%u|%s|\">%s</a>",
                                                info.basename,
                                                info.size,
                                                info.ed2k_hash_str,
                                                (option_htmlfull) ? info.filepath : info.basename);
				}

                                memset(&item, 0, sizeof(item));
                                item.mask = LVIF_TEXT|LVIF_PARAM;
                                item.pszText = bf;
                                item.lParam = (LPARAM)bf;
                                idx = ListView_InsertItem(hFileList,
                                                          &item);

                                item.iSubItem = 1;
                                item.mask = LVIF_TEXT;
                                item.pszText = info.ed2k_hash_str;
                                item.iItem = idx;
                                ListView_SetItem(hFileList, &item);

                                item.iSubItem = 2;
                                item.pszText = info.basename;
                                ListView_SetItem(hFileList, &item);

			} else if (!cancelled) ui_printerr ("while processing file '%s'\n", (char*)node->data);
			process_file_free_info_structure_content(&info);
		}
		node = node->next;
	}

	return 1;
}


/* ui_update
 *
 * called regularly during hashing (so GUI clients can update the window and process
 * messages etc.).
 *
 * returns 0 if hashing should stop, otherwise 1 (implement this in ui_run() via a global variable)
 *
 */

int
ui_update (char *filepath, unsigned int size, unsigned int done)
{
    return !cancelled;
}

/*
 * cancels the running thread
 */

void cancel_task()
{
    cancelled = 1;
}

static void add_to_queue(const char* fn)
{
    /* WARNING synchronization must be done outside of this
       function, for obvious performance reasons */

    if (_queue_sz + 1 >= _queue_bl * 16)
    {
       _queue = (char**)realloc(_queue, (++_queue_bl) * 16 * sizeof(char*));
    }

    _queue[_queue_sz++] = strdup(fn);
}

/*
 * add_file
 * Handles a single file/directory
 */

void add_file(const char *name)
{
    /* If the filename comes from the shell, it's in the old 8.3
     * format... */

    char *rname = NULL;
    WIN32_FIND_DATA d;
    HANDLE h = FindFirstFile(name, &d);
    if (h != INVALID_HANDLE_VALUE)
    {
       int k = strlen(name) - 1;
       while ((k>0) && (name[k] != '\\')) --k;
       if (k < 0)
       {
          rname = strdup(d.cFileName);
       }
       else
       {
          rname = (char*)malloc(k + 1 + strlen(d.cFileName) + 1);
          memcpy(rname, name, k + 1);
          strcpy(rname + k + 1, d.cFileName);
       }

       FindClose(h);
    }
    else
    {
       /* Should not happen... */
       rname = strdup(name);
    }

    ui_init();

    WaitForSingleObject(hMutex, INFINITE);
    add_to_queue(rname);

    if (!hThread)
    {
       launch_thread();
    }

    ReleaseMutex(hMutex);

    free(rname);
}

/*
 * add_files
 * Handles several files. The first parameter must be the buffer filled by
 * GetOpenFileName(), the second the nFileOffset field
 */

void add_files(const char *bf, int off)
{
    ui_init();

    WaitForSingleObject(hMutex, INFINITE);

    /*
      Another dirty thing: the buffer contains
      1) the full path to a file if the user selected only 1 file
      2) the base directory, and NULL-separated file names, ended
         with double-NULL if several files were selected :(
     */

    if (bf[off-1] == 0)
    {
       /* multiple selection */
       int k = 0;
       const char *dir = bf;

       while (bf[k]) ++k; /* directory */
       ++k;

       while (bf[k])
       {
          char *argv = (char*)malloc(strlen(dir) + 1 + strlen(bf+k) + 1);
          strcpy(argv, dir);
          if (argv[strlen(dir)-1] != '\\')
          {
             strcat(argv, "\\");
          }
          strcat(argv, bf+k);

          add_to_queue(argv);
          free(argv);

          while (bf[k]) ++k;
          ++k;
       }
    }
    else
    {
       add_to_queue(bf);
    }

    if (!hThread)
    {
       launch_thread();
    }

    ReleaseMutex(hMutex);
}

LPVOID ThreadProc(LPVOID *foo)
{
    while (!cancelled)
    {
       char **argv;
       int argc;

       WaitForSingleObject(hMutex, INFINITE);

       if (_queue_sz == 0)
       {
          ReleaseMutex(hMutex);

          PostMessage(hMainWindow,
                      WM_USER_THREAD_TERMINATED,
                      0,
                      0);
          break;
       }

       argv = (char**)malloc((_queue_sz + 1) * sizeof(char*));
       argv[0] = PACKAGE;
       memcpy(argv + 1, _queue, _queue_sz * sizeof(char*));
       argc = _queue_sz + 1;
       _queue_sz = 0;
       ReleaseMutex(hMutex);

       main_entry_point(argc, argv);
       while (argc > 1)
       {
          free(argv[--argc]);
       }
       free(argv);
    }

    if (cancelled)
    {
       PostMessage(hMainWindow,
                   WM_USER_THREAD_TERMINATED,
                   0,
                   0);
    }

    return NULL;
}

void launch_thread()
{
    cancelled = 0;

    DWORD id;
    hThread = CreateThread(NULL,
                           0,
                           (LPTHREAD_START_ROUTINE)ThreadProc,
                           NULL,
                           0,
                           &id);

    if (!hThread)
    {
       MessageBox(hMainWindow,
                  "Could not create thread.",
                  PACKAGE,
                  MB_OK);
    }
}

int is_running()
{
    return (hThread != NULL);
}

void cleanup_thread()
{
    if (hThread)
    {
       int k;

       WaitForSingleObject(hThread, INFINITE);
       CloseHandle(hThread);
       hThread = NULL;
       for (k = 0; k<_queue_sz; ++k) free(_queue[k]);
       _queue_sz = 0;
    }
}

#endif

