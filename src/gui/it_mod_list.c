#include "it_mod_list.h"

#include <stdio.h>
#include <string.h>

#include "diff_selector.h"
#include "document_id.h"
#include "item_id.h"
#include "it_file.h"
#include "it_window.h"
#include "map_id.h"
#include "utils.h"
#include "utils_gui.h"

it_mod_list *make_iml(it_mod_list *next, HWND index_text_hwnd, size_t index,
    HWND item_id_button_hwnd, int item_id_button_id, HBITMAP item_id_button_bmp,
    HWND item_loc_input_hwnd, it_item *item)
{
    it_mod_list *res = calloc(1, sizeof(it_mod_list));
    if (!res)
    {
        return NULL;
    }

    res->next = NULL;
    res->index = index;
    res->item_id_button_hwnd = item_id_button_hwnd;
    res->item_id_button_id = item_id_button_id;
    res->item_id_button_bmp = item_id_button_bmp;
    res->index_text_hwnd = index_text_hwnd;
    res->item_loc_input_hwnd = item_loc_input_hwnd;
    memset(&(res->diff), 0, sizeof(diff_selector));

    // TODO: determine the next fields:
    //res->item_id_index = item_id_index;
    res->item = item;
    res->add_item_button_hwnd = (HWND)0;
}

it_mod_list *make_iml_sentinel(void)
{
    // TODO: the index of the sentinel is 0, the first element will have
    // index = 1
    return make_iml(NULL, (HWND)0, 0, (HWND)0, 0, (HBITMAP)0, (HWND)0, NULL);
}

void free_destroy_iml(it_mod_list *iml)
{
    printf("destroying iml\n");
    if (!iml)
    {
        return;
    }

    it_mod_list *p = iml->next;
    it_mod_list *p2 = p;
    while (p)
    {
        printf("destroying p:\n");
        print_it_item(p->item);
        DestroyWindow(p->item_id_button_hwnd);
        DestroyWindow(p->index_text_hwnd);
        DestroyWindow(p->item_loc_input_hwnd);
        DestroyWindow(p->extra_info_button_hwnd);
        DestroyWindow(p->multiplier_hwnd);
        DeleteObject(p->item_id_button_bmp);
        destroy_diff_selector(&(p->diff));
        DestroyWindow(p->delete_item_button_hwnd);
        if (p->add_item_button_hwnd)
            DestroyWindow(p->add_item_button_hwnd);
        p = p->next;
        free(p2);
        p2 = p;
    }

    free(iml);
}

void update_iml_item_loc(it_mod_list *iml, HWND hwnd, int input_id)
{
    char input_text[512] = {0};
    GetDlgItemText(hwnd, input_id, input_text, 512);
    uint32_t loc = my_atoi_base(input_text, 16);
    if (loc)
    {
        printf("loc = %8X\n", loc);
        iml->item->item_loc = loc;
    }
}

void update_iml_multiplier(it_mod_list *iml, HWND hwnd, int input_id)
{
    char input_text[512] = {0};
    GetDlgItemText(hwnd, input_id, input_text, 512);
    uint32_t mul = my_atoi_base(input_text, 10);
    if (mul)
    {
        iml->item->multiplier = mul;
    }
}

int check_iml_diff_selector_buttons_pressed(it_mod_list *iml, WPARAM wParam)
{
    //printf("CHECKING IML DIFF SELECTOR PRESSED\nBefore:%02X\n", iml->diff.diff_mode);
    int diff_mask = 0;

    if (LOWORD(wParam) == iml->diff.easy_button_id)
    {
        diff_mask = 1;
    }
    else if (LOWORD(wParam) == iml->diff.normal_button_id)
    {
        diff_mask = 2;
    }
    else if (LOWORD(wParam) == iml->diff.hard_button_id)
    {
        diff_mask = 4;
    }
    else if (LOWORD(wParam) == iml->diff.special_button_id)
    {
        diff_mask = 8;
    }

    if (!diff_mask)
    {
        return 0;
    }

    iml->diff.diff_mode ^= diff_mask;
    update_text(&(iml->diff), diff_mask);
    // we transfer the info to the it_item directly
    // this shouldn't affect performance, although there are quite a few
    // pointers being dereferenced
    iml->item->diff_mode = iml->diff.diff_mode;
    //printf("After:%02X\n", iml->diff.diff_mode);
    return 1;
}

