/***************************************************************************
                          param.c  -  description
                             -------------------
    begin                : Sun Jan 12 14:14:50  2003
    copyright            : (C) 2003 Jérome Laheurte
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

#include "options.h"
#include "param.h"
#include "reg.h"

int			option_htmllink = 0;	/* wrap html link around ed2k-link */
int			option_htmlfull = 0;	/* print path component as well in html link */
int			option_verbose = 0;		/* be verbose */
int			option_quiet = 0;		/* only show error messages and the results */
int			option_silent = 0;		/* do not even show error messages! */
int			option_debug = 0;		/* show debug messages */
int			option_recursive = 0;	/* process all subdirectories of directories as well */
int			option_excludedirs = 0;	/* exclude directories */
int			option_waitwhenfinito = 0; /* wait for keystroke when finished */

/* options_process_cl_argument
 *
 * processes the command line argument given
 * and increases the argument counter if there are extra args to an option (and only then!)
 *
 * returns 0 on error
 *
 */

int
options_process_cl_argument (char *argv[], int *i)
{
    return *i;
}

/* options_print_help_screen
 *
 * prints help screen and exits
 *
 */

void
options_print_help_screen_and_exit (void)
{
    // TODO
}

void load_options_from_registry()
{
    HKEY hKey = open_key("Software\\ed2k_hash");
    option_htmllink = read_integer(hKey, "htmllink", 0);
    option_htmlfull = read_integer(hKey, "htmlfull", 0);
    option_recursive = read_integer(hKey, "recursive", 1);
    close_key(hKey);
}

void save_options_to_registry()
{
    HKEY hKey = open_key("Software\\ed2k_hash");
    write_integer(hKey, "htmllink", option_htmllink);
    write_integer(hKey, "htmlfull", option_htmlfull);
    write_integer(hKey, "recursive", option_recursive);
    close_key(hKey);
}

#endif

