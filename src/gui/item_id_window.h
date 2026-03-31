#ifndef ITEM_ID_WINDOW_H
#define ITEM_ID_WINDOW_H

#include <windows.h>

#include "it_mod_list.h"

#define ALL_ITEMS_IDS_START 10000

extern char ITEM_ID_WINDOW_CLASS_NAME[];
extern HWND itemIdHwnd;
//extern HINSTANCE itemIdHInstance;

void OpenItemIdWindow(HWND parentHwnd, it_mod_list **given_item_arg);
LRESULT CALLBACK ItemIdWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam,
    LPARAM lParam);

#endif /* !ITEM_ID_WINDOW_H */