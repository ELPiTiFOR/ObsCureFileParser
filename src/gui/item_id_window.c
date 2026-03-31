#include "item_id_window.h"

#include "add_item_window.h"
#include "it_window.h"
#include "utils_gui.h"

char ITEM_ID_WINDOW_CLASS_NAME[] = "ItemIdWindowClass";
HWND itemIdHwnd;
//HINSTANCE itemIdHInstance;
it_mod_list **given_item = NULL;

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
                20 + (i * 80), 20 + (j * 80), 60, 60, itemIdHwnd, (HMENU)(ALL_ITEMS_IDS_START + j * 7 + i), thisHInstance, NULL);
            
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

void OpenItemIdWindow(HWND parentHwnd, it_mod_list **given_item_arg)
{
    //itemIdHInstance = (HINSTANCE)GetWindowLongPtr(parentHwnd, GWLP_HINSTANCE);
    
    given_item = given_item_arg;
    itemIdHwnd = CreateWindowEx(
        0,
        ITEM_ID_WINDOW_CLASS_NAME,
        "Item ID Selection | ObsCure File Parser",
        0 | WS_OVERLAPPED | WS_SYSMENU | WS_MINIMIZEBOX | WS_VISIBLE,
        CW_USEDEFAULT, CW_USEDEFAULT,
        580 + 10, 500 + 30,
        parentHwnd,
        NULL,
        thisHInstance,
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

            if (!given_item)
                printf("!given_item\n");
            if (!given_item[0])
                printf("!given_item[0]\n");
            if (!given_item[0]->item)
                printf("!given_item[0]->item\n");
            given_item[0]->item->item_id = item_ids[i + 1];
            // TODO: this will cause a problem if given_item is NULL
            if (!item_to_add || *given_item != item_to_add->next)
            {
                *given_item = NULL; // TODO: we want to do this only for it_window
            }
            given_item = NULL;
            // it window
            refresh_it_mod_list();
            // TODO: dirty asf, and we have to refresh_it_mod_list before, which
            // is slow. Maybe check parentHwnd?
            if (item_to_add)
                refresh_item_to_add();
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
        if (given_item)
        {
            *given_item = NULL;
            given_item = NULL;
        }
        break;
        
    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}