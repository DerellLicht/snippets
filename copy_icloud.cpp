// copy_icloud.cpp
//  This program was created by ClaudeAI, 07.13.26
//
// Copies files from an iCloud Drive folder to a local destination folder.
// Handles iCloud's "on-demand download" placeholder files by forcing them
// to hydrate (download) before copying.
//
// Build (MinGW-GCC, GCC 9+, no extra libs needed):
//   g++ -std=c++17 -O2 -o copy_icloud.exe copy_icloud.cpp
//
// If your MinGW is older than GCC 9, std::filesystem may live in a
// separate lib; add -lstdc++fs to the command above.

#include <filesystem>
#include <iostream>
#include <string>
#include <windows.h>

namespace fs = std::filesystem;

// FILE_ATTRIBUTE_RECALL_ON_DATA_ACCESS isn't always defined in older
// MinGW headers, so define it defensively.
#ifndef FILE_ATTRIBUTE_RECALL_ON_DATA_ACCESS
#define FILE_ATTRIBUTE_RECALL_ON_DATA_ACCESS 0x00400000
#endif

// Returns true if the file is an iCloud "placeholder" that hasn't been
// downloaded to disk yet (cloud-only / offline file).
bool isCloudPlaceholder(const fs::path& p) {
    DWORD attrs = GetFileAttributesW(p.c_str());
    if (attrs == INVALID_FILE_ATTRIBUTES) return false;
    return (attrs & FILE_ATTRIBUTE_RECALL_ON_DATA_ACCESS) != 0 ||
           (attrs & FILE_ATTRIBUTE_OFFLINE) != 0;
}

// Forces Windows' Cloud Filter API (which iCloud for Windows uses) to
// download the file's real content. Simply opening + reading a chunk
// is enough to trigger hydration.
bool hydrateFile(const fs::path& p) {
    HANDLE h = CreateFileW(
        p.c_str(),
        GENERIC_READ,
        FILE_SHARE_READ,
        nullptr,
        OPEN_EXISTING,
        FILE_FLAG_SEQUENTIAL_SCAN,
        nullptr);

    if (h == INVALID_HANDLE_VALUE) {
        std::wcerr << L"  Could not open for hydration: " << p.filename().wstring() << L"\n";
        return false;
    }

    char buf[65536];
    DWORD bytesRead = 0;
    // Reading forces the provider to fetch content; loop until EOF so
    // the whole file is pulled down, not just the first chunk.
    while (ReadFile(h, buf, sizeof(buf), &bytesRead, nullptr) && bytesRead > 0) {
        // discard - we just want the OS to materialize the file on disk
    }

    CloseHandle(h);
    return true;
}

// Copies all regular files from src to dst. If recursive is true,
// subdirectories are copied too (mirroring the folder structure).
int copyFolder(const fs::path& src, const fs::path& dst, bool recursive) {
    std::error_code ec;
    fs::create_directories(dst, ec);

    int copiedCount = 0;

    auto handleEntry = [&](const fs::directory_entry& entry) {
        if (!entry.is_regular_file()) return;

        const fs::path& srcPath = entry.path();
        fs::path relative = fs::relative(srcPath, src);
        fs::path dstPath = dst / relative;

        fs::create_directories(dstPath.parent_path(), ec);

        if (isCloudPlaceholder(srcPath)) {
            std::wcout << L"Downloading from iCloud: " << relative.wstring() << L"\n";
            hydrateFile(srcPath);
        }

        std::error_code copyEc;
        fs::copy_file(srcPath, dstPath, fs::copy_options::overwrite_existing, copyEc);

        if (copyEc) {
            std::wcerr << L"  FAILED to copy " << relative.wstring()
                        << L": " << copyEc.message().c_str() << L"\n";
        } else {
            std::wcout << L"  Copied: " << relative.wstring() << L"\n";
            ++copiedCount;
        }
    };

    if (recursive) {
        for (auto& entry : fs::recursive_directory_iterator(src)) handleEntry(entry);
    } else {
        for (auto& entry : fs::directory_iterator(src)) handleEntry(entry);
    }

    return copiedCount;
}

int main(int argc, char* argv[]) {
    // Allow console to print non-ASCII (accented / unicode) filenames.
    SetConsoleOutputCP(CP_UTF8);

    // Default iCloud Drive root: C:\Users\<you>\iCloudDrive
    const wchar_t* userProfile = _wgetenv(L"USERPROFILE");
    if (!userProfile) {
        std::wcerr << L"Could not determine USERPROFILE.\n";
        return 1;
    }
    fs::path iCloudRoot = fs::path(userProfile) / L"iCloudDrive";

    // --- EDIT THESE TWO PATHS ---
    fs::path sourceFolder = iCloudRoot / L"SomeFolder";   // subfolder inside iCloud Drive
    fs::path destFolder   = L"D:\\LocalCopy";             // where to copy files to
    bool recursive        = false;                        // include subfolders?
    // ----------------------------

    if (!fs::exists(sourceFolder)) {
        std::wcerr << L"Source folder not found: " << sourceFolder.wstring() << L"\n";
        std::wcerr << L"(Confirm the actual iCloud Drive path in File Explorer -> "
                       L"right-click iCloud Drive -> Properties)\n";
        return 1;
    }

    std::wcout << L"Source: " << sourceFolder.wstring() << L"\n";
    std::wcout << L"Dest:   " << destFolder.wstring() << L"\n\n";

    int copied = copyFolder(sourceFolder, destFolder, recursive);

    std::wcout << L"\nDone. Copied " << copied << L" file(s).\n";
    return 0;
}
