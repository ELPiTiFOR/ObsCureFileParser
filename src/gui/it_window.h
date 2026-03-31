#ifndef IT_WINDOW_H
#define IT_WINDOW_H

#include <stddef.h>
#include <windows.h>

#include "it_mod_list.h"

#define SELECTED_IT_FILE_TEXT_ID 1
#define LOAD_IT_BUTTON_ID 2
#define SAVE_IT_BUTTON_ID 3
#define UP_IT_BUTTON_ID 4
#define DOWN_IT_BUTTON_ID 5
#define TOP_IT_BUTTON_ID 6
#define BOT_IT_BUTTON_ID 7

#define ITEM_ID_BUTTON_IDS_START 1000
#define ITEM_LOC_EDIT_IDS_START 2000
#define EXTRA_INFO_BUTTON_IDS_START 3000
#define MULTIPLIER_EDIT_IDS_START 4000
#define DELETE_ITEM_BUTTON_IDS_START 6000
#define ADD_ITEM_BUTTON_IDS_START 7000

extern char IT_WINDOW_CLASS_NAME[];
extern HWND itWindowHwnd;
//extern HINSTANCE itWindowHInstance;

extern it_mod_list *selected_item;

extern it_file *curr_it;

extern size_t current_it_list_offset;

void refresh_it_mod_list();

void OpenItWindow(HWND parentHwnd);
void create_it_window_elements(HWND hwnd, HINSTANCE hInstance);
LRESULT CALLBACK ItWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam,
    LPARAM lParam);

#endif /* !IT_WINDOW_H */