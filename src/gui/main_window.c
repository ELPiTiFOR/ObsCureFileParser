#include "main_window.h"

#include <stdio.h>
#include <string.h>

#include "diff_selector.h"
#include "document_id.h"
#include "document_id_window.h"
#include "it_file.h"
#include "item_id_window.h"
#include "it_mod_list.h"
#include "map_id.h"
#include "utils.h"
#include "utils_gui.h"

#define MIN(a, b) (((a) < (b)) ? (a) : (b))

int nb_commands = 0;

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
        p->next = create_iml_elements(20, 80, current_it_list_offset, i, items);
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

        // updating diff_mode
        p->item->diff_mode = p->diff.diff_mode;

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
int check_item_id_buttons_pressed(WPARAM wParam)
{
    it_mod_list *p = iml->next;
    if (!p)
    {
        return 0;
    }

    while (p)
    {
        if (selected_item)
        {
            return 0;
        }

        if (LOWORD(wParam) == p->item_id_button_id)
        {
            selected_item = p;
            OpenItemIdWindow(itWindowHwnd);
            return 1;
        }

        p = p->next;
    }

    return 0;
}

int check_extra_info_buttons_pressed(WPARAM wParam)
{
    it_mod_list *p = iml->next;
    if (!p)
    {
        return 0;
    }

    while (p)
    {
        if (selected_item)
        {
            return 0;
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
            return 1;
        }

        p = p->next;
    }

    return 0;
}

int check_diff_selector_buttons_pressed(WPARAM wParam)
{
    it_mod_list *p = iml->next;
    if (!p)
    {
        return 0;
    }

    while (p)
    {
        if (selected_item)
        {
            return 0;
        }

        int diff_mask = 0;

        if (LOWORD(wParam) == p->diff.easy_button_id)
        {
            diff_mask = 1;
            p->diff.diff_mode ^= diff_mask;
            update_text(&(p->diff), diff_mask);
            return 1;
        }
        else if (LOWORD(wParam) == p->diff.normal_button_id)
        {
            diff_mask = 2;
            p->diff.diff_mode ^= diff_mask;
            update_text(&(p->diff), diff_mask);
            return 1;
        }
        else if (LOWORD(wParam) == p->diff.hard_button_id)
        {
            diff_mask = 4;
            p->diff.diff_mode ^= diff_mask;
            update_text(&(p->diff), diff_mask);
            return 1;
        }
        else if (LOWORD(wParam) == p->diff.special_button_id)
        {
            diff_mask = 8;
            p->diff.diff_mode ^= diff_mask;
            update_text(&(p->diff), diff_mask);
            return 1;
        }

        p = p->next;
    }

    return 0;
}

int check_delete_item_buttons_pressed(WPARAM wParam)
{
    it_mod_list *p = iml->next;
    if (!p)
    {
        return 0;
    }

    while (p)
    {
        if (LOWORD(wParam) == p->delete_item_button_id)
        {
            remove_item_from_it(curr_it, p->item->item_loc);
            refresh_it_mod_list();
            return 1;
        }

        p = p->next;
    }

    return 0;
}

LRESULT CALLBACK MainWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam,
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
            if (check_item_id_buttons_pressed(wParam))
                break;
            if (check_extra_info_buttons_pressed(wParam))
                break;
            if (check_diff_selector_buttons_pressed(wParam))
                break;
            if (check_delete_item_buttons_pressed(wParam))
                break;
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