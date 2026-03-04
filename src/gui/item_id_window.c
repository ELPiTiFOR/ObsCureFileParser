#include "item_id_window.h"

#include "main_window.h"

char ITEM_ID_WINDOW_CLASS_NAME[] = "ItemIdWindowClass";
HWND itemIdHwnd;
HINSTANCE itemIdHInstance;

void create_item_ids_buttons(void)
{
    // There are 42 Item IDs, so we arrange them in a 7x6 matrix
    for (size_t j = 0; j < 6; j++)
    {
        for (size_t i = 0; i < 7; i++)
        {
            // Button of the Item ID
            HWND itemIdButtonHwnd = CreateWindow("BUTTON", "" /*button_text*/,
                WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON | BS_BITMAP,
                20 + (i * 80), 20 + (j * 80), 60, 60, itemIdHwnd, (HMENU)(ALL_ITEMS_IDS_START + j * 7 + i), itWindowHInstance, NULL);
            
            // image of the Item ID button
            char image_filename[512] = {0};
            sprintf(image_filename, ".\\resources\\items\\%s.bmp", item_name_from_id(item_ids[j * 7 + i + 1]));
            HBITMAP itemBitmap = (HBITMAP)LoadImage(GetModuleHandle(NULL),
                image_filename, IMAGE_BITMAP, 60, 60, LR_LOADFROMFILE);

            if (itemBitmap == NULL)
            {
                sprintf(image_filename, ".\\resources\\items\\%s.bmp", "NO_ITEM_ID");
                itemBitmap = (HBITMAP)LoadImage(GetModuleHandle(NULL),
                    image_filename, IMAGE_BITMAP, 60, 60, LR_LOADFROMFILE);
            }

            // setting the image
            SendMessage(itemIdButtonHwnd, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)itemBitmap);
        }
    }
}

void OpenItemIdWindow(HWND parentHwnd)
{
    itemIdHInstance = (HINSTANCE)GetWindowLongPtr(parentHwnd, GWLP_HINSTANCE);
    
    itemIdHwnd = CreateWindowEx(
        0,
        ITEM_ID_WINDOW_CLASS_NAME,
        "Item ID Selection | ObsCure File Parser",
        0 | WS_OVERLAPPED | WS_SYSMENU | WS_MINIMIZEBOX | WS_VISIBLE,
        CW_USEDEFAULT, CW_USEDEFAULT,
        580 + 10, 500 + 30,
        parentHwnd,
        NULL,
        itemIdHInstance,
        NULL
    );

    if (!itemIdHwnd)
    {
        return;
    }

    create_item_ids_buttons();

    ShowWindow(itemIdHwnd, SW_SHOW);
    UpdateWindow(itemIdHwnd);
}

void check_all_item_buttons_pressed(WPARAM wParam)
{
    for (size_t i = 0; i < 42; i++)
    {
        if (LOWORD(wParam) == ALL_ITEMS_IDS_START + i)
        {
            // we add 1 to skip the first id because we don't want a
            // "NO_ITEM_ID"
            if (!selected_item)
            {
                printf("wtf selected item is null\n");
            }
            else if (!selected_item->item)
            {
                printf("wtf selected item->item_id is null\n");
            }

            selected_item->item->item_id = item_ids[i + 1];
            selected_item = NULL;
            refresh_it_mod_list();
            DestroyWindow(itemIdHwnd);
        }
    }
}

LRESULT CALLBACK ItemIdWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam,
    LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_CREATE:
        break;
    case WM_COMMAND:
        check_all_item_buttons_pressed(wParam);
        break;
    case WM_DESTROY:
        selected_item = NULL;
        break;
        
    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}