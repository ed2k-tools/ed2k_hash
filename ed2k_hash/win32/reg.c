/***************************************************************************
                          reg.c  -  registry functions
                             -------------------
    begin                : Sun Jan 12 14:05:58  2003
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

#include "reg.h"

HKEY open_key(const char *name)
{
    HKEY ret = NULL;
    DWORD dwDisp;

    RegCreateKeyEx(HKEY_CURRENT_USER,
                   name,
                   0,
                   NULL,
                   0,
                   KEY_ALL_ACCESS,
                   NULL,
                   &ret,
                   &dwDisp);
    return ret;
}

void close_key(HKEY hKey)
{
    RegCloseKey(hKey);
}

void write_integer(HKEY hKey,
                   const char *name,
                   int value)
{
    DWORD v = (DWORD)value;

    RegSetValueEx(hKey,
                  name,
                  0,
                  REG_DWORD,
                  (BYTE*)&v,
                  sizeof(v));
}

int read_integer(HKEY hKey,
                 const char *name,
                 int def)
{
    DWORD dwType;
    if (RegQueryValueEx(hKey,
                        name,
                        0,
                        &dwType,
                        NULL,
                        NULL) != ERROR_SUCCESS)
    {
       return def;
    }

    if (dwType != REG_DWORD)
    {
       return def;
    }

    DWORD dwValue = (DWORD)def;
    DWORD dwSize = sizeof(dwValue);
    RegQueryValueEx(hKey,
                    name,
                    0,
                    NULL,
                    (BYTE*)&dwValue,
                    &dwSize);
    return (int)dwValue;
}
