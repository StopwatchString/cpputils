#ifndef CPPUTILS_WINDOWS_SELECTORS_H
#define CPPUTILS_WINDOWS_SELECTORS_H

#include <windows.h>
#include <shobjidl.h> // For IFileDialog

#include <string>
#include <iostream>

namespace cpputils {
namespace windows {

//-----------------------------------------------------
// wstring_to_string()
//-----------------------------------------------------
void wstring_to_string(std::wstring& wstring, std::string& string)
{
    // TODO:: Use wcstombs_s
    // Note: wcstombs implementations often rely on global statics for processing...
    string.resize(wstring.size());
    std::wcstombs(string.data(), wstring.data(), wstring.size());
}

//-----------------------------------------------------
// OpenWindowsFolderDialogue()
//-----------------------------------------------------
std::string OpenWindowsFolderDialogue(bool printErrors = false)
{
    // Initialize COM
    HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
    if (FAILED(hr)) {
        if (printErrors) std::cerr << "Failed to initialize COM." << std::endl;
        return "";
    }

    std::wstring wFolderPath;

    // Create the FileOpenDialog object
    IFileDialog* pFileDialog = nullptr;
    hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pFileDialog));
    if (SUCCEEDED(hr)) {
        // Set the options for the dialog
        DWORD options;
        pFileDialog->GetOptions(&options);
        pFileDialog->SetOptions(options | FOS_PICKFOLDERS | FOS_FORCEFILESYSTEM);

        // Show the dialog
        hr = pFileDialog->Show(NULL);
        if (SUCCEEDED(hr)) {
            // Get the result
            IShellItem* pItem = nullptr;
            hr = pFileDialog->GetResult(&pItem);
            if (SUCCEEDED(hr)) {
                // Get the folder path
                PWSTR pszFilePath = nullptr;
                hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
                if (SUCCEEDED(hr)) {
                    wFolderPath = pszFilePath;
                    CoTaskMemFree(pszFilePath);
                }
                pItem->Release();
            }
        }
        pFileDialog->Release();
    }

    CoUninitialize();

    if (FAILED(hr)) {
        if (printErrors) std::cerr << "Failed to open folder dialog." << std::endl;
        return "";
    }

    std::string folderPath;
    wstring_to_string(wFolderPath, folderPath);

    return folderPath;
}

} // namespace windows
} // namespace cpputils

#endif
