
/**
 * @file About.h
 * @brief Declaration of class About
 */

/**********************************************************************

  Created: 25 May 2003

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
// $Id: About.h,v 1.2 2003/06/15 14:54:08 fraca7 Exp $

#ifndef _ABOUT_H
#define _ABOUT_H

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#ifdef HAVE_FL_WITH_LC_H
# include <FL/Fl_Window.h>
# include <FL/Fl_Button.h>
# include <FL/Fl_Multiline_Output.h>
#else
# include <FL/Fl_Window.H>
# include <FL/Fl_Button.H>
# include <FL/Fl_Multiline_Output.H>
#endif

class About : public Fl_Window
{
  public:
    About();
    ~About();

    void ok_cb(Fl_Widget*);

  private:
    Fl_Button *_bt;
    Fl_Multiline_Output *_txt;

    static void _ok_cb(Fl_Widget*, void*);
};

#endif /* _ABOUT_H */



