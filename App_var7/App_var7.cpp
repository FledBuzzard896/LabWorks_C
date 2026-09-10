#include <iostream>
#include <windows.h>
#include <stdio.h>


using namespace std;


typedef int (*ProcessFileFunc)(const char*, const char*);

int main(int argc, char* argv[])
{
    if (argc < 3) {
        printf("Usage: App_var7.exe <input> <output>\n");
        return 1;
    }

    HMODULE hDll = LoadLibraryA("Dll_var7.dll");  
    if (!hDll) {
        printf("Cannot load DLL. Error: %lu\n", GetLastError());
        return 2;
    }

    ProcessFileFunc fn = (ProcessFileFunc)GetProcAddress(hDll, "ProcessFile");
    if (!fn) {
        printf("Cannot find ProcessFile.\n");
        FreeLibrary(hDll);
        return 3;
    }

    int result = fn(argv[1], argv[2]);
    if (result >= 0)
        printf("Removed spaces: %d\n", result);
    else
        printf("Error code: %d\n", result);

    FreeLibrary(hDll);
    return 0;
}
