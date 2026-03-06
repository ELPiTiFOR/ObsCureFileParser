#ifndef IT_MOD_LIST_H
#define IT_MOD_LIST_H

#include <windows.h>

#include "diff_selector.h"
#include "it_file.h"

// this represents a row in the it_file in the GUI of this program
// the "mod" comes from "modify" because you're modifying the it_file through
// the GUI
typedef struct it_mod_list
{
    // general info
    struct it_mod_list *next;
    HWND index_text_hwnd;
    size_t index;
    
    // everything related to the item_id
    size_t item_id_index;
    HWND item_id_button_hwnd;
    int item_id_button_id;
    HBITMAP item_id_button_bmp;

    // Item Location / Location ID
    HWND item_loc_input_hwnd;
    int item_loc_input_id;

    // Extra info / Document ID
    HWND extra_info_button_hwnd;
    int extra_info_button_id;

    // multiplier
    HWND multiplier_hwnd;
    int multiplier_input_id;

    // diff
    diff_selector diff;

    // delete item button
    HWND delete_item_button_hwnd;
    int delete_item_button_id;

    // the item associated with this column
    it_item *item;
} it_mod_list;

it_mod_list *make_iml(it_mod_list *next, HWND index_text_hwnd, size_t index,
    HWND item_id_button_hwnd, int item_id_button_id, HBITMAP item_id_button_bmp,
    HWND item_loc_input_hwnd, it_item *item);
it_mod_list *make_iml_sentinel(void);
void free_destroy_iml(it_mod_list *iml);
it_mod_list *create_iml_elements(int x, int y, int current_offset,
    int id_offset, it_item **items);

#endif /* !IT_MOD_LIST_H */