it_mod_list *create_iml_elements(int x, int y, int current_offset,
    int id_offset, it_item **items, int is_add, HWND hwnd)
{
    // Unused right now, but could be useful in the future
    //char button_text[7] = {0};
    //sprintf(button_text, "0x%04X", items[id_offset]->item_id);

    // Index static text
    char index_text[64] = {0};
    sprintf(index_text, "%3d", current_offset + id_offset);
    HWND indexTextHwnd = CreateWindow("STATIC", index_text, 
        WS_VISIBLE | WS_CHILD, x, y + 10 + (id_offset * 80), 35, 20, hwnd,
        (HMENU)0, thisHInstance, NULL
    );

    // Delete item button
    HWND deleteItemHwnd = CreateWindow("BUTTON", "X",
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        x, y + 40 + (id_offset * 80), 20, 20, hwnd,
        (HMENU)(DELETE_ITEM_BUTTON_IDS_START + id_offset), thisHInstance, NULL
    );

    // Item ID button
    HWND itemIdButtonHwnd = CreateWindow("BUTTON", "",
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON | BS_BITMAP,
        x + 45, y + (id_offset * 80), 60, 60, hwnd,
        (HMENU)(ITEM_ID_BUTTON_IDS_START + id_offset), thisHInstance, NULL
    );

    // image of the Item ID button
    char image_filename[512] = {0};
    sprintf(image_filename, ".\\resources\\items\\%s.bmp",
        item_name_from_id(items[id_offset]->item_id));
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
    sprintf(location, "%06X", items[id_offset]->item_loc);
    HWND itemLocInputHwnd = CreateWindow("EDIT", location,
        WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL,
        x + 115, y + 20 + (id_offset * 80), 60, 20, hwnd,
        (HMENU)(ITEM_LOC_EDIT_IDS_START + id_offset), thisHInstance, NULL
    );

    // Extra Info / Document ID
    char document_name[512] = {0};

    if (items[id_offset]->item_id == MAP)
    {
        sprintf(document_name, "%s", map_name_from_id(items[id_offset]->extra_info));
    }
    else if (items[id_offset]->item_id == DOCUMENT
        || items[id_offset]->item_id == PHOTO
        || items[id_offset]->item_id == STATUETTE
        || items[id_offset]->item_id == PIECE_OF_PAPER)
    {
        sprintf(document_name, "%s", document_name_from_id(items[id_offset]->extra_info));
    }
    else if (items[id_offset]->extra_info != 0)
    {
        sprintf(document_name, "%08X", items[id_offset]->extra_info);
    }
    else
    {
        sprintf(document_name, "");
    }

    HWND extraInfoButtonHwnd = CreateWindow("BUTTON", document_name,
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        x + 185, y + 20 + (id_offset * 80), 250, 20, hwnd,
        (HMENU)(EXTRA_INFO_BUTTON_IDS_START + id_offset), thisHInstance, NULL
    );

    // Add item button
    HWND addItemHwnd = (HWND)0;
    if (is_add)
    {
        addItemHwnd = CreateWindow("BUTTON", "Add",
            WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            x + 215, y + 40 + (id_offset * 80), 50, 20, hwnd,
            (HMENU)(ADD_ITEM_BUTTON_IDS_START + id_offset), thisHInstance, NULL
        );
    }

    // multiplier
    char multiplier_text[512] = {0};
    sprintf(multiplier_text, "%d", items[id_offset]->multiplier);
    HWND multiplierInputHwnd = CreateWindow("EDIT", multiplier_text,
        WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL,
        x + 445, y + 20 + (id_offset * 80), 60, 20, hwnd,
        (HMENU)(MULTIPLIER_EDIT_IDS_START + id_offset), thisHInstance, NULL
    );

    it_mod_list *next = make_iml(NULL, indexTextHwnd, id_offset, itemIdButtonHwnd,
        ITEM_ID_BUTTON_IDS_START + id_offset, itemBitmap, itemLocInputHwnd, items[id_offset]
    );

    // diff
    make_diff_selector(&(next->diff), hwnd, thisHInstance,
        x + 515, y + 10 + (id_offset * 80), id_offset * 100, items[id_offset]->diff_mode, is_add);

    next->item_loc_input_id = ITEM_LOC_EDIT_IDS_START + id_offset;
    next->extra_info_button_hwnd = extraInfoButtonHwnd;
    next->extra_info_button_id = EXTRA_INFO_BUTTON_IDS_START + id_offset;
    next->multiplier_hwnd = multiplierInputHwnd;
    next->multiplier_input_id = MULTIPLIER_EDIT_IDS_START + id_offset;
    next->delete_item_button_hwnd = deleteItemHwnd;
    next->delete_item_button_id = DELETE_ITEM_BUTTON_IDS_START + id_offset;
    next->add_item_button_hwnd = addItemHwnd;
    next->add_item_button_id = ADD_ITEM_BUTTON_IDS_START + id_offset;

    return next;
}