#ifndef DIFF_SELECTOR_H
#define DIFF_SELECTOR_H

#define DIFF_SELECTOR_IDS_START 5000

#include <stdint.h>
#include <windows.h>

typedef struct 
{
    HWND easyButtonHwnd;
    HWND normalButtonHwnd;
    HWND hardButtonHwnd;
    HWND specialButtonHwnd;

    HWND easyTextHwnd;
    HWND normalTextHwnd;
    HWND hardTextHwnd;
    HWND specialTextHwnd;

    int easy_button_id;
    int normal_button_id;
    int hard_button_id;
    int special_button_id;

    int id_start;

    uint32_t diff_mode;
} diff_selector;

static void set_text(char *text, int is_true);
void make_diff_selector(diff_selector *diff_selector, HWND parentHwnd,
    HINSTANCE parentHInstance, int x, int y, int id_start, uint32_t diff_mode,
    int is_add);
void update_text(diff_selector *diff, int diff_mask);
void destroy_diff_selector(diff_selector *diff_selector);

#endif /* !DIFF_SELECTOR_H */