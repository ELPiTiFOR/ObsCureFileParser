#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <windows.h>

#define SELECTED_IT_FILE_TEXT_ID 1
#define LOAD_IT_BUTTON_ID 2
#define SAVE_IT_BUTTON_ID 3
#define UP_IT_BUTTON_ID 4
#define DOWN_IT_BUTTON_ID 5
#define TOP_IT_BUTTON_ID 6
#define BOT_IT_BUTTON_ID 7

#define ITEM_ID_BUTTON_IDS_START 1000

extern char MAIN_WINDOW_CLASS_NAME[];

extern HWND thisHwnd;
extern HINSTANCE thisHInstance;

void create_main_window_elements(HWND hwnd, HINSTANCE hInstance);
LRESULT CALLBACK MainWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam,
    LPARAM lParam);

#endif /* !MAIN_WINDOW_H */