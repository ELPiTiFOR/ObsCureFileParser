#include <objbase.h>
#include <shlobj.h>
#include <shobjidl.h> 
#include <windows.h>

#include "item_id_window.h"
#include "it_window.h"
#include "document_id_window.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    // loading Icon
    HICON hIcon = LoadImage(
        NULL,
        "icon_ocfp.ico",
        IMAGE_ICON,
        0, 0,
        LR_LOADFROMFILE | LR_DEFAULTSIZE
    );

    // registering main window class
    WNDCLASS wc = {0};
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = MainWindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = IT_WINDOW_CLASS_NAME;
    wc.hbrBackground = CreateSolidBrush(RGB(255, 255, 255));
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    if (hIcon != NULL) {
        wc.hIcon = hIcon;
        //wc.hIconSm = hIcon;
    }
    RegisterClass(&wc);

    // registering item id window class
    WNDCLASS item_id_wc = {0};
    item_id_wc.style = CS_HREDRAW | CS_VREDRAW;
    item_id_wc.lpfnWndProc = ItemIdWindowProc;
    item_id_wc.hInstance = hInstance;
    item_id_wc.lpszClassName = ITEM_ID_WINDOW_CLASS_NAME;
    item_id_wc.hbrBackground = CreateSolidBrush(RGB(255, 255, 255));
    item_id_wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    if (hIcon != NULL) {
        item_id_wc.hIcon = hIcon;
        //item_id_wc.hIconSm = hIcon;
    }
    RegisterClass(&item_id_wc);

    // registering document id window class
    WNDCLASS document_id_wc = {0};
    document_id_wc.style = CS_HREDRAW | CS_VREDRAW;
    document_id_wc.lpfnWndProc = DocumentIdWindowProc;
    document_id_wc.hInstance = hInstance;
    document_id_wc.lpszClassName = DOCUMENT_ID_WINDOW_CLASS_NAME;
    document_id_wc.hbrBackground = CreateSolidBrush(RGB(255, 255, 255));
    document_id_wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    if (hIcon != NULL) {
        document_id_wc.hIcon = hIcon;
        //document_id_wc.hIconSm = hIcon;
    }
    RegisterClass(&document_id_wc);


    // the window
    HWND hwnd = CreateWindowEx(
        0,
        //WS_EX_TOPMOST,
        IT_WINDOW_CLASS_NAME,
        "ObsCure File Parser",
        //WS_OVERLAPPEDWINDOW,
        0 | WS_OVERLAPPED | WS_SYSMENU | WS_MINIMIZEBOX, // | WS_VSCROLL,
        //WS_POPUP,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        /*
        800,
        600,
        */
        710,
        830,
        NULL,
        NULL,
        hInstance,
        NULL
    );

    itWindowHwnd = hwnd;
    itWindowHInstance = hInstance;

    // buttons, etc of the main window
    create_main_window_elements(hwnd, hInstance);

    ShowWindow(hwnd, nCmdShow);
    
    MSG msg = {0};
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    
    return 0;
}