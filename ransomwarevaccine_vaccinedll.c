#define MY_DLL_TESTH_EXPORTS
#include "ransomwarevaccine_vaccinedll.h"
#include <stdio.h>
#include <windows.h>
#include <time.h>
#include <string.h>
#include <Dbghelp.h>
#include <winternl.h>
#include <Psapi.h>
#pragma comment(lib, "Dbghelp")

typedef FARPROC (WINAPI* HookTargetFunc)();

typedef HANDLE (WINAPI* OriginalCreateFileW)(LPCWSTR, DWORD, DWORD, LPSECURITY_ATTRIBUTES, DWORD, DWORD, HANDLE);

FARPROC originalcreatefilewvar;

typedef HANDLE (WINAPI* OriginalCreateFileA)(LPCSTR, DWORD, DWORD, LPSECURITY_ATTRIBUTES, DWORD, DWORD, HANDLE);

FARPROC originalcreatefileavar;

typedef BOOL (WINAPI* OriginalWriteFile)(HANDLE, LPCVOID, DWORD, LPDWORD, LPOVERLAPPED);

FARPROC originalwritefilevar;

typedef BOOL (WINAPI* OriginalReadFile)(HANDLE, LPCVOID, DWORD, LPDWORD, LPOVERLAPPED);

FARPROC originalreadfilevar;

typedef BOOL (WINAPI* OriginalDeleteFileW)(LPCWSTR);

FARPROC originaldeletefilewvar;

typedef BOOL (WINAPI* OriginalDeleteFileA)(LPCSTR);

FARPROC originaldeletefileavar;

typedef BOOL (WINAPI* OriginalCreateDirectoryW)(LPCWSTR, LPSECURITY_ATTRIBUTES);

FARPROC originalcreatedirectorywvar;

typedef BOOL (WINAPI* OriginalCreateDirectoryA)(LPCSTR, LPSECURITY_ATTRIBUTES);

FARPROC originalcreatedirectoryavar;

typedef BOOL (WINAPI* OriginalCopyFileA)(LPCSTR, LPCSTR, BOOL);

FARPROC originalcopyfileavar;

typedef BOOL (WINAPI* OriginalCopyFileW)(LPCWSTR, LPCWSTR, BOOL);

FARPROC originalcopyfilewvar;

typedef BOOL (WINAPI* OriginalMoveFileA)(LPCSTR, LPCSTR);

FARPROC originalmovefileavar;

typedef BOOL (WINAPI* OriginalMoveFileW)(LPCWSTR, LPCWSTR);

FARPROC originalmovefilewvar;

typedef BOOL (WINAPI* OriginalCreateProcessW)(LPCWSTR, LPWSTR, LPSECURITY_ATTRIBUTES, LPSECURITY_ATTRIBUTES, BOOL, DWORD, LPVOID, LPCWSTR, LPSTARTUPINFOW, LPPROCESS_INFORMATION);

FARPROC originalcreateprocesswvar;

typedef BOOL (WINAPI* OriginalCreateProcessA)(LPCSTR, LPSTR, LPSECURITY_ATTRIBUTES, LPSECURITY_ATTRIBUTES, BOOL, DWORD, LPVOID, LPCSTR, LPSTARTUPINFOA, LPPROCESS_INFORMATION);

FARPROC originalcreateprocessavar;

typedef HANDLE (WINAPI* OriginalOpenProcess)(DWORD, BOOL, DWORD);

FARPROC originalopenprocessvar;

typedef FARPROC (WINAPI* OriginalGetProcAddress)(HANDLE, LPCSTR);

FARPROC originalvirtualallocvar;

typedef FARPROC (WINAPI* OriginalVirtualAlloc)(LPVOID, SIZE_T, DWORD, DWORD);

FARPROC originalvirtualallocexvar;

typedef FARPROC (WINAPI* OriginalVirtualAllocEx)(HANDLE, LPVOID, SIZE_T, DWORD, DWORD);

FARPROC originalgetproccaddressvar;

LPTHREAD_START_ROUTINE LoadLibAddr;

// ログ書き込み関数の前方宣言
void write_log(const char* funcname, const char* targetfile);

// GetProcAddress フック用: フック対象関数名 → 自DLL内エクスポート名 の対応テーブル
char funcvardir[256][256] = {"CreateFileW", "CreateFileA", "WriteFile", "ReadFile", "DeleteFileW", "DeleteFileA", "CreateDirectoryW", "CreateDirectoryA", "CreateProcessW", "CreateProcessA"};
char originalfuncvardir[256][256] = {"OriginalCreateFileWFunc", "OriginalCreateFileAFunc", "OriginalWriteFileFunc", "OriginalReadFileFunc", "OriginalDeleteFileWFunc", "OriginalDeleteFileAFunc", "OriginalCreateDirectoryWFunc", "OriginalCreateDirectoryAFunc", "OriginalCreateProcessWFunc", "OriginalCreateProcessAFunc"};

