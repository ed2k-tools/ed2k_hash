// FileProcess.cpp: implementation of the CFileProcess class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CtxMenu.h"
#include "FileProcess.h"
#include "SHUtils.h"
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#include <JobQueue.h>

// in this example, this does nothing. but, in a real app, 
// you could use this class to do the real file processing work.
class CFileProcess
{
public:
   int ProcessFile(ThreadInfo *pThreadInfo)
   {
      int res = 0;

      // get the name of the file we'll be working on.
      // we're not really doing anything with the file here, so
      // this call is really just an example
      CString csFile;
      csFile = GET_SAFE(pThreadInfo->csFile);

      // for this example, we'll just update the progress meter, slowly
/*
      for (int i =0; i < 10; i++)
      {
         // update the progress variable.
         // the main thread will check this and update the progress dialog
         SET_SAFE(pThreadInfo->iPercentDone, 10 * i);

         // the main thread will set bStop when the user presses the 'cancel' button.
         // we need to stop what we're doing and return
         if (GET_SAFE(pThreadInfo->bStop))
         {
            res = 1;
            break;
         }

         // do some real work...
         Sleep(250);
      }
*/

	  HWND hWnd = FindWindow("FLTK", "Ed2k Hash GUI V0.3");
	  if (!hWnd)
	  {
		  HKEY hKey;
		  if (RegOpenKeyEx(HKEY_CURRENT_USER,
			               "Software\\Ed2k_Hash",
						   0,
						   KEY_READ,
						   &hKey) == ERROR_SUCCESS)
		  {
			  char bf[MAX_PATH + 20];
			  DWORD dwSize = MAX_PATH;
			  if (RegQueryValueEx(hKey,
				                  "Install_Dir",
								  0,
								  NULL,
								  (LPBYTE)bf,
								  &dwSize) == ERROR_SUCCESS)
              {
				  strcat(bf, "\\ed2k_hash_gui.exe");
				  PROCESS_INFORMATION info;
				  STARTUPINFO st;
				  memset(&st, 0, sizeof(st));
				  st.cb = sizeof(st);
				  if (CreateProcess(bf,
					                NULL,
						    		NULL,
								    NULL,
								    FALSE,
								    0,
								    NULL,
								    NULL,
								    &st,
								    &info))
				  {
					  if (WaitForInputIdle(info.hProcess,
						                   30 * 1000) == 0)
					  {
						  hWnd = FindWindow("FLTK", "Ed2k Hash GUI V0.3");
					  }
					  CloseHandle(info.hProcess);
					  CloseHandle(info.hThread);
				  }
			  }
			  RegCloseKey(hKey);
		  }
	  }

	  if (hWnd)
	  {
		  const char *bf = (const char*)csFile;
		  MSG_DATA_STRUCT *p = (MSG_DATA_STRUCT*)malloc(sizeof(MSG_DATA_STRUCT) + strlen(bf));
		  memcpy(&p->str[0], bf, strlen(bf) + 1);
		  memcpy(&p->opts, &pThreadInfo->opts, sizeof(p->opts));
		  COPYDATASTRUCT st;
		  memset(&st, 0, sizeof(st));
		  st.lpData = (LPVOID)p;
		  st.cbData = sizeof(MSG_DATA_STRUCT) + strlen(bf);
		  SendMessage(hWnd, WM_COPYDATA, (WPARAM)NULL, (LPARAM)&st);
		  free(p);
	  }

      return res;
   }

   
};


// this is the worker thread function.

UINT FileProcessThreadFunc(LPVOID data)
{
	if (data==NULL) 
	{
		AfxEndThread(0);
		ASSERT(0);
	}

	ThreadInfo *pThreadInfo = (ThreadInfo *)data;

   // we're not done yet!
	SET_SAFE(pThreadInfo->bDone, FALSE);

   // no errors
   SET_SAFE(pThreadInfo->iErr, 0);

   // process that file
   CFileProcess fp;
   int res = fp.ProcessFile(pThreadInfo);

   // tell the caller about any errors
   SET_SAFE(pThreadInfo->iErr, res);

   // now we're done.
   // the main thread is watching this value. when it sees
   // we've set bDone = TRUE, it will stop watching us
	SET_SAFE(pThreadInfo->bDone, TRUE);
   
	return 0;
}

