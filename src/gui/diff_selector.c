#include "diff_selector.h"

#include <stdio.h>

static void set_text(char *text, int is_true)
{
    if (is_true)
    {
        text[0] = 'X';
    }
    else
    {
        text[0] = 0;
    }
}

void make_diff_selector(diff_selector *diff_selector, HWND parentHwnd,
    HINSTANCE parentHInstance, int x, int y, int id_start, uint32_t diff_mode,
    int is_add)
{
    char text[2] = {0};

    set_text(text, !!(diff_mode & 1));
    diff_selector->easyButtonHwnd = CreateWindow("BUTTON", text,
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        x + 75, y + 20, 20, 20, parentHwnd,
        (HMENU)(DIFF_SELECTOR_IDS_START + !is_add + id_start + 0), parentHInstance, NULL);
    diff_selector->easyTextHwnd = CreateWindow("STATIC", "E", 
        WS_VISIBLE | WS_CHILD, x + 75, y, 20, 20, parentHwnd,
        (HMENU)0, parentHInstance, NULL);
    diff_selector->easy_button_id = DIFF_SELECTOR_IDS_START + !is_add + id_start + 0;
    printf("diff_selector->easy_button_id = %d\n", diff_selector->easy_button_id);
    printf("should be %d\n", DIFF_SELECTOR_IDS_START + !is_add + id_start + 0);

    set_text(text, !!(diff_mode & 2));
    diff_selector->normalButtonHwnd = CreateWindow("BUTTON", text,
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        x + 50, y + 20, 20, 20, parentHwnd,
        (HMENU)(DIFF_SELECTOR_IDS_START + !is_add + id_start + 1), parentHInstance, NULL);
    diff_selector->normalTextHwnd = CreateWindow("STATIC", "N", 
        WS_VISIBLE | WS_CHILD, x + 50, y, 20, 20, parentHwnd,
        (HMENU)0, parentHInstance, NULL);
    diff_selector->normal_button_id = DIFF_SELECTOR_IDS_START + !is_add + id_start + 1;

    set_text(text, !!(diff_mode & 4));
    diff_selector->hardButtonHwnd = CreateWindow("BUTTON", text,
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        x + 25, y + 20, 20, 20, parentHwnd,
        (HMENU)(DIFF_SELECTOR_IDS_START + !is_add + id_start + 2), parentHInstance, NULL);
    diff_selector->hardTextHwnd = CreateWindow("STATIC", "H", 
        WS_VISIBLE | WS_CHILD, x + 25, y, 20, 20, parentHwnd,
        (HMENU)0, parentHInstance, NULL);
    diff_selector->hard_button_id = DIFF_SELECTOR_IDS_START + !is_add + id_start + 2;

    set_text(text, !!(diff_mode & 8));
    diff_selector->specialButtonHwnd = CreateWindow("BUTTON", text,
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        x, y + 20, 20, 20, parentHwnd,
        (HMENU)(DIFF_SELECTOR_IDS_START + !is_add + id_start + 3), parentHInstance, NULL);
    diff_selector->specialTextHwnd = CreateWindow("STATIC", "S", 
        WS_VISIBLE | WS_CHILD, x, y, 20, 20, parentHwnd,
        (HMENU)0, parentHInstance, NULL);
    diff_selector->special_button_id = DIFF_SELECTOR_IDS_START + !is_add + id_start + 3;

    diff_selector->diff_mode = diff_mode;

    diff_selector->id_start = id_start;
}

void update_text(diff_selector *diff, int diff_mask)
{
    char text[2] = {0};
    set_text(text, !!(diff->diff_mode & diff_mask));
    HWND *hwnd = NULL;
    if (diff_mask == 1)
    {
        hwnd = &(diff->easyButtonHwnd);
    }
    else if (diff_mask == 2)
    {
        hwnd = &(diff->normalButtonHwnd);
    }
    else if (diff_mask == 4)
    {
        hwnd = &(diff->hardButtonHwnd);
    }
    else
    {
        hwnd = &(diff->specialButtonHwnd);
    }

    printf("hola update_text\n");
    SetWindowText(*hwnd, text);
}

void destroy_diff_selector(diff_selector *diff_selector)
{
    DestroyWindow(diff_selector->easyButtonHwnd);
    DestroyWindow(diff_selector->normalButtonHwnd);
    DestroyWindow(diff_selector->hardButtonHwnd);
    DestroyWindow(diff_selector->specialButtonHwnd);
    DestroyWindow(diff_selector->easyTextHwnd);
    DestroyWindow(diff_selector->normalTextHwnd);
    DestroyWindow(diff_selector->hardTextHwnd);
    DestroyWindow(diff_selector->specialTextHwnd);
}