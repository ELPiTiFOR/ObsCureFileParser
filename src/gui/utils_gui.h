#ifndef UTILS_GUI_H
#define UTILS_GUI_H

#include <objbase.h>
#include <shlobj.h>
#include <shobjidl.h> 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

extern HINSTANCE thisHInstance;

int SelectFolder(HWND hwnd, char *path, int size);
int SelectFile(HWND hwnd, char *path, int size);

#endif /* !UTILS_GUI_H */