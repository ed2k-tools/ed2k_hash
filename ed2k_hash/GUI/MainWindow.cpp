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
#include "Configuration.h"
#include "About.h"

#include <FL/Fl_Menu_Bar.h>
#include <FL/Fl_Menu_Item.h>
#include <FL/Fl_File_Chooser.h>
#include <FL/fl_ask.h>

#ifdef __WIN32__
#include <FL/x.h>
static int handle_unknown_message(int);
#endif

MainWindow::MainWindow()
    : Fl_Window(100, 100, 600, 300, "Ed2k Hash GUI V" VERSION),
      _old_label(0),
      _label(0)
{
    static Fl_Menu_Item menu_items[] = {
      { "&File", FL_ALT + 'f', 0, 0, FL_SUBMENU },
        { "Add &files...", FL_CTRL + FL_ALT + 'f', &MainWindow::_add_file_cb, this },
        { "Add &directory...", FL_CTRL + FL_ALT + 'd',  &MainWindow::_add_dir_cb, this },
        { "", 0, 0, 0, FL_MENU_DIVIDER | FL_MENU_INACTIVE },
        { "&Quit", FL_CTRL + 'q', &MainWindow::_quit_cb, this },
        { 0 },
      { "&Edit", FL_ALT + 'e', 0, 0, FL_SUBMENU },
        { "&Copy", FL_CTRL + 'c', &MainWindow::_copy_cb, this },
        { "Select &None", FL_CTRL + 'n', &MainWindow::_select_none_cb, this },
        { "Select &All", FL_CTRL + 'a', &MainWindow::_select_all_cb, this },
        { "&Invert selection", FL_CTRL + 'i', &MainWindow::_select_invert_cb, this },
        { "&Delete selection", FL_CTRL + 'd', &MainWindow::_select_delete_cb, this },
        { 0 },
      { "&Tasks", FL_ALT + 't', 0, 0, FL_SUBMENU },
        { "&Cancel", FL_ALT + FL_CTRL + 'c', &MainWindow::_cancel_cb, this },
        { "&Pause", FL_ALT + FL_CTRL + 'p', &MainWindow::_pause_cb, this },
        { "&Resume", FL_ALT + FL_CTRL + 'r', &MainWindow::_resume_cb, this },
        { 0 },
      { "&Preferences", FL_ALT + 'p', 0, 0, FL_SUBMENU },
        { "&Edit", FL_CTRL + 'p', &MainWindow::_edit_pref_cb, this },
        { 0 },
      { "&Help", FL_ALT + 'h', 0, 0, FL_SUBMENU },
        { "&About", 0, &MainWindow::_about_cb, this },
        { 0 },
      { 0 }
    };
    
    _list = new Fl_Multi_Browser(0, 30, 600, 250);
    _menu = new Fl_Menu_Bar(0, 0, 600, 30);
    _prog = new Fl_Progress(0, 280, 600, 20, "Ready...");

    _prog->minimum(0);
    _prog->maximum(100);

    end();
    resizable(_list);
    _menu->menu(menu_items);
    show();

    _queue = new JobQueue();
    _queue->start();

    #ifdef __WIN32__
    Fl::add_handler(&handle_unknown_message);
    #endif
}

MainWindow::~MainWindow()
{
    delete _menu;
    delete _list;
    _queue->cancel();
    _queue->join();
    delete _queue;
}

void MainWindow::addJob(const char *fn, OPT_STRUCT *opt)
{
    _queue->addJob(fn, opt);
}

void MainWindow::setProgress(const char *l, float d)
{
    Fl::lock();
    if (_label)
       free(_label);
    _label = strdup(l);

    _prog->value(d);
    _prog->label(_label);
    Fl::unlock();
    Fl::awake(_prog);
}

void MainWindow::quit_cb(Fl_Widget *w)
{
    save_config();
    exit(0);
}

void MainWindow::add_file_cb(Fl_Widget *w)
{
    Fl_File_Chooser *fc = new Fl_File_Chooser(NULL,
                                              NULL,
                                              Fl_File_Chooser::MULTI,
                                              "Choose files to hash");
    fc->show();
    while (fc->shown())
       Fl::wait();
    if (fc->value())
    {
       OPT_STRUCT st;
       st.htmllink = cf_htmllink;
       st.htmlfull = cf_htmlfull;
       st.recursive = cf_recursive;

       for (int k = 1; k <= fc->count(); ++k)
       {
          _queue->addJob(fc->value(k), &st);
       }
    }
    delete fc;
}

void MainWindow::add_dir_cb(Fl_Widget *w)
{
    Fl_File_Chooser *fc = new Fl_File_Chooser(NULL,
                                              NULL,
                                              Fl_File_Chooser::DIRECTORY,
                                              "Choose files to hash");
    fc->show();
    while (fc->shown())
       Fl::wait();
    if (fc->value())
    {
       OPT_STRUCT st;
       st.htmllink = cf_htmllink;
       st.htmlfull = cf_htmlfull;
       st.recursive = cf_recursive;

       _queue->addJob(fc->value(), &st);
    }
    delete fc;
}

