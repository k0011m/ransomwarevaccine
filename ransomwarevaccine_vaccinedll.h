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
MY_DLL_TESTH_API BOOL WINAPI OriginalMoveFileAFunc(LPCSTR lpExistingFileName, LPCSTR lpNewFileName);
MY_DLL_TESTH_API BOOL WINAPI OriginalMoveFileWFunc(LPCWSTR lpExistingFileName, LPCWSTR lpNewFileName);
MY_DLL_TESTH_API HANDLE WINAPI OriginalCreateFile2Func(LPCWSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, DWORD dwCreationDisposition, PVOID pCreateExParams);
MY_DLL_TESTH_API BOOL WINAPI OriginalSetFileInformationByHandleFunc(HANDLE hFile, FILE_INFO_BY_HANDLE_CLASS FileInformationClass, LPVOID lpFileInformation, DWORD dwBufferSize);
MY_DLL_TESTH_API BOOL WINAPI OriginalMoveFileExAFunc(LPCSTR lpExistingFileName, LPCSTR lpNewFileName, DWORD dwFlags);
MY_DLL_TESTH_API BOOL WINAPI OriginalMoveFileExWFunc(LPCWSTR lpExistingFileName, LPCWSTR lpNewFileName, DWORD dwFlags);
MY_DLL_TESTH_API BOOL WINAPI OriginalCopyFileExAFunc(LPCSTR lpExistingFileName, LPCSTR lpNewFileName, LPPROGRESS_ROUTINE lpProgressRoutine, LPVOID lpData, LPBOOL pbCancel, DWORD dwCopyFlags);
MY_DLL_TESTH_API BOOL WINAPI OriginalCopyFileExWFunc(LPCWSTR lpExistingFileName, LPCWSTR lpNewFileName, LPPROGRESS_ROUTINE lpProgressRoutine, LPVOID lpData, LPBOOL pbCancel, DWORD dwCopyFlags);
MY_DLL_TESTH_API BOOL WINAPI OriginalReplaceFileAFunc(LPCSTR lpReplacedFileName, LPCSTR lpReplacementFileName, LPCSTR lpBackupFileName, DWORD dwReplaceFlags, LPVOID lpExclude, LPVOID lpReserved);
MY_DLL_TESTH_API BOOL WINAPI OriginalReplaceFileWFunc(LPCWSTR lpReplacedFileName, LPCWSTR lpReplacementFileName, LPCWSTR lpBackupFileName, DWORD dwReplaceFlags, LPVOID lpExclude, LPVOID lpReserved);
MY_DLL_TESTH_API NTSTATUS WINAPI OriginalNtCreateFileFunc(PHANDLE FileHandle, ACCESS_MASK DesiredAccess, PVOID ObjectAttributes, PVOID IoStatusBlock, PLARGE_INTEGER AllocationSize, ULONG FileAttributes, ULONG ShareAccess, ULONG CreateDisposition, ULONG CreateOptions, PVOID EaBuffer, ULONG EaLength);
MY_DLL_TESTH_API NTSTATUS WINAPI OriginalNtWriteFileFunc(HANDLE FileHandle, HANDLE Event, PVOID ApcRoutine, PVOID ApcContext, PVOID IoStatusBlock, PVOID Buffer, ULONG Length, PLARGE_INTEGER ByteOffset, PULONG Key);
MY_DLL_TESTH_API NTSTATUS WINAPI OriginalNtReadFileFunc(HANDLE FileHandle, HANDLE Event, PVOID ApcRoutine, PVOID ApcContext, PVOID IoStatusBlock, PVOID Buffer, ULONG Length, PLARGE_INTEGER ByteOffset, PULONG Key);
MY_DLL_TESTH_API NTSTATUS WINAPI OriginalNtDeleteFileFunc(PVOID ObjectAttributes);
MY_DLL_TESTH_API NTSTATUS WINAPI OriginalNtSetInformationFileFunc(HANDLE FileHandle, PVOID IoStatusBlock, PVOID FileInformation, ULONG Length, ULONG FileInformationClass);
MY_DLL_TESTH_API FARPROC WINAPI OriginalGetProcAddressFunc(HMODULE hModule, LPCSTR lpProcName);


#endif
