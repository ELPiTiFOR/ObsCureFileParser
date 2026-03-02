#include "utils_gui.h"

int SelectFolder(HWND hwnd, char *path, int size)
{
    CoInitialize(NULL);
    
    IFileDialog *pfd;
    if (SUCCEEDED(CoCreateInstance(&CLSID_FileOpenDialog, NULL, 
                                  CLSCTX_INPROC_SERVER, &IID_IFileOpenDialog, 
                                  (void **)&pfd)))
    {
        DWORD dwOptions;
        pfd->lpVtbl->GetOptions(pfd, &dwOptions);
        pfd->lpVtbl->SetOptions(pfd, dwOptions | FOS_PICKFOLDERS);
        
        pfd->lpVtbl->SetTitle(pfd, L"Select a folder");
        
        if (SUCCEEDED(pfd->lpVtbl->Show(pfd, hwnd)))
        {
            IShellItem *psi;
            if (SUCCEEDED(pfd->lpVtbl->GetResult(pfd, &psi)))
            {
                PWSTR pszPath;
                if (SUCCEEDED(psi->lpVtbl->GetDisplayName(psi, SIGDN_FILESYSPATH, &pszPath)))
                {
                    WideCharToMultiByte(CP_UTF8, 0, pszPath, -1, path, size, NULL, NULL);
                    CoTaskMemFree(pszPath);
                    psi->lpVtbl->Release(psi);
                    pfd->lpVtbl->Release(pfd);
                    CoUninitialize();
                    return TRUE;
                }
                psi->lpVtbl->Release(psi);
            }
        }
        pfd->lpVtbl->Release(pfd);
    }
    CoUninitialize();
    return FALSE;
}

int SelectFile(HWND hwnd, char *path, int size)
{
    CoInitialize(NULL);
    
    IFileDialog *pfd;
    if (SUCCEEDED(CoCreateInstance(&CLSID_FileOpenDialog, NULL, 
                                  CLSCTX_INPROC_SERVER, &IID_IFileOpenDialog, 
                                  (void **)&pfd)))
    {
        //DWORD dwOptions;
        //pfd->lpVtbl->GetOptions(pfd, &dwOptions);
        //pfd->lpVtbl->SetOptions(pfd, dwOptions | FOS_PICKFOLDERS);
        
        pfd->lpVtbl->SetTitle(pfd, L"Select a file");
        
        if (SUCCEEDED(pfd->lpVtbl->Show(pfd, hwnd)))
        {
            IShellItem *psi;
            if (SUCCEEDED(pfd->lpVtbl->GetResult(pfd, &psi)))
            {
                PWSTR pszPath;
                if (SUCCEEDED(psi->lpVtbl->GetDisplayName(psi, SIGDN_FILESYSPATH, &pszPath)))
                {
                    WideCharToMultiByte(CP_UTF8, 0, pszPath, -1, path, size, NULL, NULL);
                    CoTaskMemFree(pszPath);
                    psi->lpVtbl->Release(psi);
                    pfd->lpVtbl->Release(pfd);
                    CoUninitialize();
                    return TRUE;
                }
                psi->lpVtbl->Release(psi);
            }
        }
        pfd->lpVtbl->Release(pfd);
    }
    CoUninitialize();
    return FALSE;
}