char moduledlldir[256][256] = {"ucrtbase.dll", "msvcrt.dll"};

char charNoticeMode[256];
char charLogMode[256];
char charProtectMode[256];
char dll_path[MAX_PATH];
char log_file_path[MAX_PATH];

DWORD dwNoticeMode;  // MessageBox = 1; CUI = 2; none = 0
BOOL  bLogMode;      // logTrue = TRUE; logFalse = FALSE
DWORD dwProtectMode; // AB = 1; WDB = 2; AN = 3

// =============================================================================
// Hook functions
// 各関数は dwProtectMode に従ってブロックするか通過させるかを決定する
// AB  (dwProtectMode == 1): All Block
// WDB (dwProtectMode == 2): Write/Delete Block
// AN  (dwProtectMode == 3): All No block (通知のみ・全て通過)
// =============================================================================

HANDLE WINAPI OriginalCreateFileWFunc(LPCWSTR filename, DWORD dwAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpsecurity, DWORD dwCreatePosition, DWORD dwFlag, HANDLE hTemplateFile){
    if (dwNoticeMode == 1) MessageBoxW(NULL, filename, L"vaccinedll-createfilew", MB_OK);
    else if (dwNoticeMode == 2) wprintf(L"\n\x1b[36mCreateFile for \x1b[39m%ls\n", filename);
    
    // ログに記録（ファイルアクセス関数のみ）
    if (bLogMode){
        char filenameA[512];
        WideCharToMultiByte(CP_ACP, 0, filename, -1, filenameA, sizeof(filenameA), NULL, NULL);
        write_log("CreateFileW", filenameA);
    }
    
    // AB のみブロック / WDB・AN は通過
    if (dwProtectMode == 1) return INVALID_HANDLE_VALUE;
    OriginalCreateFileW original = (OriginalCreateFileW)originalcreatefilewvar;
    return original(filename, dwAccess, dwShareMode, lpsecurity, dwCreatePosition, dwFlag, hTemplateFile);
}

HANDLE WINAPI OriginalCreateFileAFunc(LPCSTR filename, DWORD dwAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpsecurity, DWORD dwCreatePosition, DWORD dwFlag, HANDLE hTemplateFile){
    if (dwNoticeMode == 1) MessageBoxA(NULL, filename, "vaccinedll-createfilea", MB_OK);
    else if (dwNoticeMode == 2) printf("\n\x1b[36mCreateFile for \x1b[39m%s\n", filename);
    
    // ログに記録（ファイルアクセス関数のみ）
    if (bLogMode){
        write_log("CreateFileA", filename);
    }
    
    // AB のみブロック / WDB・AN は通過
    if (dwProtectMode == 1) return INVALID_HANDLE_VALUE;
    OriginalCreateFileA original = (OriginalCreateFileA)originalcreatefileavar;
    return original(filename, dwAccess, dwShareMode, lpsecurity, dwCreatePosition, dwFlag, hTemplateFile);
}

BOOL WINAPI OriginalWriteFileFunc(HANDLE hFile, LPCVOID lpBuffer, DWORD nNumberOfBytesToWrite, LPDWORD lpNumberOfBytesWritten, LPOVERLAPPED lpOverlapped){
    wchar_t filename[512];
    if (hFile == NULL) MessageBoxA(NULL, "hFile is NULL", "vaccinedll-ERROR", MB_ICONERROR);
    // コンソール書き込み (FILE_TYPE_CHAR) は常に通過させる (printf等を機能させるため)
    if (GetFileType(hFile) == 2) {
        OriginalWriteFile original = (OriginalWriteFile)originalwritefilevar;
        return original(hFile, lpBuffer, nNumberOfBytesToWrite, lpNumberOfBytesWritten, lpOverlapped);
    }
    if (GetFinalPathNameByHandleW(hFile, filename, 1025, FILE_NAME_NORMALIZED | VOLUME_NAME_DOS) == 0) MessageBoxA(NULL, "writefile handle error", "vaccinedll-ERROR", MB_ICONERROR);
    
    // ログファイルへの書き込みの場合、ブロックしない
    if (bLogMode){
        wchar_t logFilePathW[MAX_PATH];
        MultiByteToWideChar(CP_ACP, 0, log_file_path, -1, logFilePathW, MAX_PATH);
        if (wcscmp(filename, logFilePathW) == 0){
            // ログファイルへの書き込みなので通す
            OriginalWriteFile original = (OriginalWriteFile)originalwritefilevar;
            return original(hFile, lpBuffer, nNumberOfBytesToWrite, lpNumberOfBytesWritten, lpOverlapped);
        }
    }
    
    if (dwNoticeMode == 1) MessageBoxW(NULL, filename, L"vaccinedll-writefile", MB_OK);
    else if (dwNoticeMode == 2) wprintf(L"\n\x1b[33mWriteFile for \x1b[39m%ls\n", filename);
    
    // ログに記録（ファイルアクセス関数のみ）
    if (bLogMode){
        char filenameA[512];
        WideCharToMultiByte(CP_ACP, 0, filename, -1, filenameA, sizeof(filenameA), NULL, NULL);
        write_log("WriteFile", filenameA);
    }
    
    // AB・WDB はブロック / AN は通過
    if (dwProtectMode == 1 || dwProtectMode == 2) return FALSE;
    OriginalWriteFile original = (OriginalWriteFile)originalwritefilevar;
    return original(hFile, lpBuffer, nNumberOfBytesToWrite, lpNumberOfBytesWritten, lpOverlapped);
}

