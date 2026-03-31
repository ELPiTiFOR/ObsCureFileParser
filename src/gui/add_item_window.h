#ifndef ADD_ITEM_WINDOW_H
#define ADD_ITEM_WINDOW_H

#include <windows.h>

#include "it_mod_list.h"

#define ADD_ITEM_CONFIRMATION_UP_BUTTON_ID 4342
#define ADD_ITEM_CONFIRMATION_DOWN_BUTTON_ID 4343

extern char ADD_ITEM_WINDOW_CLASS_NAME[];
extern HWND addItemWindowHwnd;

extern it_mod_list *item_to_add;

void OpenAddItemWindow(HWND parentHwnd, int index);
void create_add_item_window_elements();
void refresh_item_to_add();
LRESULT CALLBACK AddItemWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam,
    LPARAM lParam);

#endif /* !ADD_ITEM_WINDOW_H */