
/**
 * @file Configuration.h
 * @brief Declaration of class Config
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
// $Id: Configuration.h,v 1.2 2003/06/15 14:54:08 fraca7 Exp $

#ifndef _CONFIGURATION_H
#define _CONFIGURATION_H

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#ifdef HAVE_FL_WITH_LC_H
# include <FL/Fl_Button.h>
# include <FL/Fl_Window.h>
# include <FL/Fl_Pack.h>
#else
# include <FL/Fl_Button.H>
# include <FL/Fl_Window.H>
# include <FL/Fl_Pack.H>
#endif

// Globals
extern int cf_htmllink;
extern int cf_htmlfull;
extern int cf_recursive;

class Config : public Fl_Window
{
  public:
    Config();

    void ok_cb(Fl_Widget*);
    void cancel_cb(Fl_Widget*);
    void html_cb(Fl_Widget*);

  private:
    Fl_Pack *_pck;
    Fl_Pack *_pck2;
    Fl_Button *_bt_html;
    Fl_Button *_bt_full;
    Fl_Button *_bt_rec;
    Fl_Button *_bt_ok;
    Fl_Button *_bt_cancel;

    static void _ok_cb(Fl_Widget*, void*);
    static void _cancel_cb(Fl_Widget*, void*);
    static void _html_cb(Fl_Widget*, void*);
};

void load_config(void);
void save_config(void);

#endif /* _CONFIGURATION_H */
