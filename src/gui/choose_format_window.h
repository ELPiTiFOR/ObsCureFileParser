#ifndef CHOOSE_FORMAT_WINDOW_H
#define CHOOSE_FORMAT_WINDOW_H

#include <windows.h>

#define IT_WINDOW_BUTTON_ID 10000
#define SAV_WINDOW_BUTTON_ID 20000

extern char CHOOSE_FORMAT_WINDOW_CLASS_NAME[];
extern HWND chooseFormatHwnd;
//extern HINSTANCE chooseFormatHInstance;

void OpenChooseFormatWindow(HWND parentHwnd);
void create_choose_format_window_elements();
LRESULT CALLBACK ChooseFormatWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam,
    LPARAM lParam);

#endif /* !CHOOSE_FORMAT_WINDOW_H */