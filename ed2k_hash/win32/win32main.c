/***************************************************************************
                          win32main.c  -  ugly entry point
                             -------------------
    begin                : Sun Jan 12 12:38:09  2003
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

#include <windows.h>
#include <commctrl.h>
#include <shlobj.h>

#include "win32main.h"
#include "options.h"
#include "param.h"
#include "config.h"
#include "uifuncs.h"

/* Globals */

HINSTANCE hInst;
HWND hMainWindow;
HWND hStatusBar;
HWND hFileList;

/* Locals */

static LPCTSTR lpszAppName = "ed2k_hash";
static LPCTSTR lpszTitle = "ed2k_hash";

static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
static LRESULT CALLBACK About  (HWND, UINT, WPARAM, LPARAM);

static HANDLE hMutex;

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hOld,
                     LPSTR szCmd,
                     int nCmdShow)
{
    MSG msg;
    WNDCLASSEX wc;

    InitCommonControls(); /* obsolete but simple... */

    hInst = hInstance;

    memset(&wc, 0, sizeof(wc));

    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = (WNDPROC)WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInst;
    /* wc.hIcon = LoadIcon(hInst, lpszAppName); */
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_BACKGROUND);
    wc.lpszMenuName = lpszAppName;
    wc.lpszClassName = lpszAppName;
    wc.cbSize = sizeof(WNDCLASSEX);
    /*
    wc.hIconSm = (HICON)LoadImage(hInst,
                                  lpszAppName,
                                  IMAGE_ICON,
                                  16,
                                  16,
                                  LR_DEFAULTCOLOR);
    */

    if (!RegisterClassEx(&wc))
       return FALSE;

    /*************************************/
    /* Test for another instance running */
    /*************************************/

    hMutex = CreateMutex(NULL, FALSE, "/Software/ED2K_HASH");
    if (WaitForSingleObject(hMutex, 0) == WAIT_TIMEOUT)
    {
       HWND hWnd = FindWindow(lpszAppName, lpszTitle);

       if (hWnd)
       {
          if (szCmd && (szCmd[0]))
          {
             /* We'll ask the other instance to do the job */

             COPYDATASTRUCT st;
             st.dwData = 0;
             st.cbData = strlen(szCmd) + 1;
             st.lpData = (PVOID)szCmd;
             SendMessage(hWnd,
                         WM_COPYDATA,
                         (WPARAM)hWnd,
                         (LPARAM)&st);
          }

          SetForegroundWindow(hWnd);
       }

       return 0;
    }

    /*************************************/

    hMainWindow = CreateWindow(lpszAppName,
                               lpszTitle,
                               WS_OVERLAPPEDWINDOW,
                               CW_USEDEFAULT, 0,
                               CW_USEDEFAULT, 0,
                               NULL,
                               NULL,
                               hInst,
                               NULL);

    if (!hMainWindow)
       return FALSE;

    load_options_from_registry();

    CheckMenuItem(GetMenu(hMainWindow),
                  IDM_RECURSE,
                  option_recursive ? MF_CHECKED : MF_UNCHECKED);

    EnableMenuItem(GetMenu(hMainWindow),
                   IDM_CANCEL,
                   MF_GRAYED);

    hStatusBar = CreateStatusWindow(WS_VISIBLE|WS_CHILD,
                                    PACKAGE " ready...",
                                    hMainWindow,
                                    -1);

    /* Initialisation of the listview */

    /**********************************/

    hFileList = CreateWindow(WC_LISTVIEW,
                             "",
                             WS_CHILD | LVS_REPORT | WS_BORDER | WS_VISIBLE,
                             3, 3,
                             30, 30,
                             hMainWindow,
                             NULL,
                             hInst,
                             NULL);

    LVCOLUMN col;
    memset(&col, 0, sizeof(col));
    col.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;

    col.pszText = "Link";
    col.cx = 300;
    ListView_InsertColumn(hFileList,
                          0,
                          &col);

    col.pszText = "Hash";
    col.cx = 100;
    col.iSubItem = 1;
    ListView_InsertColumn(hFileList,
                          1,
                          &col);

    col.pszText = "File";
    col.cx = 300;
    col.iSubItem = 2;
    ListView_InsertColumn(hFileList,
                          2,
                          &col);

    /**********************************/

    if (szCmd && (szCmd[0]))
    {
       add_file(szCmd);
    }

    /**********************************/

    ShowWindow(hMainWindow, nCmdShow);
    UpdateWindow(hMainWindow);

    while (GetMessage(&msg, NULL, 0, 0))
    {
       TranslateMessage(&msg);
       DispatchMessage(&msg);
    }

    save_options_to_registry();

    ReleaseMutex(hMutex);

    return (msg.wParam);
}

