#include "it_mod_list.h"

it_mod_list *make_iml(it_mod_list *next, size_t index, HWND item_id_button_hwnd,
    int item_id_button_id, it_item *item)
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

    // TODO: determine the next fields:
    //res->item_id_index = item_id_index;
}

it_mod_list *make_iml_sentinel(void)
{
    // TODO: the index of the sentinel is 0, the first element will have
    // index = 1
    return make_iml(NULL, 0, (HWND)0, 0, NULL);
}

void free_destroy_iml(it_mod_list *iml)
{
    if (!iml)
    {
        return;
    }

    it_mod_list *p = iml->next;
    while (p)
    {
        DestroyWindow(p->item_id_button_hwnd);
        p = p->next;
    }
}