//  build: g++ -Wall -O2 -DUNICODE -D_UNICODE wmain.cpp -o wmain.exe -lshlwapi
#include <windows.h>
#include <stdio.h>
#include <shellapi.h>

int main(void) 
{
    LPWSTR *szArglist;
    int nArgs;

    // Get the command line string
    LPWSTR commandLine = GetCommandLineW();

    // Convert the command line string to an array of arguments
    szArglist = CommandLineToArgvW(commandLine, &nArgs);

    if (szArglist == NULL) {
        wprintf(L"CommandLineToArgvW failed\n");
        return 1;
    } else {
        for (int i = 0; i < nArgs; i++) {
            wprintf(L"%d: %s\n", i, szArglist[i]);
        }
    }

    // Free the memory allocated by CommandLineToArgvW
    LocalFree(szArglist);

    return 0;
}
