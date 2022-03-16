// This file is part of Micropolis-SDL2PP
// Micropolis-SDL2PP is based on Micropolis
//
// Copyright © 2022 Leeor Dicker
//
// Portions Copyright © 1989-2007 Electronic Arts Inc.
//
// Micropolis-SDL2PP is free software; you can redistribute it and/or modify
// it under the terms of the GNU GPLv3, with additional terms. See the README
// file, included in this distribution, for details.

#include "FileIo.h"

#define NOMINMAX
#include <windows.h>
#include <ShlObj_core.h>
#include <Shlwapi.h>
#undef NOMINMAX


#include <locale>
#include <codecvt>
#include <sstream>
#include <stdexcept>


namespace
{
    COMDLG_FILTERSPEC FileTypeFilter[] =
    {
        { L"Micropolis City", L"*.cty"}
    };


    const std::string StringFromWString(const std::wstring& str)
    {
        const auto length = WideCharToMultiByte(CP_UTF8, 0, &str.at(0), (int)str.size(), nullptr, 0, nullptr, nullptr);

        if (length <= 0)
        {
            throw std::runtime_error("WideCharToMultiByte() failed.");
        }

        std::string out(length, 0);
        WideCharToMultiByte(CP_UTF8, 0, &str.at(0), (int)str.size(), &out.at(0), length, nullptr, nullptr);
        return out;
    }

};


FileIo::FileIo(SDL_Window& window):
	mWindow(window)
{
	SDL_GetWindowWMInfo(&mWindow, &mWmInfo);

    wchar_t* path{ nullptr };
    if (SHGetKnownFolderPath(FOLDERID_Documents, KF_FLAG_CREATE, NULL, &path) != S_OK)
    {
        throw std::runtime_error("Unable to get user documents path.");
    }

    std::wstringstream ss;
    ss << path << L"\\Micropolis";
    mSavePathW = ss.str();
    mSavePath = StringFromWString(mSavePathW);

    if (!PathFileExists(mSavePathW.c_str()))
    {
        CreateDirectory(mSavePathW.c_str(), nullptr);
    }
}


void FileIo::pickSaveFile()
{
}


/**
 * There are a few assumptions here regarding the Win32 API's handling of the file
 * open interface -- creating items, setting parameters and options, etc. are all
 * assumed to not fail. If they fail this will undoubtedly result in other issues.
 * 
 * The checks are not present for the sake of brevity. If it's determined that the
 * hresult checks are necessary they can be added as needed.
 */
void FileIo::pickOpenFile()
{
    IFileDialog* fileOpenDialog{ nullptr };
    if (!SUCCEEDED(CoCreateInstance(CLSID_FileOpenDialog, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&fileOpenDialog))))
    {
        throw std::runtime_error("Unable to create file IO dialog");
    }

    IShellItem* defaultFolder{ nullptr };
    SHCreateItemFromParsingName(mSavePathW.c_str(), nullptr, IID_PPV_ARGS(&defaultFolder));
    fileOpenDialog->SetDefaultFolder(defaultFolder);
    defaultFolder->Release();

    DWORD dwFlags{};
    fileOpenDialog->GetOptions(&dwFlags);

    fileOpenDialog->SetOptions(dwFlags | FOS_FORCEFILESYSTEM);
    fileOpenDialog->SetFileTypes(ARRAYSIZE(FileTypeFilter), FileTypeFilter);
    fileOpenDialog->SetFileTypeIndex(1);
    fileOpenDialog->SetDefaultExtension(L"cty");

    if (!SUCCEEDED(fileOpenDialog->Show(mWmInfo.info.win.window)))
    {
        fileOpenDialog->Release();
        return;
    }

    IShellItem* item{ nullptr };
    if (SUCCEEDED(fileOpenDialog->GetResult(&item)))
    {
        PWSTR filePath{ nullptr };

        if (!SUCCEEDED(item->GetDisplayName(SIGDN_FILESYSPATH, &filePath)))
        {
            throw std::runtime_error("Unable to get file name");
        }
        mFileNameW = filePath;
        CoTaskMemFree(filePath);

        item->Release();
    }

    fileOpenDialog->Release();

    std::size_t location = mFileNameW.find_last_of(L"/\\");
    mFileNameW = mFileNameW.substr(location + 1);
    mFileName = StringFromWString(mFileNameW);
}