void MainWindow::edit_pref_cb(Fl_Widget *w)
{
    Config *cfg = new Config();
    while (cfg->shown())
       Fl::wait();
    delete cfg;
}

void MainWindow::ui_print(const char *msg)
{
    Fl::lock();
    _list->add(msg);
    Fl::unlock();
}

void MainWindow::ui_printerr(const char *msg)
{
    /* FIXME this causes a deadlock
       Fl::lock();
       fl_alert("%s", msg);
       Fl::unlock();
    */
}

void MainWindow::copy_cb(Fl_Widget *w)
{
    char *data = NULL;
    int l = 0, lb = 0;

    Fl::lock();

    for (int k = 1; k <= _list->size(); ++k)
    {
       if (_list->selected(k))
       {
          const char *bf = _list->text(k);
          int lg = strlen(bf);
          while (lg + l > lb * 512)
          {
             lb++;
             data = (char*)realloc(data, lb * 512);
          }
          memcpy(data + l, bf, lg);
          l += lg;
       }
    }

    if (l + 1 > lb * 512)
    {
       data = (char*)realloc(data, l+1);
    }
    data[l] = 0;

    Fl::unlock();

    Fl::copy(data, l+1, 0); // FIXME 1 sous Windows ?
    free(data);
}

void MainWindow::select_none_cb(Fl_Widget *w)
{
    Fl::lock();
    _list->deselect();
    Fl::unlock();
}

void MainWindow::select_all_cb(Fl_Widget *w)
{
    Fl::lock();
    for (int k = 1; k <= _list->size(); ++k)
    {
       _list->select(k);
    }
    Fl::unlock();
}

void MainWindow::select_invert_cb(Fl_Widget *w)
{
    Fl::lock();
    for (int k = 1; k <= _list->size(); ++k)
    {
       _list->select(k, !_list->selected(k));
    }
    Fl::unlock();
}

void MainWindow::select_delete_cb(Fl_Widget *w)
{
    Fl::lock();
    int k = 1;
    while (k <= _list->size())
    {
       if (_list->selected(k))
       {
          _list->remove(k);
       }
       else
       {
          ++k;
       }
    }
    Fl::unlock();
}

void MainWindow::cancel_cb(Fl_Widget *w)
{
    _queue->cancel();
    setProgress("Cancelled", 0);
}

void MainWindow::pause_cb(Fl_Widget *w)
{
    if (!_queue->isPaused())
    {
       Fl::lock();
       _queue->pause();
       _old_label = _label;
       _label = 0;
       _prog->label("Paused");
       Fl::unlock();
    }
}

void MainWindow::resume_cb(Fl_Widget *w)
{
    if (_queue->isPaused())
    {
       Fl::lock();
       _queue->resume();
       if (_old_label)
       {
          if (_label)
             free(_label);

          _label = _old_label;
          _old_label = 0;
          _prog->label(_label);
       }
       Fl::unlock();
    }
}

void MainWindow::about_cb(Fl_Widget *w)
{
    About *a = new About();
    while (a->shown())
       Fl::wait();
    delete a;
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

void MainWindow::_edit_pref_cb(Fl_Widget *w, void *p)
{
    ((MainWindow*)p)->edit_pref_cb(w);
}

void MainWindow::_copy_cb(Fl_Widget *w, void *p)
{
    ((MainWindow*)p)->copy_cb(w);
}

void MainWindow::_select_none_cb(Fl_Widget *w, void *p)
{
    ((MainWindow*)p)->select_none_cb(w);
}

void MainWindow::_select_all_cb(Fl_Widget *w, void *p)
{
    ((MainWindow*)p)->select_all_cb(w);
}

void MainWindow::_select_invert_cb(Fl_Widget *w, void *p)
{
    ((MainWindow*)p)->select_invert_cb(w);
}

void MainWindow::_select_delete_cb(Fl_Widget *w, void *p)
{
    ((MainWindow*)p)->select_delete_cb(w);
}

void MainWindow::_cancel_cb(Fl_Widget *w, void *p)
{
    ((MainWindow*)p)->cancel_cb(w);
}


void MainWindow::_pause_cb(Fl_Widget *w, void *p)
{
    ((MainWindow*)p)->pause_cb(w);
}


void MainWindow::_resume_cb(Fl_Widget *w, void *p)
{
    ((MainWindow*)p)->resume_cb(w);
}

void MainWindow::_about_cb(Fl_Widget *w, void *p)
{
    ((MainWindow*)p)->about_cb(w);
}

//==============================================================================

// This is win32-specific.

#ifdef __WIN32__

extern MainWindow *AppWindow; /* in main.cpp */

int handle_unknown_message(int)
{
    if (fl_msg.message == WM_COPYDATA)
    {
        COPYDATASTRUCT *d = (COPYDATASTRUCT*)fl_msg.lParam;
        MSG_DATA_STRUCT *data = (MSG_DATA_STRUCT*)d->lpData;
        data-opts.htmlfull = cf_htmlfull;
        AppWindow->addJob(&data->str[0],
                          &data->opts);
        return 1;
    }

    return 0;
}

#endif

