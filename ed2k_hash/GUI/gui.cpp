/***************************************************************************
                          main.cpp  -  entry point
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

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#ifdef HAVE_FL_WITH_LC_H
# include <FL/Fl.h>
#else
# include <FL/Fl.H>
#endif

#include "MainWindow.h"
#include "Configuration.h"

MainWindow *AppWindow;

int main (int argc, char *argv[])
{
    int ret;

    load_config();
    AppWindow = new MainWindow();
    Fl::lock();
    ret = Fl::run();
    save_config();
    return ret;
}
