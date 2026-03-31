#include <objbase.h>
#include <shlobj.h>
#include <shobjidl.h> 
#include <windows.h>

#include "add_item_window.h"
#include "item_id_window.h"
#include "it_window.h"
#include "choose_format_window.h"
#include "document_id_window.h"
#include "sav_window.h"

HINSTANCE thisHInstance;

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

    // registering it window class
    WNDCLASS wc = {0};
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = ItWindowProc;
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

    // registering choose format window class
    WNDCLASS choose_format_wc = {0};
    choose_format_wc.style = CS_HREDRAW | CS_VREDRAW;
    choose_format_wc.lpfnWndProc = ChooseFormatWindowProc;
    choose_format_wc.hInstance = hInstance;
    choose_format_wc.lpszClassName = CHOOSE_FORMAT_WINDOW_CLASS_NAME;
    choose_format_wc.hbrBackground = CreateSolidBrush(RGB(255, 255, 255));
    choose_format_wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    if (hIcon != NULL) {
        choose_format_wc.hIcon = hIcon;
        //choose_format_wc.hIconSm = hIcon;
    }
    RegisterClass(&choose_format_wc);

    // registering choose format window class
    WNDCLASS sav_wc = {0};
    sav_wc.style = CS_HREDRAW | CS_VREDRAW;
    sav_wc.lpfnWndProc = SavWindowProc;
    sav_wc.hInstance = hInstance;
    sav_wc.lpszClassName = SAV_WINDOW_CLASS_NAME;
    sav_wc.hbrBackground = CreateSolidBrush(RGB(255, 255, 255));
    sav_wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    if (hIcon != NULL) {
        sav_wc.hIcon = hIcon;
        //sav_wc.hIconSm = hIcon;
    }
    RegisterClass(&sav_wc);

    // registering add item window class
    WNDCLASS add_item_wc = {0};
    add_item_wc.style = CS_HREDRAW | CS_VREDRAW;
    add_item_wc.lpfnWndProc = AddItemWindowProc;
    add_item_wc.hInstance = hInstance;
    add_item_wc.lpszClassName = ADD_ITEM_WINDOW_CLASS_NAME;
    add_item_wc.hbrBackground = CreateSolidBrush(RGB(255, 255, 255));
    add_item_wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    if (hIcon != NULL) {
        add_item_wc.hIcon = hIcon;
        //add_item_wc.hIconSm = hIcon;
    }
    RegisterClass(&add_item_wc);

    //itWindowHInstance = hInstance;
    //chooseFormatHInstance = hInstance;
    thisHInstance = hInstance;

    //OpenItWindow((HWND)0);
    OpenChooseFormatWindow((HWND)0);

    MSG msg = {0};
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    
    return 0;
}