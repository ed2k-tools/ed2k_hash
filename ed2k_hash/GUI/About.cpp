
/**
 * @file About.cpp
 * @brief Implementation of class About
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
// $Id: About.cpp,v 1.1 2003/05/25 11:55:33 fraca7 Exp $

#include "About.h"

#ifdef HAVE_CONFIG_H
#include "config.h"
#else
// Windows...
#define VERSION "0.4.0"
#endif

About::About()
    : Fl_Window(50, 50, 500, 350)
{
    _txt = new Fl_Multiline_Output(0, 0, 500, 320);
    _bt = new Fl_Button(220, 320, 60, 30, "OK");
    _bt->callback(&About::_ok_cb, this);
    end();

    _txt->value(
       "ed2k_hash V" VERSION "\n"
       "\n"
       "GUI (c) 2003 Jérôme Laheurte <fraca7@free.fr>\n"
       "Hash code (c) Tim-Philipp Müller <t.i.m@orange.net>\n"
       "Win32 Shell Extension code mainly (c) Smaller Animals Software\n"
       "\n"
       "This program is free software; you can redistribute it and/or modify\n"
       "it under the terms of the GNU General Public License as published by\n"
       "the Free Software Foundation; either version 2 of the License, or\n"
       "(at your option) any later version.\n"
       "\n"
       "This program is distributed in the hope that it will be useful,\n"
       "but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
       "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n"
       "GNU General Public License for more details.\n"
       "\n"
       "You should have received a copy of the GNU General Public License\n"
       "along with this program; if not, write to the Free Software\n"
       "Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA\n"
       );

    show();
}

About::~About()
{
    delete _txt;
    delete _bt;
}

void About::ok_cb(Fl_Widget*)
{
    hide();
}

void About::_ok_cb(Fl_Widget *w, void *p)
{
    ((About*)p)->ok_cb(w);
}

static const char* _rcsid_About __attribute__((unused)) = "$Id: About.cpp,v 1.1 2003/05/25 11:55:33 fraca7 Exp $";
