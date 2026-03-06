#include "document_id_window.h"

#include "document_id.h"
#include "item_id.h"
#include "it_window.h"
#include "map_id.h"

char DOCUMENT_ID_WINDOW_CLASS_NAME[] = "DocumentIdWindowClass";
HWND documentIdHwnd;
HINSTANCE documentIdHInstance;
item_id selected_item_id = NO_ITEM_ID;
size_t nb_ids = 0;

void create_document_ids_buttons(void)
{
    for (size_t i = 0; i < nb_ids; i++)
    {

        if (selected_item_id == MAP)
        {
            CreateWindow("BUTTON", map_names[i],
                WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                20, 20 + (i * 30), 250, 20, documentIdHwnd, (HMENU)(ALL_DOCUMENT_IDS_START + i), documentIdHInstance, NULL);
            continue;
        }

        CreateWindow("BUTTON", document_names[i],
            WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            20, 20 + (i * 30), 250, 20, documentIdHwnd, (HMENU)(ALL_DOCUMENT_IDS_START + i), documentIdHInstance, NULL);
    }
}

void OpenDocumentIdWindow(HWND parentHwnd)
{
    documentIdHInstance = (HINSTANCE)GetWindowLongPtr(parentHwnd, GWLP_HINSTANCE);

    selected_item_id = selected_item->item->item_id;
    if (selected_item_id == MAP)
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
        documentIdHInstance,
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
            if (selected_item_id == MAP)
            {
                selected_item->item->extra_info = map_ids[i];
            }
            else
            {
                selected_item->item->extra_info = document_ids[i];
            }
            selected_item = NULL;
            refresh_it_mod_list();
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
        selected_item = NULL;
        break;
        
    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}