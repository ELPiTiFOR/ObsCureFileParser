#include "main_window.h"

#include <stdio.h>
#include <string.h>

#include "document_id.h"
#include "document_id_window.h"
#include "it_file.h"
#include "item_id_window.h"
#include "it_mod_list.h"
#include "map_id.h"
#include "utils.h"
#include "utils_gui.h"

#define MIN(a, b) (((a) < (b)) ? (a) : (b))

// Main window info
char IT_WINDOW_CLASS_NAME[] = "ItWindowClass";
HWND itWindowHwnd;
HINSTANCE itWindowHInstance;

// the IT structure we're modifying
it_file *curr_it = NULL;

// Text showing file path
HWND selectedItFileTextHwnd;

// List of items
size_t current_it_list_offset = 0;
size_t entries_per_page = 9;
it_mod_list *iml = NULL;
it_mod_list *selected_item = NULL;

void create_it_mod_list()
{
    it_item **items = curr_it->items + current_it_list_offset;

    // TODO: check that iml == NULL?
    iml = make_iml_sentinel();
    it_mod_list *p = iml;

    // How many entries we're going to show
    size_t nb_remaining_items = curr_it->len_items - current_it_list_offset;
    size_t entries = MIN(nb_remaining_items, entries_per_page);

    // TODO: the index starts at 0, check if the sentinel is being "printed"!
    for (size_t i = 0; i < entries; i++)
    {
        // Unused right now, but could be useful in the future
        char button_text[7] = {0};
        sprintf(button_text, "0x%04X", items[i]->item_id);

        // Index static text
        char index_text[64] = {0};
        sprintf(index_text, "%3d", current_it_list_offset + i);
        HWND indexTextHwnd = CreateWindow("STATIC", index_text, 
            WS_VISIBLE | WS_CHILD, 20, 90 + (i * 80), 35, 20, itWindowHwnd,
            (HMENU)0, itWindowHInstance, NULL
        );

        // Item ID button
        HWND itemIdButtonHwnd = CreateWindow("BUTTON", "",
            WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON | BS_BITMAP,
            65, 80 + (i * 80), 60, 60, itWindowHwnd, (HMENU)(ITEM_ID_BUTTON_IDS_START + i), itWindowHInstance, NULL
        );

        // image of the Item ID button
        char image_filename[512] = {0};
        sprintf(image_filename, ".\\resources\\items\\%s.bmp", item_name_from_id(items[i]->item_id));
        HBITMAP itemBitmap = (HBITMAP)LoadImage(GetModuleHandle(NULL),
            image_filename, IMAGE_BITMAP, 60, 60, LR_LOADFROMFILE
        );

        if (itemBitmap == NULL)
        {
            sprintf(image_filename, ".\\resources\\items\\%s.bmp", "NO_ITEM_ID");
            itemBitmap = (HBITMAP)LoadImage(GetModuleHandle(NULL),
                image_filename, IMAGE_BITMAP, 60, 60, LR_LOADFROMFILE);
        }

        // setting the image
        SendMessage(itemIdButtonHwnd, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)itemBitmap);

        // Item Location / Location ID input
        char location[7] = {0};
        sprintf(location, "%06X", items[i]->item_loc);
        HWND itemLocInputHwnd = CreateWindow("EDIT", location,
            WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL /*WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON | BS_BITMAP*/,
            135, 100 + (i * 80), 60, 20, itWindowHwnd, (HMENU)(ITEM_LOC_EDIT_IDS_START + i), itWindowHInstance, NULL
        );

        // Extra Info / Document ID
        char document_name[512] = {0};

        if (items[i]->item_id == MAP)
        {
            sprintf(document_name, "%s", map_name_from_id(items[i]->extra_info));
        }
        else if (items[i]->item_id == DOCUMENT
            || items[i]->item_id == PHOTO
            || items[i]->item_id == STATUETTE
            || items[i]->item_id == PIECE_OF_PAPER)
        {
            sprintf(document_name, "%s", document_name_from_id(items[i]->extra_info));
        }
        else if (items[i]->extra_info != 0)
        {
            sprintf(document_name, "%08X", items[i]->extra_info);
        }
        else
        {
            sprintf(document_name, "");
        }

        HWND extraInfoButtonHwnd = CreateWindow("BUTTON", document_name,
            WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            205, 100 + (i * 80), 250, 20, itWindowHwnd, (HMENU)(EXTRA_INFO_BUTTON_IDS_START + i), itWindowHInstance, NULL
        );

        // multiplier
        char multiplier_text[512] = {0};
        sprintf(multiplier_text, "%d", items[i]->multiplier);
        HWND multiplierInputHwnd = CreateWindow("EDIT", multiplier_text,
            WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL /*WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON | BS_BITMAP*/,
            465, 100 + (i * 80), 60, 20, itWindowHwnd, (HMENU)(MULTIPLIER_EDIT_IDS_START + i), itWindowHInstance, NULL
        );

        it_mod_list *next = make_iml(NULL, indexTextHwnd, i, itemIdButtonHwnd,
            ITEM_ID_BUTTON_IDS_START + i, itemBitmap, itemLocInputHwnd, items[i]
        );

        next->item_loc_input_id = ITEM_LOC_EDIT_IDS_START + i;
        next->extra_info_button_hwnd = extraInfoButtonHwnd;
        next->extra_info_button_id = EXTRA_INFO_BUTTON_IDS_START + i;
        next->multiplier_hwnd = multiplierInputHwnd;
        next->multiplier_input_id = MULTIPLIER_EDIT_IDS_START + i;

        p->next = next;
        p = p->next;
    }
}

