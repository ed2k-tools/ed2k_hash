/***************************************************************************
                          MainWindow.cpp  -  implementation of class MainWindow
                             -------------------
    begin                : Sat May 17 2003
    copyright            : (C) 2003 by Jérôme Laheurte
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

#include "options.h"
#include "MainWindow.h"

#include <FL/Fl_Menu_Bar.h>
#include <FL/Fl_Menu_Item.h>

MainWindow::MainWindow()
    : Fl_Window(20, 20, 400, 300, "Ed2k Hash GUI V" VERSION)
{
    static Fl_Menu_Item menu_items[] = {
      { "&File", FL_ALT + 'f', 0, 0, FL_SUBMENU },
        { "Add &file...", 0, &MainWindow::_add_file_cb, this },
        { "Add &directory...", 0,  &MainWindow::_add_dir_cb, this },
        { "", 0, 0, 0, FL_MENU_DIVIDER | FL_MENU_INACTIVE },
        { "&Quit", 0, &MainWindow::_quit_cb, this },
        { 0 },
      { 0 }
    };
    
    _list = new Fl_Browser(0, 30, 400, 270);
    _menu = new Fl_Menu_Bar(0, 0, 400, 30);
    
    end();
    resizable(_list);
    _menu->menu(menu_items);
    show();
}

MainWindow::~MainWindow()
{
    delete _menu;
    delete _list;
}

void MainWindow::quit_cb(Fl_Widget *w)
{
    exit(0);
}

void MainWindow::add_file_cb(Fl_Widget *w)
{
    // TODO
}

void MainWindow::add_dir_cb(Fl_Widget *w)
{
    // TODO
}

//==============================================================================

void MainWindow::_quit_cb(Fl_Widget *w, void *p)
{
    ((MainWindow*)p)->quit_cb(w);
}

void MainWindow::_add_file_cb(Fl_Widget *w, void *p)
{
    ((MainWindow*)p)->add_file_cb(w);
}

void MainWindow::_add_dir_cb(Fl_Widget *w, void *p)
{
    ((MainWindow*)p)->add_dir_cb(w);
}