BOOL WINAPI OriginalReadFileFunc(HANDLE hFile, LPCVOID lpBuffer, DWORD nNumberOfBytesToWrite, LPDWORD lpNumberOfBytesWritten, LPOVERLAPPED lpOverlapped){
    wchar_t filename[512];
    if (GetFinalPathNameByHandleW(hFile, filename, 1025, FILE_NAME_NORMALIZED | VOLUME_NAME_DOS) == 0) MessageBoxA(NULL, "readfile handle error", "vaccinedll-ERROR", MB_ICONERROR);
    if (dwNoticeMode == 1) MessageBoxW(NULL, filename, L"vaccinedll-readfile", MB_OK);
    else if (dwNoticeMode == 2) wprintf(L"\n\x1b[32mReadFile for \x1b[39m%ls\n", filename);
    
    // ログに記録（ファイルアクセス関数のみ）
    if (bLogMode){
        char filenameA[512];
        WideCharToMultiByte(CP_ACP, 0, filename, -1, filenameA, sizeof(filenameA), NULL, NULL);
        write_log("ReadFile", filenameA);
    }
    
    // AB のみブロック / WDB・AN は通過
    if (dwProtectMode == 1) return FALSE;
    OriginalReadFile original = (OriginalReadFile)originalreadfilevar;
    return original(hFile, lpBuffer, nNumberOfBytesToWrite, lpNumberOfBytesWritten, lpOverlapped);
}

BOOL WINAPI OriginalDeleteFileWFunc(LPCWSTR lpFileName){
    if (dwNoticeMode == 1) MessageBoxW(NULL, lpFileName, L"vaccinedll-deletefilew", MB_OK);
    else if (dwNoticeMode == 2) wprintf(L"\n\x1b[31mDeleteFile for \x1b[39m%ls\n", lpFileName);
    
    // ログに記録（ファイルアクセス関数のみ）
    if (bLogMode){
        char filenameA[512];
        WideCharToMultiByte(CP_ACP, 0, lpFileName, -1, filenameA, sizeof(filenameA), NULL, NULL);
        write_log("DeleteFileW", filenameA);
    }
    
    // AB・WDB はブロック / AN は通過
    if (dwProtectMode == 1 || dwProtectMode == 2) return FALSE;
    OriginalDeleteFileW original = (OriginalDeleteFileW)originaldeletefilewvar;
    return original(lpFileName);
}

BOOL WINAPI OriginalDeleteFileAFunc(LPCSTR lpFileName){
    if (dwNoticeMode == 1) MessageBoxA(NULL, lpFileName, "vaccinedll-deletefilea", MB_OK);
    else if (dwNoticeMode == 2) printf("\n\x1b[31mDeleteFile for \x1b[39m%s\n", lpFileName);
    
    // ログに記録（ファイルアクセス関数のみ）
    if (bLogMode){
        write_log("DeleteFileA", lpFileName);
    }
    
    // AB・WDB はブロック / AN は通過
    if (dwProtectMode == 1 || dwProtectMode == 2) return FALSE;
    OriginalDeleteFileA original = (OriginalDeleteFileA)originaldeletefileavar;
    return original(lpFileName);
}

BOOL WINAPI OriginalCreateDirectoryWFunc(LPCWSTR lpPathName, LPSECURITY_ATTRIBUTES lpSecurityAttributes){
    if (dwNoticeMode == 1) MessageBoxW(NULL, lpPathName, L"vaccinedll-CreateDirectoryW", MB_OK);
    else if (dwNoticeMode == 2) wprintf(L"\n\x1b[35mCreateDirectory for \x1b[39m%ls\n", lpPathName);
    // 全モードで通過
    OriginalCreateDirectoryW original = (OriginalCreateDirectoryW)originalcreatedirectorywvar;
    return original(lpPathName, lpSecurityAttributes);
}

BOOL WINAPI OriginalCreateDirectoryAFunc(LPCSTR lpPathName, LPSECURITY_ATTRIBUTES lpSecurityAttributes){
    if (dwNoticeMode == 1) MessageBoxA(NULL, lpPathName, "vaccinedll-CreateDirectoryA", MB_OK);
    else if (dwNoticeMode == 2) printf("\n\x1b[35mCreateDirectory for \x1b[39m%s\n", lpPathName);
    // 全モードで通過
    OriginalCreateDirectoryA original = (OriginalCreateDirectoryA)originalcreatedirectoryavar;
    return original(lpPathName, lpSecurityAttributes);
}

