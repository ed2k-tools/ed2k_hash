
/**
 * @file Config.cpp
 * @brief Implementation of class Config
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
// $Id: Configuration.cpp,v 1.1 2003/05/25 11:55:33 fraca7 Exp $

#include "Configuration.h"

#include <FL/Fl_Valuator.H> // for FL_HORIZONTAL and FL_VERTICAL
#include <FL/Fl_Return_Button.H>
#include <FL/Fl_Check_Button.H>

#include <stdio.h>
#include <stdlib.h>

#include <iostream>

using namespace std;

int cf_htmlfull = 0;
int cf_htmllink = 0;
int cf_recursive = 0;

Config::Config()
    : Fl_Window(20, 20, 200, 100, "Configuration")
{
    _pck = new Fl_Pack(0, 0, 200, 160);
    _pck->type(FL_VERTICAL);
    _pck->begin();

    _bt_rec = new Fl_Check_Button(0, 0, 200, 20, "Recursive");
    _bt_html = new Fl_Check_Button(0, 20, 200, 20, "Produce HTML links");
    _bt_full = new Fl_Check_Button(0, 40, 200, 20, "Produce full links");

    _bt_html->when(FL_WHEN_CHANGED);

    _bt_rec->value(cf_recursive);
    _bt_full->value(cf_htmlfull);
    _bt_html->value(cf_htmllink);

    if (!_bt_html->value())
    {
       _bt_full->deactivate();
    }

    _bt_html->callback(&Config::_html_cb, this);

    _pck2 = new Fl_Pack(0, 160, 200, 40);
    _pck2->type(FL_HORIZONTAL);
    _pck2->begin();

    _bt_ok = new Fl_Return_Button(0, 0, 100, 40, "OK");
    _bt_cancel = new Fl_Button(100, 0, 100, 40, "Cancel");

    _bt_ok->callback(&Config::_ok_cb, this);
    _bt_cancel->callback(&Config::_cancel_cb, this);

    _pck2->end();
    _pck->end();
    end();

    show();
}

void Config::ok_cb(Fl_Widget *w)
{
    cf_recursive = _bt_rec->value();
    cf_htmllink = _bt_html->value();
    cf_htmlfull = _bt_full->value();
    hide();
}

void Config::cancel_cb(Fl_Widget *w)
{
    hide();
}

void Config::html_cb(Fl_Widget *w)
{
    if (_bt_html->value())
    {
       _bt_full->activate();
    }
    else
    {
       _bt_full->deactivate();
    }
}

void Config::_ok_cb(Fl_Widget *w, void *p)
{
    ((Config*)p)->ok_cb(w);
}

void Config::_cancel_cb(Fl_Widget *w, void *p)
{
    ((Config*)p)->cancel_cb(w);
}

void Config::_html_cb(Fl_Widget *w, void *p)
{
    ((Config*)p)->html_cb(w);
}

void load_config()
{
#ifdef __WIN32__
    HKEY hKey;
    if (RegOpenKeyEx(HKEY_CURRENT_USER,
                     "Software\\ed2k_hash",
                     0,
                     KEY_READ,
                     &hKey) == ERROR_SUCCESS)
    {
        DWORD dwType;
        DWORD dwValue;

        DWORD dwSize = sizeof(DWORD);
        if (RegQueryValueEx(hKey,
                            "htmllink",
                            0,
                            &dwType,
                            (LPBYTE)&dwValue,
                            &dwSize) == ERROR_SUCCESS)
        {
            cf_htmllink = dwValue;
        }

        dwSize = sizeof(DWORD);
        if (RegQueryValueEx(hKey,
                            "htmlfull",
                            0,
                            &dwType,
                            (LPBYTE)&dwValue,
                            &dwSize) == ERROR_SUCCESS)
        {
            cf_htmlfull = dwValue;
        }

        dwSize = sizeof(DWORD);
        if (RegQueryValueEx(hKey,
                            "recursive",
                            0,
                            &dwType,
                            (LPBYTE)&dwValue,
                            &dwSize) == ERROR_SUCCESS)
        {
            cf_recursive = dwValue;
        }

        RegCloseKey(hKey);
    }
#else
    const char *pth = getenv("HOME");
    if (!pth)
    {
       cerr << "Cannot find environment variable HOME !" << endl;
       return;
    }

    char *fn = new char[strlen(pth) + 9];
    sprintf(fn, "%s/.ed2krc", pth);
    FILE *fl = fopen(fn, "r");
    delete[] fn;

    if (!fl)
    {
       cerr << "Cannot open configuration file" << endl;
       return;
    }

    char bf[4096];
    char nm[4096];
    int v;
    while (fgets(bf, 4096, fl))
    {
       sscanf(bf, "%s %d", nm, &v);
       if (!strcmp(nm, "htmllink"))
       {
          cf_htmllink = v;
       }
       else if (!strcmp(nm, "htmlfull"))
       {
          cf_htmlfull = v;
       }
       else if (!strcmp(nm, "recursive"))
       {
          cf_recursive = v;
       }
    }
    fclose(fl);
#endif
}

void save_config()
{
#ifdef __WIN32__
    HKEY hKey;
    if (RegCreateKeyEx(HKEY_CURRENT_USER,
                       "Software\\ed2k_hash",
                       0,
                       NULL,
                       0,
                       KEY_WRITE,
                       NULL,
                       &hKey,
                       NULL) == ERROR_SUCCESS)
    {
        DWORD dwVal = cf_htmllink;
        RegSetValueEx(hKey, "htmllink", 0, REG_DWORD, (BYTE*)&dwVal, sizeof(DWORD));
        dwVal = cf_htmlfull;
        RegSetValueEx(hKey, "htmlfull", 0, REG_DWORD, (BYTE*)&dwVal, sizeof(DWORD));
        dwVal = cf_recursive;
        RegSetValueEx(hKey, "recursive", 0, REG_DWORD, (BYTE*)&dwVal, sizeof(DWORD));

        RegCloseKey(hKey);
    }
#else
    const char *pth = getenv("HOME");
    if (!pth)
    {
       cerr << "Cannot find environment variable HOME!" << endl;
       return;
    }

    char *fn = new char[strlen(pth) + 9];
    sprintf(fn, "%s/.ed2krc", pth);
    FILE *fl = fopen(fn, "w");
    delete[] fn;

    if (!fl)
    {
       cerr << "Cannot open configuration file" << endl;
       return;
    }

    fprintf(fl, "htmllink %d\n", cf_htmllink);
    fprintf(fl, "htmlfull %d\n", cf_htmlfull);
    fprintf(fl, "recursive %d\n", cf_recursive);
    fclose(fl);
#endif
}

static const char* _rcsid_Config __attribute__((unused)) = "$Id: Configuration.cpp,v 1.1 2003/05/25 11:55:33 fraca7 Exp $";
