#include "main_window.h"

#include <stdio.h>

#include "it_file.h"
#include "it_mod_list.h"
#include "utils_gui.h"

char MAIN_WINDOW_CLASS_NAME[] = "MainWindowClass";
it_file *curr_it = NULL;
HWND selectedItFileTextHwnd;

HWND thisHwnd;
HINSTANCE thisHInstance;

size_t current_it_list_offset = 0;
size_t entries_per_page = 9;

it_mod_list *iml = NULL;

void create_it_mod_list()
{
    it_item **items = curr_it->items + current_it_list_offset;
    // TODO: check that iml == NULL?
    iml = make_iml_sentinel();
    it_mod_list *p = iml;

    // TODO: refacto (macro MIN()?)
    size_t entries;
    size_t nb_remaining_items = curr_it->len_items - current_it_list_offset;
    if (nb_remaining_items >= entries_per_page)
    {
        entries = entries_per_page;
    }
    else
    {
        entries = nb_remaining_items;
    }

    for (size_t i = 0; i < entries; i++)
    {
        char button_text[7] = {0};
        sprintf(button_text, "0x%04X", items[i]->item_id);
        HWND itemIdButtonHwnd = CreateWindow("BUTTON", button_text,
            WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            20, 80 + (i * 80), 60, 60, thisHwnd, (HMENU)ITEM_ID_BUTTON_IDS_START + i, thisHInstance, NULL);
        it_mod_list *next = make_iml(NULL, i, itemIdButtonHwnd,
            ITEM_ID_BUTTON_IDS_START + i, items[i]);
        p->next = next;
        p = p->next;
    }
}

void refresh_it_mod_list()
{
    free_destroy_iml(iml);
    create_it_mod_list();
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
                print_it_file(curr_it);
                create_it_mod_list();
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
            serialize_it_file(curr_it, path);
        }
        else if (LOWORD(wParam) == UP_IT_BUTTON_ID)
        {
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
            if (current_it_list_offset + entries_per_page >= curr_it->len_items)
            {
                //we can do nothing or we can set the offset
                // to len_items - entires_per_page
                break;
            }

            current_it_list_offset += entries_per_page;
            refresh_it_mod_list();
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