void refresh_it_mod_list()
{
    free_destroy_iml(iml);
    create_it_mod_list();
}

// updates item locs AND multiplier in the it_item with the values in the inputs
void update_inputs()
{
    it_mod_list *p = iml->next;
    if (!p)
    {
        return;
    }

    while (p)
    {
        // updating Item Location / Location ID
        char input_text[512] = {0};
        GetDlgItemText(itWindowHwnd, p->item_loc_input_id, input_text, 512);
        uint32_t loc = my_atoi_base(input_text, 16);
        if (loc)
        {
            p->item->item_loc = loc;
        }

        // updating multiplier
        memset(input_text, 0, 512);
        GetDlgItemText(itWindowHwnd, p->multiplier_input_id, input_text, 512);
        uint32_t mul = my_atoi_base(input_text, 10);
        if (mul)
        {
            p->item->multiplier = mul;
        }

        p = p->next;
    }
}

void create_main_window_elements(HWND hwnd, HINSTANCE hInstance)
{
    CreateWindow("BUTTON", "Load IT", WS_TABSTOP | WS_VISIBLE | WS_CHILD |
        BS_DEFPUSHBUTTON, 20, 20, 60, 40, hwnd, (HMENU)LOAD_IT_BUTTON_ID,
        hInstance, NULL);

    selectedItFileTextHwnd = CreateWindow("STATIC", "", 
        WS_VISIBLE | WS_CHILD, 100, 30, 520, 20, hwnd,
        (HMENU)SELECTED_IT_FILE_TEXT_ID, hInstance, NULL);

    CreateWindow("BUTTON", "Save", WS_TABSTOP | WS_VISIBLE | WS_CHILD |
        BS_DEFPUSHBUTTON, 640, 20, 40, 40, hwnd, (HMENU)SAVE_IT_BUTTON_ID,
        hInstance, NULL);

    CreateWindow("BUTTON", "Up", WS_TABSTOP | WS_VISIBLE | WS_CHILD |
        BS_DEFPUSHBUTTON, 650, 350, 40, 40, hwnd, (HMENU)UP_IT_BUTTON_ID,
        hInstance, NULL);

    CreateWindow("BUTTON", "Down", WS_TABSTOP | WS_VISIBLE | WS_CHILD |
        BS_DEFPUSHBUTTON, 650, 410, 40, 40, hwnd, (HMENU)DOWN_IT_BUTTON_ID,
        hInstance, NULL);

    CreateWindow("BUTTON", "Top", WS_TABSTOP | WS_VISIBLE | WS_CHILD |
        BS_DEFPUSHBUTTON, 650, 290, 40, 40, hwnd, (HMENU)TOP_IT_BUTTON_ID,
        hInstance, NULL);

    CreateWindow("BUTTON", "Bot", WS_TABSTOP | WS_VISIBLE | WS_CHILD |
        BS_DEFPUSHBUTTON, 650, 470, 40, 40, hwnd, (HMENU)BOT_IT_BUTTON_ID,
        hInstance, NULL);
}

