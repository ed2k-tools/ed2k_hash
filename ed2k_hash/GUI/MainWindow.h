/***************************************************************************
                          MainWindow.h  -  declaration of MainWindow class
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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <FL/Fl_Window.h>

#include <FL/Fl_Browser.h>
#include <FL/Fl_Menu_Bar.h>

class MainWindow : public Fl_Window
{
  public:
    MainWindow();
    ~MainWindow();

  protected:
    void quit_cb(Fl_Widget*);
    void add_file_cb(Fl_Widget*);
    void add_dir_cb(Fl_Widget*);

  private:
    Fl_Browser *_list;
    Fl_Menu_Bar *_menu;
    
    static void _quit_cb(Fl_Widget*, void*);
    static void _add_file_cb(Fl_Widget*, void*);
    static void _add_dir_cb(Fl_Widget*, void*);
};

#endif

