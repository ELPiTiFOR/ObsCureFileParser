#include "it_mod_list.h"

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

    // TODO: determine the next fields:
    //res->item_id_index = item_id_index;
    res->item = item;
}

it_mod_list *make_iml_sentinel(void)
{
    // TODO: the index of the sentinel is 0, the first element will have
    // index = 1
    return make_iml(NULL, (HWND)0, 0, (HWND)0, 0, (HBITMAP)0, (HWND)0, NULL);
}

void free_destroy_iml(it_mod_list *iml)
{
    if (!iml)
    {
        return;
    }

    it_mod_list *p = iml->next;
    it_mod_list *p2 = p;
    while (p)
    {
        DestroyWindow(p->item_id_button_hwnd);
        DestroyWindow(p->index_text_hwnd);
        DestroyWindow(p->item_loc_input_hwnd);
        DestroyWindow(p->extra_info_button_hwnd);
        DestroyWindow(p->multiplier_hwnd);
        DeleteObject(p->item_id_button_bmp);
        p = p->next;
        free(p2);
        p2 = p;
    }

    free(iml);
}