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

#include <FL/Fl_Multi_Browser.h>
#include <FL/Fl_Menu_Bar.h>
#include <FL/Fl_Progress.h>

#include "JobQueue.h"

class MainWindow : public Fl_Window
{
  public:
    MainWindow();
    ~MainWindow();

    void ui_print(const char*);
    void ui_printerr(const char*);

    void setProgress(const char*, float);

    void addJob(const char*, OPT_STRUCT*);

  protected:
    void quit_cb(Fl_Widget*);
    void add_file_cb(Fl_Widget*);
    void add_dir_cb(Fl_Widget*);
    void edit_pref_cb(Fl_Widget*);

    // Selection
    void copy_cb(Fl_Widget*);
    void select_none_cb(Fl_Widget*);
    void select_all_cb(Fl_Widget*);
    void select_invert_cb(Fl_Widget*);
    void select_delete_cb(Fl_Widget*);

    // Tasks
    void cancel_cb(Fl_Widget*);
    void pause_cb(Fl_Widget*);
    void resume_cb(Fl_Widget*);

    void about_cb(Fl_Widget*);

  private:
    Fl_Multi_Browser *_list;
    Fl_Menu_Bar *_menu;
    Fl_Progress *_prog;

    JobQueue *_queue;

    char *_old_label;
    char *_label;

    static void _quit_cb(Fl_Widget*, void*);
    static void _add_file_cb(Fl_Widget*, void*);
    static void _add_dir_cb(Fl_Widget*, void*);
    static void _edit_pref_cb(Fl_Widget*, void*);

    static void _copy_cb(Fl_Widget*, void*);
    static void _select_none_cb(Fl_Widget*, void*);
    static void _select_all_cb(Fl_Widget*, void*);
    static void _select_invert_cb(Fl_Widget*, void*);
    static void _select_delete_cb(Fl_Widget*, void*);

    static void _cancel_cb(Fl_Widget*, void*);
    static void _pause_cb(Fl_Widget*, void*);
    static void _resume_cb(Fl_Widget*, void*);

    static void _about_cb(Fl_Widget*, void*);
};

#endif
