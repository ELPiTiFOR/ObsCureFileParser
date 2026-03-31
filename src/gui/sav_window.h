#ifndef SAV_WINDOW_H
#define SAV_WINDOW_H

#include <windows.h>

//#include "sav_mod_list.h"

#define SELECTED_SAV_FILE_TEXT_ID 1
#define LOAD_SAV_BUTTON_ID 2
#define SAVE_SAV_BUTTON_ID 3
/*
#define UP_SAV_BUTTON_ID 4
#define DOWN_SAV_BUTTON_ID 5
#define TOP_SAV_BUTTON_ID 6
#define BOT_SAV_BUTTON_ID 7

#define SAVEM_ID_BUTTON_IDS_START 1000
#define SAVEM_LOC_EDSAV_IDS_START 2000
#define EXTRA_INFO_BUTTON_IDS_START 3000
#define MULTIPLIER_EDSAV_IDS_START 4000
#define DELETE_SAVEM_BUTTON_IDS_START 6000
*/

extern char SAV_WINDOW_CLASS_NAME[];
extern HWND savWindowHwnd;
//extern HINSTANCE savWindowHInstance;

//extern sav_mod_list *selected_item;

//void refresh_sav_mod_list();

void OpenSavWindow(HWND parentHwnd);
void create_sav_window_elements();
LRESULT CALLBACK SavWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam,
    LPARAM lParam);

#endif /* !SAV_WINDOW_H */