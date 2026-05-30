#include "document_id_window.h"

#include "add_item_window.h"
#include "document_id.h"
#include "item.h"
#include "it_window.h"
#include "map_id.h"
#include "utils_gui.h"

char DOCUMENT_ID_WINDOW_CLASS_NAME[] = "DocumentIdWindowClass";
HWND documentIdHwnd;
it_mod_list **selected_item_doc;
//HINSTANCE documentIdHInstance;
item_type selected_item_id = NOITEM_T;
size_t nb_ids = 0;

void create_document_ids_buttons(void)
{
    for (size_t i = 0; i < nb_ids; i++)
    {

        if (selected_item_id == MAP_T)
        {
            CreateWindow("BUTTON", map_names[i],
                WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                20, 20 + (i * 30), 250, 20, documentIdHwnd, (HMENU)(ALL_DOCUMENT_IDS_START + i), thisHInstance, NULL);
            continue;
        }

        CreateWindow("BUTTON", document_names[i],
            WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            20, 20 + (i * 30), 250, 20, documentIdHwnd, (HMENU)(ALL_DOCUMENT_IDS_START + i), thisHInstance, NULL);
    }
}

void OpenDocumentIdWindow(HWND parentHwnd, it_mod_list **given_item)
{
    //documentIdHInstance = (HINSTANCE)GetWindowLongPtr(parentHwnd, GWLP_HINSTANCE);

    selected_item_doc = given_item;
    selected_item_id = selected_item_doc[0]->item->item_id;
    if (selected_item_id == MAP_T)
    {
        nb_ids = NB_MAP_IDS;
    }
    else
    {
        nb_ids = NB_DOCUMENT_IDS;
    }
    
    documentIdHwnd = CreateWindowEx(
        0,
        DOCUMENT_ID_WINDOW_CLASS_NAME,
        "Document ID Selection | ObsCure File Parser",
        0 | WS_OVERLAPPED | WS_SYSMENU | WS_MINIMIZEBOX | WS_VISIBLE,
        CW_USEDEFAULT, CW_USEDEFAULT,
        290, 50 + (30 * nb_ids),
        parentHwnd,
        NULL,
        thisHInstance,
        NULL
    );

    if (!documentIdHwnd)
    {
        return;
    }

    printf("before creating buttons\n");
    create_document_ids_buttons();
    printf("after creating buttons\n");

    ShowWindow(documentIdHwnd, SW_SHOW);
    UpdateWindow(documentIdHwnd);
}

void check_all_document_buttons_pressed(WPARAM wParam)
{
    for (size_t i = 0; i < nb_ids; i++)
    {
        if (LOWORD(wParam) == ALL_DOCUMENT_IDS_START + i)
        {
            if (selected_item_id == MAP_T)
            {
                selected_item_doc[0]->item->extra_info = map_ids[i];
            }
            else
            {
                selected_item_doc[0]->item->extra_info = document_ids[i];
            }
            if (!item_to_add || selected_item_doc[0] != item_to_add->next)
                selected_item_doc[0] = NULL;
            refresh_it_mod_list();
            if (item_to_add)
                refresh_item_to_add();
            DestroyWindow(documentIdHwnd);
        }
    }
}

LRESULT CALLBACK DocumentIdWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam,
    LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_CREATE:
        break;
    case WM_COMMAND:
        check_all_document_buttons_pressed(wParam);
        break;
    case WM_DESTROY:
        if (!item_to_add || selected_item_doc[0] != item_to_add->next)
            selected_item_doc[0] = NULL;
        break;
        
    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}