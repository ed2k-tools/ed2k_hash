/***************************************************************************
                          config.c  -  description
                             -------------------
    begin                : Sun Jan 12 14:35:30  2003
    copyright            : (C) 2003 by Jérome Laheurte
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

#if defined(__WIN32)

#include "config.h"
#include "options.h"
#include "param.h"
#include "win32main.h"

LRESULT CALLBACK ConfigProc(HWND hDlg,
                            UINT uMsg,
                            WPARAM wParam,
                            LPARAM lParam)
{
    switch (uMsg)
    {
       case WM_INITDIALOG:
          SendDlgItemMessage(hDlg,
                             IDB_CF_RECURSE,
                             BM_SETCHECK,
                             (WPARAM)(option_recursive ? BST_CHECKED : BST_UNCHECKED),
                             0);

          SendDlgItemMessage(hDlg,
                             IDB_CF_HTML,
                             BM_SETCHECK,
                             (WPARAM)(option_htmllink ? BST_CHECKED : BST_UNCHECKED),
                             0);

          SendDlgItemMessage(hDlg,
                             IDB_CF_FULL,
                             BM_SETCHECK,
                             (WPARAM)(option_htmlfull ? BST_CHECKED : BST_UNCHECKED),
                             0);

          EnableWindow(GetDlgItem(hDlg, IDB_CF_FULL),
                       option_htmllink);
          return TRUE;

       case WM_COMMAND:
          switch (LOWORD(wParam))
          {
             case IDOK:
                option_recursive = (SendDlgItemMessage(hDlg, IDB_CF_RECURSE, BM_GETCHECK, 0, 0) == BST_CHECKED);
                option_htmllink = (SendDlgItemMessage(hDlg, IDB_CF_HTML, BM_GETCHECK, 0, 0) == BST_CHECKED);
                option_htmlfull = (SendDlgItemMessage(hDlg, IDB_CF_FULL, BM_GETCHECK, 0, 0) == BST_CHECKED);
                save_options_to_registry();

                EndDialog(hDlg, TRUE);
                return TRUE;

             case IDCANCEL:
                EndDialog(hDlg, FALSE);
                return TRUE;

             case IDB_CF_HTML:
                EnableWindow(GetDlgItem(hDlg, IDB_CF_FULL),
                             (SendDlgItemMessage(hDlg,
                                                 IDB_CF_HTML,
                                                 BM_GETCHECK,
                                                 0,
                                                 0) == BST_CHECKED));
                break;
          }
          break;
    }

    return FALSE;
}

#endif