BOOL WINAPI OriginalCopyFileAFunc(LPCSTR lpExistingFileName, LPCSTR lpNewFileName, BOOL bFailIfExists){
    if (dwNoticeMode == 1) MessageBoxA(NULL, lpExistingFileName, "vaccinedll-CopyFileA", MB_OK);
    else if (dwNoticeMode == 2) printf("\n\x1b[36mCopyFile for \x1b[39m%s\n", lpExistingFileName);
    
    // ログに記録（ファイルアクセス関数のみ）
    if (bLogMode){
        write_log("CopyFileA", lpExistingFileName);
    }
    
    // AB・WDB はブロック / AN は通過
    if (dwProtectMode == 1 || dwProtectMode == 2) return FALSE;
    OriginalCopyFileA original = (OriginalCopyFileA)originalcopyfileavar;
    return original(lpExistingFileName, lpNewFileName, bFailIfExists);
}

BOOL WINAPI OriginalCopyFileWFunc(LPCWSTR lpExistingFileName, LPCWSTR lpNewFileName, BOOL bFailIfExists){
    if (dwNoticeMode == 1) MessageBoxW(NULL, lpExistingFileName, L"vaccinedll-CopyFileW", MB_OK);
    else if (dwNoticeMode == 2) wprintf(L"\n\x1b[36mCopyFile for \x1b[39m%ls\n", lpExistingFileName);
    
    // ログに記録（ファイルアクセス関数のみ）
    if (bLogMode){
        char filenameA[512];
        WideCharToMultiByte(CP_ACP, 0, lpExistingFileName, -1, filenameA, sizeof(filenameA), NULL, NULL);
        write_log("CopyFileW", filenameA);
    }
    
    // AB・WDB はブロック / AN は通過
    if (dwProtectMode == 1 || dwProtectMode == 2) return FALSE;
    OriginalCopyFileW original = (OriginalCopyFileW)originalcopyfilewvar;
    return original(lpExistingFileName, lpNewFileName, bFailIfExists);
}

BOOL WINAPI OriginalMoveFileAFunc(LPCSTR lpExistingFileName, LPCSTR lpNewFileName){
    if (dwNoticeMode == 1) MessageBoxA(NULL, lpExistingFileName, "vaccinedll-MoveFileA", MB_OK);
    else if (dwNoticeMode == 2) printf("\n\x1b[36mMoveFile for \x1b[39m%s\n", lpExistingFileName);
    
    // ログに記録（ファイルアクセス関数のみ）
    if (bLogMode){
        write_log("MoveFileA", lpExistingFileName);
    }
    
    // AB・WDB はブロック / AN は通過
    if (dwProtectMode == 1 || dwProtectMode == 2) return FALSE;
    OriginalMoveFileA original = (OriginalMoveFileA)originalmovefileavar;
    return original(lpExistingFileName, lpNewFileName);
}

BOOL WINAPI OriginalMoveFileWFunc(LPCWSTR lpExistingFileName, LPCWSTR lpNewFileName){
    if (dwNoticeMode == 1) MessageBoxW(NULL, lpExistingFileName, L"vaccinedll-MoveFileW", MB_OK);
    else if (dwNoticeMode == 2) wprintf(L"\n\x1b[36mMoveFile for \x1b[39m%ls\n", lpExistingFileName);
    
    // ログに記録（ファイルアクセス関数のみ）
    if (bLogMode){
        char filenameA[512];
        WideCharToMultiByte(CP_ACP, 0, lpExistingFileName, -1, filenameA, sizeof(filenameA), NULL, NULL);
        write_log("MoveFileW", filenameA);
    }
    
    // AB・WDB はブロック / AN は通過
    if (dwProtectMode == 1 || dwProtectMode == 2) return FALSE;
    OriginalMoveFileW original = (OriginalMoveFileW)originalmovefilewvar;
    return original(lpExistingFileName, lpNewFileName);
}

