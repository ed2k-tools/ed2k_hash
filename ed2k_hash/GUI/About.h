
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
// $Id$

#ifndef _ABOUT_H
#define _ABOUT_H

#include <FL/Fl_Window.h>
#include <FL/Fl_Button.h>
#include <FL/Fl_Multiline_Output.h>

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