// Check which Item ID button has been pressed,
// open the Item ID Window if needed
void check_item_id_buttons_pressed(WPARAM wParam)
{
    it_mod_list *p = iml->next;
    if (!p)
    {
        return;
    }

    while (p)
    {
        if (selected_item)
        {
            return;
        }

        if (LOWORD(wParam) == p->item_id_button_id)
        {
            selected_item = p;
            OpenItemIdWindow(itWindowHwnd);
            return;
        }

        p = p->next;
    }
}

void check_extra_info_buttons_pressed(WPARAM wParam)
{
    it_mod_list *p = iml->next;
    if (!p)
    {
        return;
    }

    while (p)
    {
        if (selected_item)
        {
            return;
        }

        if (LOWORD(wParam) == p->extra_info_button_id
            && (p->item->item_id == MAP
            || p->item->item_id == DOCUMENT
            || p->item->item_id == PHOTO
            || p->item->item_id == STATUETTE
            || p->item->item_id == PIECE_OF_PAPER))
        {
            selected_item = p;
            OpenDocumentIdWindow(itWindowHwnd);
            return;
        }

        p = p->next;
    }
}

LRESULT CALLBACK MainWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam,
    LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_CREATE:
        break;
    case WM_COMMAND:
        if (LOWORD(wParam) == LOAD_IT_BUTTON_ID)
        {
            // TODO: remove magic number
            char path[512] = {0};
            if (SelectFile(hwnd, path, 512))
            {
                SetDlgItemText(hwnd, SELECTED_IT_FILE_TEXT_ID, path);
                curr_it = parse_it_file(path);
                refresh_it_mod_list();
            }
        }
        else if (LOWORD(wParam) == SAVE_IT_BUTTON_ID)
        {
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
        }
        else if (LOWORD(wParam) == UP_IT_BUTTON_ID)
        {
            if (current_it_list_offset == 0)
            {
                break;
            }

            update_inputs();
            if (current_it_list_offset >= entries_per_page)
            {
                current_it_list_offset -= entries_per_page;
            }
            else
            {
                current_it_list_offset = 0;
            }

            refresh_it_mod_list();
        }
        else if (LOWORD(wParam) == DOWN_IT_BUTTON_ID)
        {
            if (!curr_it)
            {
                break;
            }

            update_inputs();
            if (current_it_list_offset + entries_per_page >= curr_it->len_items)
            {
                //we can do nothing or we can set the offset
                // to len_items - entires_per_page
                break;
            }

            current_it_list_offset += entries_per_page;
            refresh_it_mod_list();
        }
        else if (LOWORD(wParam) == TOP_IT_BUTTON_ID)
        {
            if (current_it_list_offset == 0)
            {
                break;
            }

            update_inputs();
            current_it_list_offset = 0;
            refresh_it_mod_list();
        }
        else if (LOWORD(wParam) == BOT_IT_BUTTON_ID)
        {
            if (!curr_it)
            {
                break;
            }

            size_t final_offset = (curr_it->len_items / entries_per_page)
                * entries_per_page;
            if (final_offset == current_it_list_offset)
            {
                break;
            }

            update_inputs();
            current_it_list_offset = final_offset;
            refresh_it_mod_list();
        }
        else
        {
            check_item_id_buttons_pressed(wParam);
            check_extra_info_buttons_pressed(wParam);
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