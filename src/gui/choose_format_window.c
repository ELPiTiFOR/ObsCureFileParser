#include "choose_format_window.h"

#include "it_window.h"
#include "sav_window.h"
#include "utils_gui.h"

char CHOOSE_FORMAT_WINDOW_CLASS_NAME[] = "ChooseFormatWindowClass";
HWND chooseFormatHwnd;
//HINSTANCE chooseFormatHInstance;

void OpenChooseFormatWindow(HWND parentHwnd)
{
    //chooseFormatHInstance = (HINSTANCE)GetWindowLongPtr(parentHwnd, GWLP_HINSTANCE);
    
    chooseFormatHwnd = CreateWindowEx(
        0,
        CHOOSE_FORMAT_WINDOW_CLASS_NAME,
        "Choose Format | ObsCure File Parser",
        0 | WS_OVERLAPPED | WS_SYSMENU | WS_MINIMIZEBOX | WS_VISIBLE,
        CW_USEDEFAULT, CW_USEDEFAULT,
        340, 240,
        parentHwnd,
        NULL,
        thisHInstance,
        //chooseFormatHInstance,
        NULL
    );

    if (!chooseFormatHwnd)
    {
        return;
    }

    create_choose_format_window_elements();

    ShowWindow(chooseFormatHwnd, SW_SHOW);
    UpdateWindow(chooseFormatHwnd);
}

void create_choose_format_window_elements()
{
    /*
    HWND selectedItFileTextHwnd = CreateWindow("STATIC", "", 
        WS_VISIBLE | WS_CHILD, 30, 20, 120, 20, chooseFormatHwnd,
        (HMENU)0, chooseFormatHInstance, NULL);
    */
    
    CreateWindow("BUTTON", "IT", WS_TABSTOP | WS_VISIBLE | WS_CHILD |
        BS_DEFPUSHBUTTON, 70, 70, 60, 60, chooseFormatHwnd, (HMENU)IT_WINDOW_BUTTON_ID,
        thisHInstance, NULL);

    CreateWindow("BUTTON", "SAV", WS_TABSTOP | WS_VISIBLE | WS_CHILD |
        BS_DEFPUSHBUTTON, 140, 70, 60, 60, chooseFormatHwnd, (HMENU)SAV_WINDOW_BUTTON_ID,
        thisHInstance, NULL);
}

LRESULT CALLBACK ChooseFormatWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam,
    LPARAM lParam)
{
    WORD code = HIWORD(wParam);

    switch (uMsg)
    {
    case WM_CREATE:
        break;
    case WM_COMMAND:
        if (code != BN_CLICKED)
            break;
        
        if (LOWORD(wParam) == IT_WINDOW_BUTTON_ID)
        {
            OpenItWindow(chooseFormatHwnd);
        }
        else if (LOWORD(wParam) == SAV_WINDOW_BUTTON_ID)
        {
            OpenSavWindow(chooseFormatHwnd);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
        
    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}