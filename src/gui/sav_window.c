#include "sav_window.h"

#include "utils_gui.h"

char SAV_WINDOW_CLASS_NAME[] = "SavWindowClass";
HWND savWindowHwnd;
//extern HINSTANCE savWindowHInstance;

void OpenSavWindow(HWND parentHwnd)
{
    //itWindowHInstance = (HINSTANCE)GetWindowLongPtr(parentHwnd, GWLP_HINSTANCE);

    savWindowHwnd = CreateWindowEx(
        0,
        //WS_EX_TOPMOST,
        SAV_WINDOW_CLASS_NAME,
        "SAV | ObsCure File Parser",
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
        thisHInstance,//hInstance,
        NULL
    );

    if (!savWindowHwnd)
    {
        return;
    }

    create_sav_window_elements();
    ShowWindow(savWindowHwnd, SW_SHOW);
    UpdateWindow(savWindowHwnd);
}

void create_sav_window_elements()
{
}

LRESULT CALLBACK SavWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam,
    LPARAM lParam)
{
    WORD code = HIWORD(wParam);

    switch (uMsg)
    {
    case WM_CREATE:
        break;
    case WM_COMMAND:
        // if it's not a mouse click, we don't want to check the buttons
        if (code != BN_CLICKED)
            break;

        if (LOWORD(wParam) == LOAD_SAV_BUTTON_ID)
        {
            // TODO: remove magic number
            /*
            char path[512] = {0};
            if (SelectFile(hwnd, path, 512))
            {
                SetDlgItemText(hwnd, SELECTED_IT_FILE_TEXT_ID, path);
                curr_it = parse_it_file(path);
                refresh_it_mod_list();
            }
            */
        }
        else if (LOWORD(wParam) == SAVE_SAV_BUTTON_ID)
        {
            /*
            if (!curr_it)
            {
                MessageBox(hwnd, "Please load an IT file first", "Error", MB_OK);
                break;
            }

            // Saving the changes to the same path that was loaded
            char path[512] = {0};
            GetDlgItemText(hwnd, SELECTED_IT_FILE_TEXT_ID, path, 512);
            update_inputs();
            serialize_it_file(curr_it, path);
            */
        }

        break;
    case WM_DESTROY:
        break;
        
    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}