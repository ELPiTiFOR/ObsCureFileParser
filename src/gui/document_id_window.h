#ifndef DOCUMENT_ID_WINDOW_H
#define DOCUMENT_ID_WINDOW_H

#include <windows.h>

#define ALL_DOCUMENT_IDS_START 20000

extern char DOCUMENT_ID_WINDOW_CLASS_NAME[];
extern HWND documentIdHwnd;
extern HINSTANCE documentIdHInstance;

void OpenDocumentIdWindow(HWND parentHwnd);
LRESULT CALLBACK DocumentIdWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam,
    LPARAM lParam);

#endif /* !DOCUMENT_ID_WINDOW_H */