BOOL WINAPI OriginalCreateProcessWFunc(LPCWSTR lpApplicationName, LPWSTR lpCommandLine, LPSECURITY_ATTRIBUTES lpProcessAttributes, LPSECURITY_ATTRIBUTES lpThreadAttributes, BOOL bInheritHandles, DWORD dwCreationFlags, LPVOID lpEnvironment, LPCWSTR lpCurrentDirectory, LPSTARTUPINFOW lpStartupInfo, LPPROCESS_INFORMATION lpProcessInformation){
    MessageBoxA(NULL, "CreateProcess", "", MB_OK);
    OriginalCreateProcessW original = (OriginalCreateProcessW)originalcreateprocesswvar;
    BOOL returnvar = original(lpApplicationName, lpCommandLine, lpProcessAttributes, lpThreadAttributes, bInheritHandles, dwCreationFlags | CREATE_SUSPENDED, lpEnvironment, lpCurrentDirectory, lpStartupInfo, lpProcessInformation);
    if ((*lpProcessInformation).hProcess == NULL) {
        MessageBoxA(NULL, "not found process", "ERROR MESSAGE", MB_OK);
        if (!TerminateProcess((*lpProcessInformation).hProcess, 1)){
            MessageBoxA(NULL, "Exit Process Error", "Error", MB_ICONERROR);
            ExitProcess(1);
            return FALSE;
        }
        return FALSE;
    }
    LPVOID lpvoidbuffer = VirtualAllocEx((*lpProcessInformation).hProcess, NULL, 2048, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (lpvoidbuffer == NULL) {
        MessageBoxA(NULL, "VirtualAllocEx Error", "ERROR MESSAGE", MB_OK);
        if (!TerminateProcess((*lpProcessInformation).hProcess, 1)){
            MessageBoxA(NULL, "Exit Process Error", "Error", MB_ICONERROR);
            ExitProcess(1);
            return FALSE;
        }
        return FALSE;
    }
    if (WriteProcessMemory((*lpProcessInformation).hProcess, lpvoidbuffer, (LPCVOID)dll_path, strlen(dll_path)+1, NULL) == 0) {
        MessageBoxA(NULL, "write memory failure", "ERROR MESSAGE", MB_OK);
        if (!TerminateProcess((*lpProcessInformation).hProcess, 1)){
            MessageBoxA(NULL, "Exit Process Error", "Error", MB_ICONERROR);
            ExitProcess(1);
            return FALSE;
        }
        return FALSE;
    }
    HANDLE remotethreadhandle = CreateRemoteThread((*lpProcessInformation).hProcess, NULL, 0, LoadLibAddr, lpvoidbuffer, 0, NULL);
    if (remotethreadhandle == NULL) {
        MessageBoxA(NULL, "create thread failure", "ERROR MESSAGE", MB_OK);
        if (!TerminateProcess((*lpProcessInformation).hProcess, 1)){
            MessageBoxA(NULL, "Exit Process Error", "Error", MB_ICONERROR);
            ExitProcess(1);
            return FALSE;
        }
        return FALSE;
    }
    else{
        WaitForSingleObject(remotethreadhandle, INFINITE);
        CloseHandle(remotethreadhandle);
    }
    ResumeThread((*lpProcessInformation).hThread);
    WaitForSingleObject((*lpProcessInformation).hProcess, INFINITE);
    return returnvar;
}

BOOL WINAPI OriginalCreateProcessAFunc(LPCSTR lpApplicationName, LPSTR lpCommandLine, LPSECURITY_ATTRIBUTES lpProcessAttributes, LPSECURITY_ATTRIBUTES lpThreadAttributes, BOOL bInheritHandles, DWORD dwCreationFlags, LPVOID lpEnvironment, LPCSTR lpCurrentDirectory, LPSTARTUPINFOA lpStartupInfo, LPPROCESS_INFORMATION lpProcessInformation){
    MessageBoxA(NULL, "CreateProcess", "", MB_OK);
    OriginalCreateProcessA original = (OriginalCreateProcessA)originalcreateprocessavar;
    BOOL returnvar = original(lpApplicationName, lpCommandLine, lpProcessAttributes, lpThreadAttributes, bInheritHandles, dwCreationFlags | CREATE_SUSPENDED, lpEnvironment, lpCurrentDirectory, lpStartupInfo, lpProcessInformation);
    if ((*lpProcessInformation).hProcess == NULL) {
        MessageBoxA(NULL, "not found process", "ERROR MESSAGE", MB_OK);
        if (!TerminateProcess((*lpProcessInformation).hProcess, 1)){
            MessageBoxA(NULL, "Exit Process Error", "Error", MB_ICONERROR);
            ExitProcess(1);
            return FALSE;
        }
        return FALSE;
    }
    LPVOID lpvoidbuffer = VirtualAllocEx((*lpProcessInformation).hProcess, NULL, 2048, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (lpvoidbuffer == NULL) {
        MessageBoxA(NULL, "VirtualAllocEx Error", "ERROR MESSAGE", MB_OK);
        if (!TerminateProcess((*lpProcessInformation).hProcess, 1)){
            MessageBoxA(NULL, "Exit Process Error", "Error", MB_ICONERROR);
            ExitProcess(1);
            return FALSE;
        }
        return FALSE;
    }
    if (WriteProcessMemory((*lpProcessInformation).hProcess, lpvoidbuffer, (LPCVOID)dll_path, strlen(dll_path)+1, NULL) == 0) {
        MessageBoxA(NULL, "write memory failure", "ERROR MESSAGE", MB_OK);
        if (!TerminateProcess((*lpProcessInformation).hProcess, 1)){
            MessageBoxA(NULL, "Exit Process Error", "Error", MB_ICONERROR);
            ExitProcess(1);
            return FALSE;
        }
        return FALSE;
    }
    HANDLE remotethreadhandle = CreateRemoteThread((*lpProcessInformation).hProcess, NULL, 0, LoadLibAddr, lpvoidbuffer, 0, NULL);
    if (remotethreadhandle == NULL) {
        MessageBoxA(NULL, "create thread failure", "ERROR MESSAGE", MB_OK);
        if (!TerminateProcess((*lpProcessInformation).hProcess, 1)){
            MessageBoxA(NULL, "Exit Process Error", "Error", MB_ICONERROR);
            ExitProcess(1);
            return FALSE;
        }
        return FALSE;
    }
    else{
        WaitForSingleObject(remotethreadhandle, INFINITE);
        CloseHandle(remotethreadhandle);
    }
    ResumeThread((*lpProcessInformation).hThread);
    WaitForSingleObject((*lpProcessInformation).hProcess, INFINITE);
    return returnvar;
}

// ランサムウェアが IAT を迂回して GetProcAddress で直接 Win32 API を取得しようとする手口を封鎖
// フック対象関数名を要求された場合、自DLL内のブロック関数アドレスを返す
FARPROC WINAPI OriginalGetProcAddressFunc(HMODULE hModule, LPCSTR lpProcName){
    OriginalGetProcAddress original = (OriginalGetProcAddress)originalgetproccaddressvar;
    char funcname[256];
    strcpy(funcname, lpProcName);
    for (int i = 0; i < 10; i++){
        if (!strcmp(lpProcName, funcvardir[i])){
            MessageBoxA(NULL, "GetProcAddress", funcvardir[i], MB_OK);
            strcpy(funcname, originalfuncvardir[i]);
            hModule = GetModuleHandleA("ransomwarevaccine_vaccinedll.dll");
        }
    }
    FARPROC returnvar = original(hModule, funcname);
    char errorfuncname[256];
    sprintf(errorfuncname, "GetProcAddress error, Call Func:%s", lpProcName);
    if (returnvar == NULL) MessageBoxA(NULL, errorfuncname, "Error", MB_OK);
    return returnvar;
}

// VirtualAlloc フック: AB モードのみブロック
LPVOID WINAPI OriginalVirtualAllocFunc(LPVOID lpAddress, SIZE_T dwSize, DWORD flAllocationType, DWORD flProtect){
    OriginalVirtualAlloc original = (OriginalVirtualAlloc)originalvirtualallocvar;
    char addresschar[256];
    sprintf(addresschar, "VirutalAlloc address:0x%p, size:%llu, flAllocationType:0x%x, flProtect:0x%x", lpAddress, (unsigned long long)dwSize, (unsigned int)flAllocationType, (unsigned int)flProtect);
    if (dwNoticeMode == 1) MessageBoxA(NULL, addresschar, "vaccinedll-virtualallocfunc", MB_OK);
    else if (dwNoticeMode == 2) printf("\n%s\n", addresschar);
    // AB のみブロック / WDB・AN は通過
    if (dwProtectMode == 1) return NULL;
    return original(lpAddress, dwSize, flAllocationType, flProtect);
}

LPVOID WINAPI OriginalVirtualAllocExFunc(HANDLE hProcess, LPVOID lpAddress, SIZE_T dwSize, DWORD flAllocationType, DWORD flProtect){
    OriginalVirtualAllocEx original = (OriginalVirtualAllocEx)originalvirtualallocexvar;
    wchar_t addresschar[512];
    wchar_t ProcessPath[256];
    GetFinalPathNameByHandleW(hProcess, ProcessPath, 256, 0);
    swprintf(addresschar, 512, L"VirutalAllocEx TargetProcessPath:%s, address:0x%p, size:%llu, flAllocationType:0x%x, flProtect:0x%x", ProcessPath, lpAddress, (unsigned long long)dwSize, (unsigned int)flAllocationType, (unsigned int)flProtect);
    if (dwNoticeMode == 1) MessageBoxW(NULL, addresschar, L"vaccinedll-virtualallocexfunc", MB_OK);
    else if (dwNoticeMode == 2) wprintf(L"\n%ls\n", addresschar);
    // AB のみブロック / WDB・AN は通過
    if (dwProtectMode == 1) return NULL;
    return original(hProcess, lpAddress, dwSize, flAllocationType, flProtect);
}

int iathook(LPCSTR modulename, LPCSTR dllname, LPCSTR funcname, HookTargetFunc hooktargetfunc, FARPROC* pOriginalOut){
    TCHAR szMypath[MAX_PATH];
    ULONG cbsize = 0;

    GetModuleFileNameA(NULL, szMypath, _countof(szMypath));
    HANDLE hModule = GetModuleHandleA(modulename);
    if (modulename == NULL) hModule = GetModuleHandleA((LPCSTR)szMypath);

    FARPROC originaldebug = GetProcAddress(GetModuleHandleA(dllname), funcname);
    *pOriginalOut = originaldebug;
    if (originaldebug == NULL) return 0;

    PIMAGE_IMPORT_DESCRIPTOR pimage_import_descriptor = (PIMAGE_IMPORT_DESCRIPTOR)ImageDirectoryEntryToData(hModule, TRUE, IMAGE_DIRECTORY_ENTRY_IMPORT, &cbsize);

    for(; pimage_import_descriptor->Name; pimage_import_descriptor++){

        LPCSTR pModuleName = (LPCSTR)((PBYTE)hModule + pimage_import_descriptor->Name);
        PIMAGE_THUNK_DATA pFirstThunk = (PIMAGE_THUNK_DATA)((PBYTE)hModule + pimage_import_descriptor->FirstThunk);
        PIMAGE_THUNK_DATA pOriginalFirstThunk = (PIMAGE_THUNK_DATA)((PBYTE)hModule + pimage_import_descriptor->OriginalFirstThunk);

        for (; pFirstThunk->u1.Function; pFirstThunk++, pOriginalFirstThunk++){
            FARPROC pfnImportedFunc = (FARPROC)(pFirstThunk->u1.Function);
            PIMAGE_IMPORT_BY_NAME pimage_import_by_name = (PIMAGE_IMPORT_BY_NAME)((PBYTE)hModule + pOriginalFirstThunk->u1.AddressOfData);

            if (pfnImportedFunc == (FARPROC)originaldebug){

                MEMORY_BASIC_INFORMATION mbi;
                DWORD dwJunk;
                VirtualQuery(pFirstThunk, &mbi, sizeof(MEMORY_BASIC_INFORMATION));

                if(!VirtualProtect(mbi.BaseAddress, mbi.RegionSize, PAGE_EXECUTE_READWRITE, &mbi.Protect)){
                    return 0;
                }
                pFirstThunk->u1.Function = (ULONGLONG)(DWORD_PTR)hooktargetfunc;
                if(VirtualProtect(mbi.BaseAddress, mbi.RegionSize, mbi.Protect, &dwJunk)) return 1;
            }
        }
    }
    return 0;
}

void write_log(const char* funcname, const char* targetfile){
    if (!bLogMode) return;
    
    // ログファイルを追記モードで開く
    HANDLE hLogFile = CreateFileA(log_file_path, FILE_APPEND_DATA, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hLogFile == INVALID_HANDLE_VALUE){
        MessageBoxA(NULL, "Failed to open log file", "vaccinedll-ERROR", MB_ICONERROR);
        return;
    }
    
    // タイムスタンプを取得
    time_t now = time(NULL);
    struct tm *timeinfo = localtime(&now);
    char timestamp[64];
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", timeinfo);
    
    // ログ文字列を構築
    char logBuffer[1024];
    sprintf(logBuffer, "[%s] Function: %s, Target: %s\r\n", timestamp, funcname, targetfile);
    
    // ログを書き込み（WriteFileの標準関数を使用）
    DWORD bytesWritten;
    OriginalWriteFile original = (OriginalWriteFile)originalwritefilevar;
    original(hLogFile, logBuffer, strlen(logBuffer), &bytesWritten, NULL);
    
    // ハンドルを閉じる
    CloseHandle(hLogFile);
}

int allhook(){
    LoadLibAddr = (LPTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandleA("kernel32.dll"), "LoadLibraryA");
    GetEnvironmentVariableA("ransomwarevaccine_dll_path", dll_path, MAX_PATH);

    for (int i = 0; i < 2; i++){
        iathook(moduledlldir[i], "Kernelbase.dll", "CreateFileW",      (HookTargetFunc)OriginalCreateFileWFunc,      &originalcreatefilewvar);
        iathook(moduledlldir[i], "Kernelbase.dll", "CreateFileA",      (HookTargetFunc)OriginalCreateFileAFunc,      &originalcreatefileavar);
        iathook(moduledlldir[i], "Kernelbase.dll", "WriteFile",        (HookTargetFunc)OriginalWriteFileFunc,        &originalwritefilevar);
        iathook(moduledlldir[i], "Kernelbase.dll", "ReadFile",         (HookTargetFunc)OriginalReadFileFunc,         &originalreadfilevar);
        iathook(moduledlldir[i], "Kernelbase.dll", "DeleteFileW",      (HookTargetFunc)OriginalDeleteFileWFunc,      &originaldeletefilewvar);
        iathook(moduledlldir[i], "Kernelbase.dll", "DeleteFileA",      (HookTargetFunc)OriginalDeleteFileAFunc,      &originaldeletefileavar);
        iathook(moduledlldir[i], "Kernelbase.dll", "CreateDirectoryW", (HookTargetFunc)OriginalCreateDirectoryWFunc, &originalcreatedirectorywvar);
        iathook(moduledlldir[i], "Kernelbase.dll", "CreateDirectoryA", (HookTargetFunc)OriginalCreateDirectoryAFunc, &originalcreatedirectoryavar);
        iathook(NULL,            "Kernel32.dll",   "CopyFileA",        (HookTargetFunc)OriginalCopyFileAFunc,        &originalcopyfileavar);
        iathook(NULL,            "Kernel32.dll",   "CopyFileW",        (HookTargetFunc)OriginalCopyFileWFunc,        &originalcopyfilewvar);
        iathook(NULL,            "Kernel32.dll",   "MoveFileA",        (HookTargetFunc)OriginalMoveFileAFunc,        &originalmovefileavar);
        iathook(NULL,            "Kernel32.dll",   "MoveFileW",        (HookTargetFunc)OriginalMoveFileWFunc,        &originalmovefilewvar);
        iathook(NULL,            "Kernel32.dll",   "CreateProcessW",   (HookTargetFunc)OriginalCreateProcessWFunc,   &originalcreateprocesswvar);
        iathook(NULL,            "Kernel32.dll",   "CreateProcessA",   (HookTargetFunc)OriginalCreateProcessAFunc,   &originalcreateprocessavar);
        iathook(NULL,            "Kernel32.dll",   "GetProcAddress",   (HookTargetFunc)OriginalGetProcAddressFunc,   &originalgetproccaddressvar);
        iathook(NULL,            "Kernel32.dll",   "VirtualAlloc",     (HookTargetFunc)OriginalVirtualAllocFunc,     &originalvirtualallocvar);
    }
    return 1;
}


BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved){
    if (fdwReason == DLL_PROCESS_ATTACH){
        DisableThreadLibraryCalls(hinstDLL);
        MessageBoxA(NULL, "Call ransomwarevaccine_vaccinedll", "vaccinedll-Message", MB_OK);

        // 通知モードの読み込み
        DWORD dwRetN = GetEnvironmentVariableA("ransomwarevaccine_notice_mode", charNoticeMode, sizeof(charNoticeMode));
        if (0 >= dwRetN || dwRetN >= sizeof(charNoticeMode)) MessageBoxA(NULL, "Enviroment Value error (notice_mode)", "vaccinedll-ERROR", MB_ICONERROR);

        // ログモードの読み込み
        DWORD dwRetL = GetEnvironmentVariableA("ransomwarevaccine_log_mode", charLogMode, sizeof(charLogMode));
        if (0 >= dwRetL || dwRetL >= sizeof(charLogMode)) MessageBoxA(NULL, "Enviroment Value error (log_mode)", "vaccinedll-ERROR", MB_ICONERROR);
        
        // ログモードが有効な場合、ログファイルパスを読み込む
        if (!strcmp(charLogMode, "1")){
            DWORD dwRetLogFile = GetEnvironmentVariableA("ransomwarevaccine_log_file", log_file_path, sizeof(log_file_path));
            if (0 >= dwRetLogFile || dwRetLogFile >= sizeof(log_file_path)) {
                MessageBoxA(NULL, "Enviroment Value error (log_file path)", "vaccinedll-ERROR", MB_ICONERROR);
            }
        }

        // 保護モードの読み込み (AB=1 / WDB=2 / AN=3)
        DWORD dwRetP = GetEnvironmentVariableA("ransomwarevaccine_protect_mode", charProtectMode, sizeof(charProtectMode));
        if (0 >= dwRetP || dwRetP >= sizeof(charProtectMode)) MessageBoxA(NULL, "Enviroment Value error (protect_mode)", "vaccinedll-ERROR", MB_ICONERROR);

        if (!strcmp(charNoticeMode, "1")){
            MessageBoxA(NULL, "notice mode is 1 (MessageBox)", "vaccinedll-message", MB_OK);
            dwNoticeMode = 1;
        }else if (!strcmp(charNoticeMode, "2")){
            dwNoticeMode = 2;
        }else dwNoticeMode = 0;
        
        // ログモードの設定
        if (!strcmp(charLogMode, "1")){
            MessageBoxA(NULL, "log mode is 1 (Logging enabled)", "vaccinedll-message", MB_OK);
            bLogMode = TRUE;
        }else{
            bLogMode = FALSE;
        }

        if (!strcmp(charProtectMode, "1")){
            MessageBoxA(NULL, "protect mode is AB (All Block)", "vaccinedll-message", MB_OK);
            dwProtectMode = 1;
        }else if (!strcmp(charProtectMode, "2")){
            MessageBoxA(NULL, "protect mode is WDB (Write/Delete Block)", "vaccinedll-message", MB_OK);
            dwProtectMode = 2;
        }else if (!strcmp(charProtectMode, "3")){
            MessageBoxA(NULL, "protect mode is AN (All No block)", "vaccinedll-message", MB_OK);
            dwProtectMode = 3;
        }else{
            MessageBoxA(NULL, "Enviroment Value error (protect_mode value)", "vaccinedll-ERROR", MB_ICONERROR);
            dwProtectMode = 1; // 不明な値の場合は安全側として AB (All Block) にフォールバック
        }

        allhook();
    }
    return TRUE;
}
