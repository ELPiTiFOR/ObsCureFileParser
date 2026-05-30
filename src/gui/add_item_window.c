#include "add_item_window.h"

#include "document_id_window.h"
#include "it_file.h"
#include "it_window.h"
#include "item_id_window.h"
#include "utils_gui.h"

char ADD_ITEM_WINDOW_CLASS_NAME[] = "AddItemWindowClass";
HWND addItemWindowHwnd;

it_item it_item_to_add = {0};
it_mod_list *item_to_add = NULL;
HWND addItemUpHwnd = (HWND)NULL;
HWND addItemDownHwnd = (HWND)NULL;
int index_of_selected_item = 0;

void OpenAddItemWindow(HWND parentHwnd, int index)
{
    addItemWindowHwnd = CreateWindowEx(
        0,
        ADD_ITEM_WINDOW_CLASS_NAME,
        "Add Item | ObsCure File Parser",
        0 | WS_OVERLAPPED | WS_SYSMENU | WS_MINIMIZEBOX,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        660,
        160,
        NULL,
        NULL,
        thisHInstance,
        NULL
    );

    if (!addItemWindowHwnd)
    {
        return;
    }

    index_of_selected_item = index + current_it_list_offset;
    printf("index_of_selected_item: %d\n", index_of_selected_item);
    create_add_item_window_elements();
    ShowWindow(addItemWindowHwnd, SW_SHOW);
    UpdateWindow(addItemWindowHwnd);
}

void create_add_item_window_elements()
{
    //item_to_add
    it_item *p_it_item_to_add = &it_item_to_add;
    // TODO: free?
    item_to_add = make_iml_sentinel();
    item_to_add->next = create_iml_elements(20, 20, 0, 0, &p_it_item_to_add,
        0, addItemWindowHwnd);
    printf("item_to_add->next newly created: %p\n", item_to_add->next);
    
    addItemUpHwnd = CreateWindow("BUTTON", "Add Up",
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        20, 100, 80, 20, addItemWindowHwnd,
        (HMENU)(ADD_ITEM_CONFIRMATION_UP_BUTTON_ID), thisHInstance, NULL
    );
    addItemDownHwnd = CreateWindow("BUTTON", "Add Down",
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        140, 100, 100, 20, addItemWindowHwnd,
        (HMENU)(ADD_ITEM_CONFIRMATION_DOWN_BUTTON_ID), thisHInstance, NULL
    );
}

void refresh_item_to_add()
{
    //printf("item_to_add that's gonna be destroyed: %p next: %p\n", item_to_add, item_to_add->next);
    free_destroy_iml(item_to_add);
    if (addItemUpHwnd)
    {
        DestroyWindow(addItemUpHwnd);
    }
    if (addItemDownHwnd)
    {
        DestroyWindow(addItemDownHwnd);
    }
    create_add_item_window_elements();
}

LRESULT CALLBACK AddItemWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam,
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

        if (LOWORD(wParam) == ITEM_ID_BUTTON_IDS_START)
        {
            OpenItemIdWindow(addItemWindowHwnd, &(item_to_add->next));
        }
        else if (LOWORD(wParam) == item_to_add->next->extra_info_button_id
            && (item_to_add->next->item->item_id == MAP_T
            || item_to_add->next->item->item_id == DOCUMENT_T
            || item_to_add->next->item->item_id == PHOTO_T
            || item_to_add->next->item->item_id == STATUETTE_T
            || item_to_add->next->item->item_id == PIECE_OF_PAPER_T))
        {
            // TODO: selected item should be a pointer of pointer
            // but there's a problem, selected_item is supposed to be null while
            // no item is being selected, but we commented the *given_item =
            // NULL line from item_id_window
            OpenDocumentIdWindow(addItemWindowHwnd, &(item_to_add->next));
        }
        else if (LOWORD(wParam) == ADD_ITEM_CONFIRMATION_UP_BUTTON_ID)
        {
            printf("pressed add item confirmation button\n");
            // update it_item with inputs values
            update_iml_multiplier(item_to_add->next, addItemWindowHwnd, item_to_add->next->multiplier_input_id);
            update_iml_item_loc(item_to_add->next, addItemWindowHwnd, item_to_add->next->item_loc_input_id);
            // add item
            insert_item_to_it_at(curr_it, item_to_add->next->item, index_of_selected_item);
            refresh_it_mod_list();
            free_destroy_iml(item_to_add);
            item_to_add = NULL;
            DestroyWindow(addItemWindowHwnd);
        }
        else if (LOWORD(wParam) == ADD_ITEM_CONFIRMATION_DOWN_BUTTON_ID)
        {
            printf("pressed add item confirmation button\n");
            // update it_item with inputs values
            update_iml_multiplier(item_to_add->next, addItemWindowHwnd, item_to_add->next->multiplier_input_id);
            update_iml_item_loc(item_to_add->next, addItemWindowHwnd, item_to_add->next->item_loc_input_id);
            // add item
            insert_item_to_it_at(curr_it, item_to_add->next->item, index_of_selected_item + 1);
            refresh_it_mod_list();
            free_destroy_iml(item_to_add);
            item_to_add = NULL;
            DestroyWindow(addItemWindowHwnd);
        }
        else
        {
            printf("else %d\n", LOWORD(wParam));
            if (check_iml_diff_selector_buttons_pressed(item_to_add->next, wParam))
                break;
        }

        break;
    case WM_DESTROY:
        free_destroy_iml(item_to_add);
        item_to_add = NULL;
        break;
        
    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}