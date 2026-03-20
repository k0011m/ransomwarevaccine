#ifndef MY_DLL_TESTH
#define MY_DLL_TESTH

#include <windows.h>
#include <Dbghelp.h>
#include <winternl.h>
#include <stdio.h>
#pragma comment(lib, "Dbghelp")

#ifdef MY_DLL_TESTH_EXPORTS
    #define MY_DLL_TESTH_API __declspec(dllexport)
#else
    #define MY_DLL_TESTH_API __declspec(dllimport)
#endif

typedef FARPROC (WINAPI* HookTargetFunc)();
MY_DLL_TESTH_API HANDLE WINAPI OriginalCreateFileWFunc(LPCWSTR filename, DWORD dwAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpsecurity, DWORD dwCreatePosition, DWORD dwFlag, HANDLE hTemplateFile);
MY_DLL_TESTH_API HANDLE WINAPI OriginalCreateFileAFunc(LPCSTR filename, DWORD dwAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpsecurity, DWORD dwCreatePosition, DWORD dwFlag, HANDLE hTemplateFile);
MY_DLL_TESTH_API BOOL WINAPI OriginalWriteFileFunc(HANDLE hFile, LPCVOID lpBuffer, DWORD nNumberOfBytesToWrite, LPDWORD lpNumberOfBytesWritten, LPOVERLAPPED lpOverlapped);
MY_DLL_TESTH_API BOOL WINAPI OriginalReadFileFunc(HANDLE hFile, LPCVOID lpBuffer, DWORD nNumberOfBytesToWrite, LPDWORD lpNumberOfBytesWritten, LPOVERLAPPED lpOverlapped);
MY_DLL_TESTH_API BOOL WINAPI OriginalDeleteFileWFunc(LPCWSTR lpFileName);
MY_DLL_TESTH_API BOOL WINAPI OriginalDeleteFileAFunc(LPCSTR lpFileName);
MY_DLL_TESTH_API BOOL WINAPI OriginalCreateDirectoryWFunc(LPCWSTR lpPathName, LPSECURITY_ATTRIBUTES lpSecurityAttributes);
MY_DLL_TESTH_API BOOL WINAPI OriginalCreateDirectoryAFunc(LPCSTR lpPathName, LPSECURITY_ATTRIBUTES lpSecurityAttributes);
MY_DLL_TESTH_API BOOL WINAPI OriginalCreateProcessWFunc(LPCWSTR lpApplicationName, LPWSTR lpCommandLine, LPSECURITY_ATTRIBUTES lpProcessAttributes, LPSECURITY_ATTRIBUTES lpThreadAttributes, BOOL bInheritHandles, DWORD dwCreationFlags, LPVOID lpEnvironment, LPCWSTR lpCurrentDirectory, LPSTARTUPINFOW lpStartupInfo, LPPROCESS_INFORMATION lpProcessInformation);
MY_DLL_TESTH_API BOOL WINAPI OriginalCreateProcessAFunc(LPCSTR lpApplicationName, LPSTR lpCommandLine, LPSECURITY_ATTRIBUTES lpProcessAttributes, LPSECURITY_ATTRIBUTES lpThreadAttributes, BOOL bInheritHandles, DWORD dwCreationFlags, LPVOID lpEnvironment, LPCSTR lpCurrentDirectory, LPSTARTUPINFOA lpStartupInfo, LPPROCESS_INFORMATION lpProcessInformation);
MY_DLL_TESTH_API BOOL WINAPI OriginalCopyFileAFunc(LPCSTR lpExistingFileName, LPCSTR lpNewFileName, BOOL bFailIfExists);
MY_DLL_TESTH_API BOOL WINAPI OriginalCopyFileWFunc(LPCWSTR lpExistingFileName, LPCWSTR lpNewFileName, BOOL bFailIfExists);
MY_DLL_TESTH_API FARPROC WINAPI OriginalGetProcAddressFunc(HMODULE hModule, LPCSTR lpProcName);
MY_DLL_TESTH_API FARPROC WINAPI OriginalGetProcAddressFunc(HMODULE hModule, LPCSTR lpProcName);


#endif