/**************************************************/
/* Main callback proc, too big as always          */
/**************************************************/

LRESULT CALLBACK WndProc(HWND hWnd,
                         UINT uMsg,
                         WPARAM wParam,
                         LPARAM lParam)
{
    switch (uMsg)
    {
       case WM_COPYDATA:
       {
          /* Another instance gives us a file/directory to scan */

          add_file((char*)(((PCOPYDATASTRUCT)lParam)->lpData));
          EnableMenuItem(GetMenu(hMainWindow),
                         IDM_CANCEL,
                         MF_ENABLED);
          return TRUE;
       }

       case WM_COMMAND:
          switch (LOWORD(wParam))
          {
             case IDM_ABOUT:
                DialogBox(hInst,
                          "ED2K_HASH_ABOUTBOX",
                          hWnd,
                          (DLGPROC)About);
                break;

             case IDM_QUIT:
                DestroyWindow(hWnd);
                break;

             case IDM_RECURSE:
                option_recursive = !option_recursive;
                CheckMenuItem(GetMenu(hWnd),
                              IDM_RECURSE,
                              option_recursive ? MF_CHECKED : MF_UNCHECKED);
                break;

             case IDM_CONFIG:
                DialogBox(hInst,
                          "ED2K_HASH_CONFIG",
                          hWnd,
                          (DLGPROC)ConfigProc);

                CheckMenuItem(GetMenu(hWnd),
                              IDM_RECURSE,
                              option_recursive ? MF_CHECKED : MF_UNCHECKED);
                break;

             case IDM_CLEAR:
             {
                if (is_running())
                {
                   MessageBox(hWnd,
                              "A scan is in progress. You must cancel it first.",
                              PACKAGE,
                              MB_OK);
                }
                else
                {
                   /* Clear the listview */

                   int n = ListView_GetItemCount(hFileList);
                   int k;

                   LVITEM it;
                   memset(&it, 0, sizeof(it));
                   it.mask = LVIF_PARAM;

                   for (k = 0; k<n; ++k)
                   {
                      it.iItem = k;
                      if (ListView_GetItem(hFileList, &it))
                      {
                         if (it.lParam)
                         {
                            free((void*)it.lParam);
                         }
                      }
                   }

                   ListView_DeleteAllItems(hFileList);
                }
             }
             break;

             case IDM_COPY:
             {
                /* Copy results in the clipboard */
                /* TODO: CR/LF instead of LF ? */

                char *bf = NULL;
                int lbf = 0;
                int n = 0;
                int idx = -1;

                LVITEM it;
                memset(&it, 0, sizeof(it));
                it.mask = LVIF_PARAM;

                while ((idx = SendMessage(hFileList,
                                          LVM_GETNEXTITEM,
                                          (WPARAM)idx,
                                          MAKELPARAM(LVNI_ALL | LVNI_SELECTED, 0))) != -1)
                {
                   it.iItem = idx;
                   if (ListView_GetItem(hFileList, &it))
                   {
                      char *str = (char*)it.lParam;
                      int l = strlen(str);
                      while (n + l + 1 > lbf)
                      {
                         bf = realloc(bf, lbf + 1024);
                         lbf += 1024;
                      }

                      memcpy(bf + n, str, l);
                      bf[n+l] = '\n';
                      n += l + 1;
                   }
                }

                if (bf)
                {
                   if (OpenClipboard(hWnd))
                   {
                      HGLOBAL hGlob;

                      EmptyClipboard();

                      hGlob = GlobalAlloc(GMEM_DDESHARE, n+1);
                      if (hGlob)
                      {
                         char *szCopy = GlobalLock(hGlob);
                         memcpy(szCopy, bf, n);
                         szCopy[n] = 0;
                         GlobalUnlock(hGlob);

                         SetClipboardData(CF_TEXT, hGlob);
                      }

                      CloseClipboard();
                   }

                   free(bf);
                }
             }
             break;

             case IDM_ADDFILES:
             {
                /* Multiple files selection dialog */

                OPENFILENAME info;
                char *bf = (char*)malloc(4096);
                int ok = 1;

                memset(&info, 0, sizeof(info));
                info.lStructSize = sizeof(info);
                info.hwndOwner = hWnd;
                info.lpstrFile = bf;
                bf[0] = 0;
                info.nMaxFile = 4096; /* I hate this. Doesn't the kernel know how to
                                         allocate memory ? */
                info.lpstrTitle = "Choose files to add";
                info.Flags = OFN_ALLOWMULTISELECT | OFN_EXPLORER | OFN_FILEMUSTEXIST;
                while (!GetOpenFileName(&info))
                {
                   if (CommDlgExtendedError() == FNERR_BUFFERTOOSMALL)
                   {
                      bf = (char*)realloc(bf, *((unsigned int*)bf) + 1);
                      info.nMaxFile = *((unsigned int*)bf) + 1;
                      MessageBox(hWnd,
                                 "A Memory error occured (buffer too small), which means\n"
                                 "you choose too many files. the buffer has been resized\n"
                                 "so that you can try again.",
                                 PACKAGE,
                                 MB_OK);
                   }
                   else
                   {
                      ok = 0;
                      break;
                   }
                }

                if (ok)
                {
                   add_files(bf, info.nFileOffset);
                   EnableMenuItem(GetMenu(hMainWindow),
                                  IDM_CANCEL,
                                  MF_ENABLED);
                }

                free(bf);
             }
             break;

             case IDM_ADDDIR:
             {
                /* Of course, there's no unified way to select files *and*
                   directories... */

                BROWSEINFO info;
                char szDir[MAX_PATH]; /* beuarh */

                memset(&info, 0, sizeof(info));
                info.hwndOwner = hWnd;
                info.pidlRoot = NULL;
                info.pszDisplayName = szDir;
                info.lpszTitle = "Select directory";
                info.ulFlags = BIF_BROWSEINCLUDEFILES;
                LPITEMIDLIST l = SHBrowseForFolder(&info);
                if (l)
                {
                   SHGetPathFromIDList(l, info.pszDisplayName);

                   add_file(info.pszDisplayName);

                   EnableMenuItem(GetMenu(hMainWindow),
                                  IDM_CANCEL,
                                  MF_ENABLED);

                   /* memory leak here, but gcc complains that "structure
                      has no member named 'Free'" ! Maybe Mingw32 headers
                      aren't up-to-date */

                   /*
                     LPMALLOC m = NULL;
                     if (SHGetMalloc(&m) == NOERROR)
                     {
                     m->Free((void*)l);
                     }
                   */
                }
             }
             break;

             case IDM_CANCEL:
                cancel_task();
                break;
          }
          break;

       case WM_DESTROY:
          PostQuitMessage(0);
          break;

       case WM_USER_THREAD_TERMINATED:
          EnableMenuItem(GetMenu(hMainWindow),
                         IDM_CANCEL,
                         MF_GRAYED);
          cleanup_thread();
          break;

       case WM_SIZE:
       {
          RECT r;
          GetClientRect(hWnd, &r);
          MoveWindow(hFileList,
                     3, 3,
                     r.right - r.left - 6,
                     r.bottom - r.top - 6 - 23,
                     TRUE);

          PostMessage(hStatusBar, WM_SIZE, 0, 0);
       }
       break;

       default:
          return DefWindowProc(hWnd, uMsg, wParam, lParam);
    }

    return 0;
}

LRESULT CALLBACK About(HWND hDlg,
                       UINT uMsg,
                       WPARAM wParam,
                       LPARAM lParam)
{
    switch (uMsg)
    {
       case WM_INITDIALOG:
          return TRUE;

       case WM_COMMAND:
          if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
          {
             EndDialog(hDlg, TRUE);
             return TRUE;
          }
          break;
    }

    return FALSE;
}

#endif

