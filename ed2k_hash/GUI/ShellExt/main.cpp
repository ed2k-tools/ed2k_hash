
#include <windows.h>
#include <shlobj.h>
#include <olectl.h>
#include <shlwapi.h>
#include <tchar.h>

#include <list>
#include <string>
#include <sstream>
#include <iosfwd>

#include <JobQueue.h>

using namespace std;

//==========================================================================

#ifdef DEBUG

#include <stdio.h>

#define TRACE(fmt, args...) \
{ \
  FILE *fp = fopen("C:\\SHTRACE.TXT", "a+"); \
  fprintf(fp, fmt "\n", ## args); \
  fclose(fp); \
} \

#define TRACEW(fmt, args...) \
{ \
  FILE *fp = fopen("C:\\SHTRACE.TXT", "a+"); \
  fwprintf(fp, fmt L"\n", ## args); \
  fclose(fp); \
} \

#else

#define TRACE(fmt, args...)
#define TRACEW(fmt, args...)

#endif

//==========================================================================

#define HASHWNDTEXT TEXT("Ed2k Hash GUI")

static BOOL CALLBACK FindHashEnumProc(HWND hWnd, HWND* pWndRet)
{
    DWORD dwResult;

    if (SendMessageTimeout(hWnd,
                           WM_GETTEXTLENGTH,
                           0,
                           0,
                           SMTO_ABORTIFHUNG,
                           2000,
                           &dwResult))
    {
       LPTSTR szName = (LPTSTR)malloc(sizeof(TCHAR) * (dwResult + 1));

       if (SendMessageTimeout(hWnd,
                              WM_GETTEXT,
                              (WPARAM)dwResult,
                              (LPARAM)szName,
                              SMTO_ABORTIFHUNG,
                              2000,
                              &dwResult))
       {
          if (!strncmp(szName, HASHWNDTEXT, lstrlen(HASHWNDTEXT)))
          {
             *pWndRet = hWnd;

             return FALSE;
          }
       }
    }

    return TRUE;
}

static HWND FindHashWindow(void)
{
    HWND hWnd = 0;

    EnumWindows((WNDENUMPROC)FindHashEnumProc, (LPARAM)&hWnd);

    return hWnd;
}

static void HandleFile(LPCTSTR szFilename, OPT_STRUCT *pOpt)
{
    HWND hWnd = FindHashWindow();

    TRACE("Handling file %s", szFilename);

    if (!hWnd)
    {
       HKEY hKey;

       TRACE("Window not found; launching");

       if (RegOpenKeyEx(HKEY_CURRENT_USER,
                        TEXT("Software\\Ed2k_Hash"),
                        0,
                        KEY_READ,
                        &hKey) == ERROR_SUCCESS)
       {
          TCHAR bf[MAX_PATH + 20];
          DWORD dwSize = MAX_PATH;

          if (RegQueryValueEx(hKey,
                              TEXT("Install_Dir"),
                              0,
                              NULL,
                              (LPBYTE)bf,
                              &dwSize) == ERROR_SUCCESS)
          {
#ifdef UNICODE
             wcscat(bf, TEXT("\\ed2k_hash_gui.exe"));
#else
             strcat(bf, "\\ed2k_hash_gui.exe");
#endif

             PROCESS_INFORMATION info;
             STARTUPINFO st;

             ZeroMemory(&st, sizeof(st));

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
                   hWnd = FindHashWindow();
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
       MSG_DATA_STRUCT *p = (MSG_DATA_STRUCT*)malloc(sizeof(MSG_DATA_STRUCT) + lstrlen(szFilename));

       TRACE("Window found; sending message");

       memcpy(&p->str[0], szFilename, lstrlen(szFilename) + 1);
       memcpy(&p->opts, pOpt, sizeof(p->opts));

       COPYDATASTRUCT st;

       ZeroMemory(&st, sizeof(st));

       st.lpData = (LPVOID)p;
       st.cbData = sizeof(MSG_DATA_STRUCT) + lstrlen(szFilename);
       SendMessage(hWnd, WM_COPYDATA, (WPARAM)NULL, (LPARAM)&st);
       free(p);
    }
}

//==========================================================================

DEFINE_GUID(ED2KHASHGUID, 0x2314e9ee, 0xc2f2, 0x4040, 0xa2, 0x63, 0x28, 0x31, 0xe0, 0x68, 0xa8, 0x6a);

#define STRINGGUID "{2314E9EE-C2F2-4040-A263-2831E068A86A}"

static DWORD dwObjectCount = 0;

class Ed2kHashShell : virtual public IShellExtInit, virtual public IContextMenu
{
public:
    Ed2kHashShell();
    virtual ~Ed2kHashShell();

    // IUnknown

    STDMETHODIMP QueryInterface(REFIID, LPVOID*);
    STDMETHODIMP_(DWORD) AddRef();
    STDMETHODIMP_(DWORD) Release();

    // IShellExtInit
    STDMETHODIMP Initialize(LPCITEMIDLIST, IDataObject*, HKEY);

    // IContextMenu

    STDMETHODIMP QueryContextMenu(HMENU, UINT, UINT, UINT, UINT);
    STDMETHODIMP GetCommandString(UINT_PTR, UINT, UINT*, LPSTR, UINT);
    STDMETHODIMP InvokeCommand(LPCMINVOKECOMMANDINFO);

private:
    DWORD m_dwCount;
    list<string> m_filenames;
    list<string> m_directories;
};

class Ed2kHashShellClassFactory : virtual public IClassFactory
{
  public:
    Ed2kHashShellClassFactory();
    virtual ~Ed2kHashShellClassFactory();

    // IUnknown

    STDMETHODIMP QueryInterface(REFIID, LPVOID*);
    STDMETHODIMP_(DWORD) AddRef();
    STDMETHODIMP_(DWORD) Release();

    // IClassFactory

    STDMETHODIMP CreateInstance(IUnknown*, REFIID, LPVOID*);
    STDMETHODIMP LockServer(BOOL);

  private:
    DWORD m_dwCount;
};

//==========================================================================

#define IDM_MENU               0
#define IDM_HASH               1
#define IDM_HTMLHASH           2
#define IDM_FULLHTMLHASH       3
#define IDM_RECHASH            4
#define IDM_RECHTMLHASH        5
#define IDM_FULLRECHTMLHASH    6
#define IDM_LAST               7

//==========================================================================

Ed2kHashShell::Ed2kHashShell()
    : m_dwCount(1)
{
    ++dwObjectCount;
    TRACE("Ed2kHashShell::Ed2kHashShell(); count=%d", (int)dwObjectCount);
}

Ed2kHashShell::~Ed2kHashShell()
{
    --dwObjectCount;
    TRACE("Ed2kHashShell::~Ed2kHashShell(); count=%d", (int)dwObjectCount);
}

STDMETHODIMP Ed2kHashShell::QueryInterface(REFIID iid, LPVOID *lpRet)
{
    *lpRet = NULL;

    if (IsEqualIID(iid, IID_IUnknown))
    {
        *lpRet = this;
        TRACE("Ed2kHashShell::QueryInterface(IUnknown)");
    }
    else if (IsEqualIID(iid, IID_IShellExtInit))
    {
        *lpRet = (IShellExtInit*)this;
        TRACE("Ed2kHashShell::QueryInterface(IShellExtInit)");
    }
    else if (IsEqualIID(iid, IID_IContextMenu))
    {
        *lpRet = (IContextMenu*)this;
        TRACE("Ed2kHashShell::QueryInterface(IContextMenu)");
    }
#ifdef DEBUG
    else
    {
        LPOLESTR szName;

        if (SUCCEEDED(StringFromIID(iid, &szName)))
        {
            TRACEW(L"Ed2kHashShell::QueryInterface(%s)", szName);

            CoTaskMemFree(szName);
        }
        else
        {
            TRACE("Could not get IID.");
        }
    }
#endif

    if (*lpRet)
    {
        ((IUnknown*)(*lpRet))->AddRef();
        return S_OK;
    }

    return E_NOTIMPL;
}

STDMETHODIMP_(DWORD) Ed2kHashShell::AddRef()
{
    TRACE("Ed2kHashShell::AddRef(): %d", (int)m_dwCount);

    return ++m_dwCount;
}

STDMETHODIMP_(DWORD) Ed2kHashShell::Release()
{
    TRACE("Ed2kHashShell::Release(): %d", (int)m_dwCount);

    if (--m_dwCount == 0)
    {
        TRACE("Ed2kHashShell::Release(): deleting.");
        delete this;
        return 0;
    }

    return m_dwCount;
}

STDMETHODIMP Ed2kHashShell::QueryContextMenu(HMENU hMenu,
                                         UINT indexMenu,
                                         UINT idCmdFirst,
                                         UINT idCmdLast,
                                         UINT uFlags)
{
    TRACE("Ed2kHashShell::QueryContextMenu");

    if (!(uFlags & CMF_DEFAULTONLY))
    {
        HMENU hMyMenu;
        UINT uMenuFlags;
        MENUITEMINFO info;

        hMyMenu = CreateMenu();

        InsertMenu(hMyMenu,
                   0,
                   MF_STRING|MF_BYPOSITION,
                   idCmdFirst + IDM_HASH,
                   TEXT("Hash"));

        InsertMenu(hMyMenu,
                   1,
                   MF_STRING|MF_BYPOSITION,
                   idCmdFirst + IDM_HTMLHASH,
                   TEXT("HTML Hash"));

        InsertMenu(hMyMenu,
                   2,
                   MF_STRING|MF_BYPOSITION,
                   idCmdFirst + IDM_FULLHTMLHASH,
                   TEXT("Full HTML Hash"));

        InsertMenu(hMyMenu,
                   3,
                   MF_STRING|MF_BYPOSITION,
                   idCmdFirst + IDM_RECHASH,
                   TEXT("Recursive Hash"));

        InsertMenu(hMyMenu,
                   4,
                   MF_STRING|MF_BYPOSITION,
                   idCmdFirst + IDM_RECHTMLHASH,
                   TEXT("Recursive HTML Hash"));

        InsertMenu(hMyMenu,
                   5,
                   MF_STRING|MF_BYPOSITION,
                   idCmdFirst + IDM_FULLRECHTMLHASH,
                   TEXT("Full recursive HTML Hash"));

        if (m_filenames.size() && !m_directories.size())
           uMenuFlags = MF_ENABLED;
        else
           uMenuFlags = MF_DISABLED|MF_GRAYED;

        EnableMenuItem(hMyMenu, idCmdFirst + IDM_HASH, uMenuFlags);
        EnableMenuItem(hMyMenu, idCmdFirst + IDM_HTMLHASH, uMenuFlags);
        EnableMenuItem(hMyMenu, idCmdFirst + IDM_FULLHTMLHASH, uMenuFlags);

        if (m_directories.size())
           uMenuFlags = MF_ENABLED;
        else
           uMenuFlags = MF_DISABLED|MF_GRAYED;

        EnableMenuItem(hMyMenu, idCmdFirst + IDM_RECHASH, uMenuFlags);
        EnableMenuItem(hMyMenu, idCmdFirst + IDM_RECHTMLHASH, uMenuFlags);
        EnableMenuItem(hMyMenu, idCmdFirst + IDM_FULLRECHTMLHASH, uMenuFlags);

        ZeroMemory(&info, sizeof(info));

        info.cbSize = sizeof(info);
        info.fMask = MIIM_SUBMENU|MIIM_ID|MIIM_STRING;
        info.fType = MFT_STRING;
        info.fState = MFS_ENABLED;
        info.wID = idCmdFirst + IDM_MENU;
        info.hSubMenu = hMyMenu;
        info.cch = lstrlen(TEXT("Ed2k Hash"));
        info.dwTypeData = TEXT("Ed2k Hash");

        if (!InsertMenuItem(hMenu, indexMenu, TRUE, &info))
        {
            TRACE("Could not insert menu: 0x%08x", (int)GetLastError());
        }

        return MAKE_HRESULT(SEVERITY_SUCCESS, 0, USHORT(IDM_LAST));
    }

    return MAKE_HRESULT(SEVERITY_SUCCESS, 0, USHORT(0));
}

#define DECLCMD(id, help, verb) \
  case id: \
    hr = S_OK; \
    switch (uFlags) { \
      case GCS_HELPTEXTA: \
        strcpy(pszName, help); \
        break; \
      case GCS_HELPTEXTW: \
        wcscpy((LPWSTR)pszName, L##help); \
        break; \
      case GCS_VERBA: \
        strcpy(pszName, verb); \
        break; \
      case GCS_VERBW: \
        wcscpy((LPWSTR)pszName, L##verb); \
        break; \
      default: \
        break; \
    } \
    break

STDMETHODIMP Ed2kHashShell::GetCommandString(UINT_PTR idCmd,
                                             UINT uFlags,
                                             UINT *pwReserved,
                                             LPSTR pszName,
                                             UINT cchMax)
{
    HRESULT hr = E_INVALIDARG;

    TRACE("Ed2kHashShell::GetCommandString");

    switch (idCmd)
    {
       DECLCMD(IDM_HASH, "Hash selected files", "hash");
       DECLCMD(IDM_HTMLHASH, "Hash selected file with HTML output", "htmlhash");
       DECLCMD(IDM_FULLHTMLHASH, "Full hash with HTML output", "fullhtmlhash");
       DECLCMD(IDM_RECHASH, "Hash selected files and directories recursively", "rechash");
       DECLCMD(IDM_RECHTMLHASH, "Hash selected files and directories recursively with HTML output", "rechtmlhash");
       DECLCMD(IDM_FULLRECHTMLHASH, "Full recursive hash with HTML output", "fullrechtmlhash");
    default:
        break;
    }

    return hr;
}

STDMETHODIMP Ed2kHashShell::InvokeCommand(LPCMINVOKECOMMANDINFO lpcmi)
{
    UINT uCommand = (UINT)-1;
    OPT_STRUCT opt;

    TRACE("Ed2kHashShell::InvokeCommand");

    if (HIWORD(lpcmi->lpVerb))
    {
        if (!strcmp(lpcmi->lpVerb, "hash"))
        {
            uCommand = IDM_HASH;
        }
        else if (!strcmp(lpcmi->lpVerb, "htmlhash"))
        {
            uCommand = IDM_HTMLHASH;
        }
        else if (!strcmp(lpcmi->lpVerb, "fullhtmlhash"))
        {
            uCommand = IDM_FULLHTMLHASH;
        }
        else if (!strcmp(lpcmi->lpVerb, "rechash"))
        {
            uCommand = IDM_RECHASH;
        }
        else if (!strcmp(lpcmi->lpVerb, "rechtmlhash"))
        {
            uCommand = IDM_RECHTMLHASH;
        }
        else if (!strcmp(lpcmi->lpVerb, "fullrechtmlhash"))
        {
            uCommand = IDM_FULLRECHTMLHASH;
        }
        else
        {
            return E_FAIL;
        }
    }
    else
    {
        uCommand = LOWORD(lpcmi->lpVerb);
    }

    ZeroMemory(&opt, sizeof(opt));

    switch (uCommand)
    {
       case IDM_HASH:
          break;

       case IDM_HTMLHASH:
          opt.htmllink = 1;
          break;

       case IDM_FULLHTMLHASH:
          opt.htmllink = 1;
          opt.htmlfull = 1;
          break;

       case IDM_RECHASH:
          opt.recursive = 1;
          break;

       case IDM_RECHTMLHASH:
          opt.htmllink = 1;
          opt.recursive = 1;
          break;

       case IDM_FULLRECHTMLHASH:
          opt.htmllink = 1;
          opt.htmlfull = 1;
          opt.recursive = 1;
          break;

    default:
        return E_FAIL;
    }

    for (list<string>::iterator k = m_filenames.begin();
         k != m_filenames.end();
         ++k)
    {
       HandleFile(k->c_str(), &opt);
    }

    for (list<string>::iterator k = m_directories.begin();
         k != m_directories.end();
         ++k)
    {
       HandleFile(k->c_str(), &opt);
    }

    return S_OK;
}

STDMETHODIMP Ed2kHashShell::Initialize(LPCITEMIDLIST pidlFolder,
                                   IDataObject *pdtobj,
                                   HKEY hkeyProgID)
{
    TRACE("Ed2kHashShell::Initialize; %p", pdtobj);

    if (pidlFolder)
    {
        TCHAR szPath[MAX_PATH];

        m_directories.clear();

        if (SHGetPathFromIDList(pidlFolder, szPath))
        {
            m_directories.push_back(string(szPath));
            TRACE("Directory: %s", szPath);
        }
        else
        {
            TRACE("Could not get PIDL path.");
        }
    }

    if (pdtobj)
    {
        m_filenames.clear();
        m_directories.clear();

        FORMATETC fmt;
        STGMEDIUM med;

        fmt.cfFormat = CF_HDROP;
        fmt.ptd = NULL;
        fmt.dwAspect = DVASPECT_CONTENT;
        fmt.lindex = -1;
        fmt.tymed = TYMED_HGLOBAL;

        ZeroMemory(&med, sizeof(med));

        if (SUCCEEDED(pdtobj->GetData(&fmt, &med)))
        {
            HDROP hDrop = (HDROP)GlobalLock(med.hGlobal);
            UINT nFiles;

            nFiles = DragQueryFile(hDrop, 0xFFFFFFFFU, NULL, 0);

            for (UINT k = 0; k < nFiles; ++k)
            {
                UINT sz = 0;
                LPTSTR name;

                sz = DragQueryFile(hDrop, k, NULL, 0);
                name = (LPTSTR)LocalAlloc(LPTR, sizeof(TCHAR) * (sz + 1));
                DragQueryFile(hDrop, k, name, sz + 1);
                TRACE("File: %s", name);
                m_filenames.push_back(string(name));

                HANDLE hFile;
                WIN32_FIND_DATA data;

                if ((hFile = FindFirstFile(name, &data)) != INVALID_HANDLE_VALUE)
                {
                    if (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
                    {
                        m_directories.push_back(string(name));
                    }

                    FindClose(hFile);
                }

                LocalFree(name);
            }

            GlobalUnlock((HGLOBAL)hDrop);

            if (med.pUnkForRelease)
                med.pUnkForRelease->Release();
            else
                GlobalFree(med.hGlobal);
        }
    }

    return S_OK;
}

//==========================================================================

Ed2kHashShellClassFactory::Ed2kHashShellClassFactory()
    : m_dwCount(1)
{
    TRACE("Ed2kHashShellClassFactory::Ed2kHashShellClassFactory()");
}

Ed2kHashShellClassFactory::~Ed2kHashShellClassFactory()
{
    TRACE("Ed2kHashShellClassFactory::~Ed2kHashShellClassFactory()");
}

STDMETHODIMP Ed2kHashShellClassFactory::QueryInterface(REFIID iid, LPVOID *ret)
{
    *ret = NULL;

    if (IsEqualIID(iid, IID_IUnknown))
    {
       *ret = this;
    }
    else if (IsEqualIID(iid, IID_IClassFactory))
    {
       *ret = (IClassFactory*)this;
    }

    if (*ret)
    {
       ((IUnknown*)(*ret))->AddRef();
       return S_OK;
    }

    return E_NOTIMPL;
}

STDMETHODIMP_(DWORD) Ed2kHashShellClassFactory::AddRef()
{
    return ++m_dwCount;
}

STDMETHODIMP_(DWORD) Ed2kHashShellClassFactory::Release()
{
    if (--m_dwCount == 0)
    {
       delete this;

       return 0;
    }

    return m_dwCount;
}

STDMETHODIMP Ed2kHashShellClassFactory::CreateInstance(IUnknown *outer, REFIID iid, void **ret)
{
    HRESULT res;

    TRACE("Ed2kHashShellClassFactory::CreateInstance");

    Ed2kHashShell *instance = new Ed2kHashShell();

    res = instance->QueryInterface(iid, ret);
    instance->Release();

    return res;
}

STDMETHODIMP Ed2kHashShellClassFactory::LockServer(BOOL lock)
{
    if (lock)
       AddRef();
    else
       Release();

    return S_OK;
}

//==========================================================================

extern HRESULT __declspec(dllexport) __stdcall DllRegisterServer()
{
    HKEY key;
    TCHAR path[MAX_PATH];
    DWORD len;

    len = GetModuleFileName(GetModuleHandle(TEXT("ED2KSHL.DLL")), path, MAX_PATH);

    // COM server

    if (RegCreateKeyEx(HKEY_CLASSES_ROOT,
                       TEXT("CLSID\\" STRINGGUID "\\InprocServer32"),
                       0,
                       NULL,
                       0,
                       KEY_ALL_ACCESS,
                       NULL,
                       &key,
                       NULL) != ERROR_SUCCESS)
    {
       return SELFREG_E_CLASS;
    }

    RegSetValueEx(key,
                  NULL,
                  0,
                  REG_SZ,
                  (LPBYTE)path,
                  len * sizeof(TCHAR));

    RegSetValueEx(key,
                  TEXT("ThreadingModel"),
                  0,
                  REG_SZ,
                  (LPBYTE)TEXT("Apartment"),
                  9 * sizeof(TCHAR));

    RegCloseKey(key);

    RegCreateKeyEx(HKEY_CLASSES_ROOT,
                   TEXT("CLSID\\" STRINGGUID),
                   0,
                   NULL,
                   0,
                   KEY_ALL_ACCESS,
                   NULL,
                   &key,
                   NULL);

    RegSetValueEx(key,
                  NULL,
                  0,
                  REG_SZ,
                  (LPBYTE)TEXT("Ed2kHashShell"),
                  8 * sizeof(TCHAR));

    RegCloseKey(key);

    // Shell extension

    if (RegCreateKeyEx(HKEY_CLASSES_ROOT,
                       TEXT("AllFileSystemObjects\\shellex\\ContextMenuHandlers\\Ed2kHashShell"),
                       0,
                       NULL,
                       0,
                       KEY_ALL_ACCESS,
                       NULL,
                       &key,
                       NULL) != ERROR_SUCCESS)
    {
        return SELFREG_E_CLASS;
    }

    RegSetValueExA(key,
                   NULL,
                   0,
                   REG_SZ,
                   (LPBYTE)STRINGGUID,
                   strlen(STRINGGUID));

    if (RegCreateKeyEx(HKEY_CLASSES_ROOT,
                       TEXT("Directory\\Background\\shellex\\ContextMenuHandlers\\Ed2kHashShell"),
                       0,
                       NULL,
                       0,
                       KEY_ALL_ACCESS,
                       NULL,
                       &key,
                       NULL) != ERROR_SUCCESS)
    {
        return SELFREG_E_CLASS;
    }

    RegSetValueExA(key,
                   NULL,
                   0,
                   REG_SZ,
                   (LPBYTE)STRINGGUID,
                   strlen(STRINGGUID));

    RegCloseKey(key);

#ifdef DEBUG
    if (RegCreateKeyEx(HKEY_CURRENT_USER,
                       TEXT("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer"),
                       0,
                       NULL,
                       0,
                       KEY_ALL_ACCESS,
                       NULL,
                       &key,
                       NULL) == ERROR_SUCCESS)
    {
        DWORD value = 1;

        RegSetValueEx(key, TEXT("DesktopProcess"), 0, REG_DWORD, (PBYTE)&value, sizeof(DWORD));
        RegCloseKey(key);
    }
#endif

    return S_OK;
}

extern HRESULT __declspec(dllexport) __stdcall DllUnregisterServer(void)
{
    SHDeleteKey(HKEY_CLASSES_ROOT,
                TEXT("AllFileSystemObjects\\shellex\\ContextMenuHandlers\\Ed2kHashShell"));
    SHDeleteKey(HKEY_CLASSES_ROOT,
                TEXT("Directory\\Background\\shellex\\ContextMenuHandlers\\Ed2kHashShell"));

#ifdef DEBUG
    HKEY key;

    if (RegCreateKeyEx(HKEY_CURRENT_USER,
                       TEXT("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer"),
                       0,
                       NULL,
                       0,
                       KEY_ALL_ACCESS,
                       NULL,
                       &key,
                       NULL) == ERROR_SUCCESS)
    {
        DWORD value = 0;

        RegSetValueEx(key, TEXT("DesktopProcess"), 0, REG_DWORD, (PBYTE)&value, sizeof(DWORD));
        RegCloseKey(key);
    }
#endif

    return S_OK;
}

extern HRESULT __declspec(dllexport) __stdcall DllGetClassObject(REFCLSID clsid, REFIID iid, LPVOID *ret)
{
    *ret = new Ed2kHashShellClassFactory();

    return S_OK;
}

extern HRESULT __declspec(dllexport) __stdcall DllCanUnloadNow(void)
{
    return dwObjectCount ? S_FALSE : S_OK;
}
