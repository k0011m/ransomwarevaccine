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

#ifndef STATUS_SUCCESS
#define STATUS_SUCCESS ((NTSTATUS)0x00000000L)
#endif

#ifndef STATUS_UNSUCCESSFUL
#define STATUS_UNSUCCESSFUL ((NTSTATUS)0xC0000001L)
#endif

#ifndef FILE_SUPERSEDE
#define FILE_SUPERSEDE 0x00000000
#endif

#ifndef FILE_CREATE
#define FILE_CREATE 0x00000002
#endif

#ifndef FILE_OVERWRITE
#define FILE_OVERWRITE 0x00000004
#endif

#ifndef FILE_OVERWRITE_IF
#define FILE_OVERWRITE_IF 0x00000005
#endif

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

typedef HANDLE (WINAPI* OriginalCreateFile2)(LPCWSTR, DWORD, DWORD, DWORD, PVOID);

FARPROC originalcreatefile2var;

typedef BOOL (WINAPI* OriginalSetFileInformationByHandle)(HANDLE, FILE_INFO_BY_HANDLE_CLASS, LPVOID, DWORD);

FARPROC originalsetfileinformationbyhandlevar;

typedef BOOL (WINAPI* OriginalMoveFileExA)(LPCSTR, LPCSTR, DWORD);

FARPROC originalmovefileexavar;

typedef BOOL (WINAPI* OriginalMoveFileExW)(LPCWSTR, LPCWSTR, DWORD);

FARPROC originalmovefileexwvar;

typedef BOOL (WINAPI* OriginalCopyFileExA)(LPCSTR, LPCSTR, LPPROGRESS_ROUTINE, LPVOID, LPBOOL, DWORD);

FARPROC originalcopyfileexavar;

typedef BOOL (WINAPI* OriginalCopyFileExW)(LPCWSTR, LPCWSTR, LPPROGRESS_ROUTINE, LPVOID, LPBOOL, DWORD);

FARPROC originalcopyfileexwvar;

typedef BOOL (WINAPI* OriginalReplaceFileA)(LPCSTR, LPCSTR, LPCSTR, DWORD, LPVOID, LPVOID);

FARPROC originalreplacefileavar;

typedef BOOL (WINAPI* OriginalReplaceFileW)(LPCWSTR, LPCWSTR, LPCWSTR, DWORD, LPVOID, LPVOID);

FARPROC originalreplacefilewvar;

typedef NTSTATUS (WINAPI* OriginalNtCreateFile)(PHANDLE, ACCESS_MASK, PVOID, PVOID, PLARGE_INTEGER, ULONG, ULONG, ULONG, ULONG, PVOID, ULONG);

FARPROC originalntcreatefilevar;

typedef NTSTATUS (WINAPI* OriginalNtWriteFile)(HANDLE, HANDLE, PVOID, PVOID, PVOID, PVOID, ULONG, PLARGE_INTEGER, PULONG);

FARPROC originalntwritefilevar;

typedef NTSTATUS (WINAPI* OriginalNtReadFile)(HANDLE, HANDLE, PVOID, PVOID, PVOID, PVOID, ULONG, PLARGE_INTEGER, PULONG);

FARPROC originalntreadfilevar;

typedef NTSTATUS (WINAPI* OriginalNtDeleteFile)(PVOID);

FARPROC originalntdeletefilevar;

typedef NTSTATUS (WINAPI* OriginalNtSetInformationFile)(HANDLE, PVOID, PVOID, ULONG, ULONG);

FARPROC originalntsetinformationfilevar;

typedef BOOL (WINAPI* OriginalCreateProcessW)(LPCWSTR, LPWSTR, LPSECURITY_ATTRIBUTES, LPSECURITY_ATTRIBUTES, BOOL, DWORD, LPVOID, LPCWSTR, LPSTARTUPINFOW, LPPROCESS_INFORMATION);

FARPROC originalcreateprocesswvar;

typedef BOOL (WINAPI* OriginalCreateProcessA)(LPCSTR, LPSTR, LPSECURITY_ATTRIBUTES, LPSECURITY_ATTRIBUTES, BOOL, DWORD, LPVOID, LPCSTR, LPSTARTUPINFOA, LPPROCESS_INFORMATION);

FARPROC originalcreateprocessavar;

typedef HANDLE (WINAPI* OriginalOpenProcess)(DWORD, BOOL, DWORD);

FARPROC originalopenprocessvar;

typedef FARPROC (WINAPI* OriginalGetProcAddress)(HANDLE, LPCSTR);

FARPROC originalgetproccaddressvar;

typedef BOOL (WINAPI* OriginalFreeLibrary)(HMODULE);

FARPROC originalfreelibraryvar;

FARPROC originalvirtualallocvar;

typedef FARPROC (WINAPI* OriginalVirtualAlloc)(LPVOID, SIZE_T, DWORD, DWORD);

FARPROC originalvirtualallocexvar;

typedef FARPROC (WINAPI* OriginalVirtualAllocEx)(HANDLE, LPVOID, SIZE_T, DWORD, DWORD);

LPTHREAD_START_ROUTINE LoadLibAddr;

// ログ書き込み関数の前方宣言
void write_log(const char* funcname, const char* targetfile);

#define HONEYPOT_DUMMY_COUNT 10

HANDLE honeypotDummyHandlesA[HONEYPOT_DUMMY_COUNT];
HANDLE honeypotDummyHandlesW[HONEYPOT_DUMMY_COUNT];
DWORD honeypotUseIndex;
DWORD honeypotCreateIndexA;
DWORD honeypotCreateIndexW;

typedef struct _HONEYPOT_IO_STATUS_BLOCK {
    union {
        NTSTATUS Status;
        PVOID Pointer;
    };
    ULONG_PTR Information;
} HONEYPOT_IO_STATUS_BLOCK, *PHONEYPOT_IO_STATUS_BLOCK;

// ブロック時に実ファイルの代わりへ誘導するため、閉じないダミーファイルを作成する
HANDLE create_honeypot_dummy_file_a(void);

// Wide API の呼び出し元にも自然な HANDLE を返すため、W 系の元関数でダミーファイルを作成する
HANDLE create_honeypot_dummy_file_w(void);

// AB/WDB のブロック処理で使い回す HANDLE を準備し、足りなくなった場合は追加作成する
HANDLE acquire_honeypot_handle(void);

// IAT Hook 後に AB/WDB の場合だけ、初期プールとして A/W それぞれ 10 個の HANDLE を保持する
void initialize_honeypot_dummy_files(void);

// Native API の成功偽装時に呼び出し元へ処理済みサイズを返す
void set_honeypot_io_status(PVOID ioStatusBlock, NTSTATUS status, ULONG_PTR information);

// WDB の NtCreateFile で、読み取り以外の open/create を dummy HANDLE に逃がすか判定する
BOOL should_redirect_nt_create_file(ACCESS_MASK desiredAccess, ULONG createDisposition);

// GetProcAddress フック用: フック対象関数名 → 自DLL内エクスポート名 の対応テーブル
#define HOOK_REDIRECT_COUNT 28
char funcvardir[HOOK_REDIRECT_COUNT][256] = {
    "CreateFileW", "CreateFileA", "WriteFile", "ReadFile", "DeleteFileW", "DeleteFileA", "CreateDirectoryW", "CreateDirectoryA", "CreateProcessW", "CreateProcessA",
    "CopyFileA", "CopyFileW", "MoveFileA", "MoveFileW", "FreeLibrary",
    "CreateFile2", "SetFileInformationByHandle", "MoveFileExA", "MoveFileExW", "CopyFileExA", "CopyFileExW", "ReplaceFileA", "ReplaceFileW",
    "NtCreateFile", "NtWriteFile", "NtReadFile", "NtDeleteFile", "NtSetInformationFile"
};
char originalfuncvardir[HOOK_REDIRECT_COUNT][256] = {
    "OriginalCreateFileWFunc", "OriginalCreateFileAFunc", "OriginalWriteFileFunc", "OriginalReadFileFunc", "OriginalDeleteFileWFunc", "OriginalDeleteFileAFunc", "OriginalCreateDirectoryWFunc", "OriginalCreateDirectoryAFunc", "OriginalCreateProcessWFunc", "OriginalCreateProcessAFunc",
    "OriginalCopyFileAFunc", "OriginalCopyFileWFunc", "OriginalMoveFileAFunc", "OriginalMoveFileWFunc", "OriginalFreeLibraryFunc",
    "OriginalCreateFile2Func", "OriginalSetFileInformationByHandleFunc", "OriginalMoveFileExAFunc", "OriginalMoveFileExWFunc", "OriginalCopyFileExAFunc", "OriginalCopyFileExWFunc", "OriginalReplaceFileAFunc", "OriginalReplaceFileWFunc",
    "OriginalNtCreateFileFunc", "OriginalNtWriteFileFunc", "OriginalNtReadFileFunc", "OriginalNtDeleteFileFunc", "OriginalNtSetInformationFileFunc"
};

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

// ブロックされた CreateFileA の戻り値として返すため、実体のあるダミーファイル HANDLE を生成する
HANDLE create_honeypot_dummy_file_a(void){
    OriginalCreateFileA original = (OriginalCreateFileA)originalcreatefileavar;
    char tempPath[MAX_PATH];
    char dummyPath[MAX_PATH];

    if (original == NULL) return INVALID_HANDLE_VALUE;
    if (GetTempPathA(MAX_PATH, tempPath) == 0) strcpy(tempPath, ".\\");
    sprintf(dummyPath, "%sransomwarevaccine_honeypot_%lu_A_%lu.tmp", tempPath, GetCurrentProcessId(), honeypotCreateIndexA++);
    return original(dummyPath, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_TEMPORARY, NULL);
}

// ブロックされた CreateFileW の戻り値として返すため、Wide 文字列パスのダミーファイル HANDLE を生成する
HANDLE create_honeypot_dummy_file_w(void){
    OriginalCreateFileW original = (OriginalCreateFileW)originalcreatefilewvar;
    wchar_t tempPath[MAX_PATH];
    wchar_t dummyPath[MAX_PATH];

    if (original == NULL) return INVALID_HANDLE_VALUE;
    if (GetTempPathW(MAX_PATH, tempPath) == 0) wcscpy(tempPath, L".\\");
    swprintf(dummyPath, MAX_PATH, L"%sransomwarevaccine_honeypot_%lu_W_%lu.tmp", tempPath, GetCurrentProcessId(), honeypotCreateIndexW++);
    return original(dummyPath, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_TEMPORARY, NULL);
}

// Write/Read のブロック時に、実対象ではなく事前作成済みダミー HANDLE へ処理を逃がす
HANDLE acquire_honeypot_handle(void){
    DWORD index = honeypotUseIndex++;

    if (index < HONEYPOT_DUMMY_COUNT && honeypotDummyHandlesA[index] != INVALID_HANDLE_VALUE && honeypotDummyHandlesA[index] != NULL) return honeypotDummyHandlesA[index];
    return create_honeypot_dummy_file_a();
}

// 全 IAT Hook の完了後に AB/WDB だけをハニーポット化し、ダミー HANDLE を閉じずに保持する
void initialize_honeypot_dummy_files(void){
    if (dwProtectMode != 1 && dwProtectMode != 2) return;

    honeypotUseIndex = 0;
    for (int i = 0; i < HONEYPOT_DUMMY_COUNT; i++){
        honeypotDummyHandlesA[i] = create_honeypot_dummy_file_a();
        honeypotDummyHandlesW[i] = create_honeypot_dummy_file_w();
    }
}

// Native API の戻り値だけでなく IoStatusBlock も成功扱いにし、呼び出し側の異常検知を避ける
void set_honeypot_io_status(PVOID ioStatusBlock, NTSTATUS status, ULONG_PTR information){
    if (ioStatusBlock == NULL) return;
    PHONEYPOT_IO_STATUS_BLOCK statusBlock = (PHONEYPOT_IO_STATUS_BLOCK)ioStatusBlock;
    statusBlock->Status = status;
    statusBlock->Information = information;
}

// WDB では読み取り open を通し、作成・上書き・書込権限付き open をハニーポットへ逃がす
BOOL should_redirect_nt_create_file(ACCESS_MASK desiredAccess, ULONG createDisposition){
    ACCESS_MASK writeAccess =
        GENERIC_WRITE |
        FILE_WRITE_DATA |
        FILE_APPEND_DATA |
        FILE_WRITE_ATTRIBUTES |
        FILE_WRITE_EA |
        DELETE |
        WRITE_DAC |
        WRITE_OWNER;

    if ((desiredAccess & writeAccess) != 0) return TRUE;
    if (createDisposition == FILE_CREATE || createDisposition == FILE_OVERWRITE || createDisposition == FILE_OVERWRITE_IF || createDisposition == FILE_SUPERSEDE) return TRUE;
    return FALSE;
}

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
    if (dwProtectMode == 1) return create_honeypot_dummy_file_w();
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
    if (dwProtectMode == 1) return create_honeypot_dummy_file_a();
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
    
    // AB・WDB は実ファイルではなくダミー HANDLE へ書き込み、呼び出し元には成功/失敗を自然に返す
    if (dwProtectMode == 1 || dwProtectMode == 2) {
        HANDLE honeypotHandle = acquire_honeypot_handle();
        if (honeypotHandle == INVALID_HANDLE_VALUE || honeypotHandle == NULL) return FALSE;
        OriginalWriteFile original = (OriginalWriteFile)originalwritefilevar;
        SetFilePointer(honeypotHandle, 0, NULL, FILE_END);
        return original(honeypotHandle, lpBuffer, nNumberOfBytesToWrite, lpNumberOfBytesWritten, lpOverlapped);
    }
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
    
    // AB は実ファイルではなくダミー HANDLE から読み取り、WDB・AN は通過させる
    if (dwProtectMode == 1) {
        HANDLE honeypotHandle = acquire_honeypot_handle();
        if (honeypotHandle == INVALID_HANDLE_VALUE || honeypotHandle == NULL) return FALSE;
        OriginalReadFile original = (OriginalReadFile)originalreadfilevar;
        SetFilePointer(honeypotHandle, 0, NULL, FILE_BEGIN);
        return original(honeypotHandle, (LPVOID)lpBuffer, nNumberOfBytesToWrite, lpNumberOfBytesWritten, lpOverlapped);
    }
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
    if (dwProtectMode == 1 || dwProtectMode == 2) return TRUE;
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
    if (dwProtectMode == 1 || dwProtectMode == 2) return TRUE;
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
    if (dwProtectMode == 1 || dwProtectMode == 2) return TRUE;
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
    if (dwProtectMode == 1 || dwProtectMode == 2) return TRUE;
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
    if (dwProtectMode == 1 || dwProtectMode == 2) return TRUE;
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
    if (dwProtectMode == 1 || dwProtectMode == 2) return TRUE;
    OriginalMoveFileW original = (OriginalMoveFileW)originalmovefilewvar;
    return original(lpExistingFileName, lpNewFileName);
}

// CreateFile2 フック: AB では実対象を開かず dummy HANDLE を返し、WDB/AN は通過させる
HANDLE WINAPI OriginalCreateFile2Func(LPCWSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, DWORD dwCreationDisposition, PVOID pCreateExParams){
    if (dwNoticeMode == 1) MessageBoxW(NULL, lpFileName, L"vaccinedll-CreateFile2", MB_OK);
    else if (dwNoticeMode == 2) wprintf(L"\n\x1b[36mCreateFile2 for \x1b[39m%ls\n", lpFileName);

    if (bLogMode){
        char filenameA[512];
        WideCharToMultiByte(CP_ACP, 0, lpFileName, -1, filenameA, sizeof(filenameA), NULL, NULL);
        write_log("CreateFile2", filenameA);
    }

    if (dwProtectMode == 1) return create_honeypot_dummy_file_w();
    OriginalCreateFile2 original = (OriginalCreateFile2)originalcreatefile2var;
    if (original == NULL) return INVALID_HANDLE_VALUE;
    return original(lpFileName, dwDesiredAccess, dwShareMode, dwCreationDisposition, pCreateExParams);
}

// SetFileInformationByHandle フック: rename/delete/truncate 系をまとめて成功偽装する
BOOL WINAPI OriginalSetFileInformationByHandleFunc(HANDLE hFile, FILE_INFO_BY_HANDLE_CLASS FileInformationClass, LPVOID lpFileInformation, DWORD dwBufferSize){
    if (dwNoticeMode == 1) MessageBoxA(NULL, "SetFileInformationByHandle", "vaccinedll-SetFileInformationByHandle", MB_OK);
    else if (dwNoticeMode == 2) printf("\n\x1b[31mSetFileInformationByHandle blocked class:%d\x1b[39m\n", (int)FileInformationClass);
    if (bLogMode) write_log("SetFileInformationByHandle", "(handle)");

    if (dwProtectMode == 1 || dwProtectMode == 2) return TRUE;
    OriginalSetFileInformationByHandle original = (OriginalSetFileInformationByHandle)originalsetfileinformationbyhandlevar;
    if (original == NULL) return FALSE;
    return original(hFile, FileInformationClass, lpFileInformation, dwBufferSize);
}

// MoveFileExA フック: 実移動を行わず成功扱いにして元ファイルを保護する
BOOL WINAPI OriginalMoveFileExAFunc(LPCSTR lpExistingFileName, LPCSTR lpNewFileName, DWORD dwFlags){
    if (dwNoticeMode == 1) MessageBoxA(NULL, lpExistingFileName, "vaccinedll-MoveFileExA", MB_OK);
    else if (dwNoticeMode == 2) printf("\n\x1b[36mMoveFileEx for \x1b[39m%s\n", lpExistingFileName);
    if (bLogMode) write_log("MoveFileExA", lpExistingFileName);

    if (dwProtectMode == 1 || dwProtectMode == 2) return TRUE;
    OriginalMoveFileExA original = (OriginalMoveFileExA)originalmovefileexavar;
    if (original == NULL) return FALSE;
    return original(lpExistingFileName, lpNewFileName, dwFlags);
}

// MoveFileExW フック: Wide API 経由の rename/delete-on-reboot 系を成功偽装する
BOOL WINAPI OriginalMoveFileExWFunc(LPCWSTR lpExistingFileName, LPCWSTR lpNewFileName, DWORD dwFlags){
    if (dwNoticeMode == 1) MessageBoxW(NULL, lpExistingFileName, L"vaccinedll-MoveFileExW", MB_OK);
    else if (dwNoticeMode == 2) wprintf(L"\n\x1b[36mMoveFileEx for \x1b[39m%ls\n", lpExistingFileName);
    if (bLogMode){
        char filenameA[512];
        WideCharToMultiByte(CP_ACP, 0, lpExistingFileName, -1, filenameA, sizeof(filenameA), NULL, NULL);
        write_log("MoveFileExW", filenameA);
    }

    if (dwProtectMode == 1 || dwProtectMode == 2) return TRUE;
    OriginalMoveFileExW original = (OriginalMoveFileExW)originalmovefileexwvar;
    if (original == NULL) return FALSE;
    return original(lpExistingFileName, lpNewFileName, dwFlags);
}

// CopyFileExA フック: コピー先作成を行わず成功扱いにする
BOOL WINAPI OriginalCopyFileExAFunc(LPCSTR lpExistingFileName, LPCSTR lpNewFileName, LPPROGRESS_ROUTINE lpProgressRoutine, LPVOID lpData, LPBOOL pbCancel, DWORD dwCopyFlags){
    if (dwNoticeMode == 1) MessageBoxA(NULL, lpExistingFileName, "vaccinedll-CopyFileExA", MB_OK);
    else if (dwNoticeMode == 2) printf("\n\x1b[36mCopyFileEx for \x1b[39m%s\n", lpExistingFileName);
    if (bLogMode) write_log("CopyFileExA", lpExistingFileName);

    if (dwProtectMode == 1 || dwProtectMode == 2) return TRUE;
    OriginalCopyFileExA original = (OriginalCopyFileExA)originalcopyfileexavar;
    if (original == NULL) return FALSE;
    return original(lpExistingFileName, lpNewFileName, lpProgressRoutine, lpData, pbCancel, dwCopyFlags);
}

// CopyFileExW フック: Wide API 経由のコピーを実行せず成功扱いにする
BOOL WINAPI OriginalCopyFileExWFunc(LPCWSTR lpExistingFileName, LPCWSTR lpNewFileName, LPPROGRESS_ROUTINE lpProgressRoutine, LPVOID lpData, LPBOOL pbCancel, DWORD dwCopyFlags){
    if (dwNoticeMode == 1) MessageBoxW(NULL, lpExistingFileName, L"vaccinedll-CopyFileExW", MB_OK);
    else if (dwNoticeMode == 2) wprintf(L"\n\x1b[36mCopyFileEx for \x1b[39m%ls\n", lpExistingFileName);
    if (bLogMode){
        char filenameA[512];
        WideCharToMultiByte(CP_ACP, 0, lpExistingFileName, -1, filenameA, sizeof(filenameA), NULL, NULL);
        write_log("CopyFileExW", filenameA);
    }

    if (dwProtectMode == 1 || dwProtectMode == 2) return TRUE;
    OriginalCopyFileExW original = (OriginalCopyFileExW)originalcopyfileexwvar;
    if (original == NULL) return FALSE;
    return original(lpExistingFileName, lpNewFileName, lpProgressRoutine, lpData, pbCancel, dwCopyFlags);
}

// ReplaceFileA フック: 置換操作を実行せず成功扱いにして既存ファイルを守る
BOOL WINAPI OriginalReplaceFileAFunc(LPCSTR lpReplacedFileName, LPCSTR lpReplacementFileName, LPCSTR lpBackupFileName, DWORD dwReplaceFlags, LPVOID lpExclude, LPVOID lpReserved){
    if (dwNoticeMode == 1) MessageBoxA(NULL, lpReplacedFileName, "vaccinedll-ReplaceFileA", MB_OK);
    else if (dwNoticeMode == 2) printf("\n\x1b[31mReplaceFile for \x1b[39m%s\n", lpReplacedFileName);
    if (bLogMode) write_log("ReplaceFileA", lpReplacedFileName);

    if (dwProtectMode == 1 || dwProtectMode == 2) return TRUE;
    OriginalReplaceFileA original = (OriginalReplaceFileA)originalreplacefileavar;
    if (original == NULL) return FALSE;
    return original(lpReplacedFileName, lpReplacementFileName, lpBackupFileName, dwReplaceFlags, lpExclude, lpReserved);
}

// ReplaceFileW フック: Wide API 経由の置換操作を成功偽装する
BOOL WINAPI OriginalReplaceFileWFunc(LPCWSTR lpReplacedFileName, LPCWSTR lpReplacementFileName, LPCWSTR lpBackupFileName, DWORD dwReplaceFlags, LPVOID lpExclude, LPVOID lpReserved){
    if (dwNoticeMode == 1) MessageBoxW(NULL, lpReplacedFileName, L"vaccinedll-ReplaceFileW", MB_OK);
    else if (dwNoticeMode == 2) wprintf(L"\n\x1b[31mReplaceFile for \x1b[39m%ls\n", lpReplacedFileName);
    if (bLogMode){
        char filenameA[512];
        WideCharToMultiByte(CP_ACP, 0, lpReplacedFileName, -1, filenameA, sizeof(filenameA), NULL, NULL);
        write_log("ReplaceFileW", filenameA);
    }

    if (dwProtectMode == 1 || dwProtectMode == 2) return TRUE;
    OriginalReplaceFileW original = (OriginalReplaceFileW)originalreplacefilewvar;
    if (original == NULL) return FALSE;
    return original(lpReplacedFileName, lpReplacementFileName, lpBackupFileName, dwReplaceFlags, lpExclude, lpReserved);
}

// NtCreateFile フック: Native API 経由の作成・上書きを dummy HANDLE に逃がす
NTSTATUS WINAPI OriginalNtCreateFileFunc(PHANDLE FileHandle, ACCESS_MASK DesiredAccess, PVOID ObjectAttributes, PVOID IoStatusBlock, PLARGE_INTEGER AllocationSize, ULONG FileAttributes, ULONG ShareAccess, ULONG CreateDisposition, ULONG CreateOptions, PVOID EaBuffer, ULONG EaLength){
    if (dwNoticeMode == 1) MessageBoxA(NULL, "NtCreateFile", "vaccinedll-NtCreateFile", MB_OK);
    else if (dwNoticeMode == 2) printf("\n\x1b[36mNtCreateFile access:0x%lx disposition:%lu\x1b[39m\n", (unsigned long)DesiredAccess, CreateDisposition);
    if (bLogMode) write_log("NtCreateFile", "(native)");

    if (dwProtectMode == 1 || (dwProtectMode == 2 && should_redirect_nt_create_file(DesiredAccess, CreateDisposition))){
        HANDLE honeypotHandle = acquire_honeypot_handle();
        if (honeypotHandle == INVALID_HANDLE_VALUE || honeypotHandle == NULL) return STATUS_UNSUCCESSFUL;
        if (FileHandle != NULL) *FileHandle = honeypotHandle;
        set_honeypot_io_status(IoStatusBlock, STATUS_SUCCESS, 1);
        return STATUS_SUCCESS;
    }

    OriginalNtCreateFile original = (OriginalNtCreateFile)originalntcreatefilevar;
    if (original == NULL) return STATUS_UNSUCCESSFUL;
    return original(FileHandle, DesiredAccess, ObjectAttributes, IoStatusBlock, AllocationSize, FileAttributes, ShareAccess, CreateDisposition, CreateOptions, EaBuffer, EaLength);
}

// NtWriteFile フック: Native API 経由の書き込みを dummy HANDLE へ転送する
NTSTATUS WINAPI OriginalNtWriteFileFunc(HANDLE FileHandle, HANDLE Event, PVOID ApcRoutine, PVOID ApcContext, PVOID IoStatusBlock, PVOID Buffer, ULONG Length, PLARGE_INTEGER ByteOffset, PULONG Key){
    if (dwNoticeMode == 1) MessageBoxA(NULL, "NtWriteFile", "vaccinedll-NtWriteFile", MB_OK);
    else if (dwNoticeMode == 2) printf("\n\x1b[33mNtWriteFile size:%lu\x1b[39m\n", Length);
    if (bLogMode) write_log("NtWriteFile", "(native)");

    if (dwProtectMode == 1 || dwProtectMode == 2){
        HANDLE honeypotHandle = acquire_honeypot_handle();
        if (honeypotHandle == INVALID_HANDLE_VALUE || honeypotHandle == NULL) return STATUS_UNSUCCESSFUL;
        DWORD bytesWritten = 0;
        OriginalWriteFile originalWrite = (OriginalWriteFile)originalwritefilevar;
        SetFilePointer(honeypotHandle, 0, NULL, FILE_END);
        if (!originalWrite(honeypotHandle, Buffer, Length, &bytesWritten, NULL)) return STATUS_UNSUCCESSFUL;
        set_honeypot_io_status(IoStatusBlock, STATUS_SUCCESS, bytesWritten);
        return STATUS_SUCCESS;
    }

    OriginalNtWriteFile original = (OriginalNtWriteFile)originalntwritefilevar;
    if (original == NULL) return STATUS_UNSUCCESSFUL;
    return original(FileHandle, Event, ApcRoutine, ApcContext, IoStatusBlock, Buffer, Length, ByteOffset, Key);
}

// NtReadFile フック: AB では Native API 経由の読み取りも dummy HANDLE へ向ける
NTSTATUS WINAPI OriginalNtReadFileFunc(HANDLE FileHandle, HANDLE Event, PVOID ApcRoutine, PVOID ApcContext, PVOID IoStatusBlock, PVOID Buffer, ULONG Length, PLARGE_INTEGER ByteOffset, PULONG Key){
    if (dwNoticeMode == 1) MessageBoxA(NULL, "NtReadFile", "vaccinedll-NtReadFile", MB_OK);
    else if (dwNoticeMode == 2) printf("\n\x1b[32mNtReadFile size:%lu\x1b[39m\n", Length);
    if (bLogMode) write_log("NtReadFile", "(native)");

    if (dwProtectMode == 1){
        HANDLE honeypotHandle = acquire_honeypot_handle();
        if (honeypotHandle == INVALID_HANDLE_VALUE || honeypotHandle == NULL) return STATUS_UNSUCCESSFUL;
        DWORD bytesRead = 0;
        OriginalReadFile originalRead = (OriginalReadFile)originalreadfilevar;
        SetFilePointer(honeypotHandle, 0, NULL, FILE_BEGIN);
        if (!originalRead(honeypotHandle, Buffer, Length, &bytesRead, NULL)) return STATUS_UNSUCCESSFUL;
        set_honeypot_io_status(IoStatusBlock, STATUS_SUCCESS, bytesRead);
        return STATUS_SUCCESS;
    }

    OriginalNtReadFile original = (OriginalNtReadFile)originalntreadfilevar;
    if (original == NULL) return STATUS_UNSUCCESSFUL;
    return original(FileHandle, Event, ApcRoutine, ApcContext, IoStatusBlock, Buffer, Length, ByteOffset, Key);
}

// NtDeleteFile フック: Native API 経由の削除を実行せず成功扱いにする
NTSTATUS WINAPI OriginalNtDeleteFileFunc(PVOID ObjectAttributes){
    if (dwNoticeMode == 1) MessageBoxA(NULL, "NtDeleteFile", "vaccinedll-NtDeleteFile", MB_OK);
    else if (dwNoticeMode == 2) printf("\n\x1b[31mNtDeleteFile blocked\x1b[39m\n");
    if (bLogMode) write_log("NtDeleteFile", "(native)");

    if (dwProtectMode == 1 || dwProtectMode == 2) return STATUS_SUCCESS;
    OriginalNtDeleteFile original = (OriginalNtDeleteFile)originalntdeletefilevar;
    if (original == NULL) return STATUS_UNSUCCESSFUL;
    return original(ObjectAttributes);
}

// NtSetInformationFile フック: Native API 経由の rename/delete/truncate をまとめて成功偽装する
NTSTATUS WINAPI OriginalNtSetInformationFileFunc(HANDLE FileHandle, PVOID IoStatusBlock, PVOID FileInformation, ULONG Length, ULONG FileInformationClass){
    if (dwNoticeMode == 1) MessageBoxA(NULL, "NtSetInformationFile", "vaccinedll-NtSetInformationFile", MB_OK);
    else if (dwNoticeMode == 2) printf("\n\x1b[31mNtSetInformationFile class:%lu\x1b[39m\n", FileInformationClass);
    if (bLogMode) write_log("NtSetInformationFile", "(native)");

    if (dwProtectMode == 1 || dwProtectMode == 2){
        set_honeypot_io_status(IoStatusBlock, STATUS_SUCCESS, 0);
        return STATUS_SUCCESS;
    }

    OriginalNtSetInformationFile original = (OriginalNtSetInformationFile)originalntsetinformationfilevar;
    if (original == NULL) return STATUS_UNSUCCESSFUL;
    return original(FileHandle, IoStatusBlock, FileInformation, Length, FileInformationClass);
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
    if (((ULONG_PTR)lpProcName >> 16) == 0) return original(hModule, lpProcName);
    char funcname[256];
    strcpy(funcname, lpProcName);
    // AB/WDB では GetProcAddress による IAT 回避もフック関数へ戻す
    if (dwProtectMode == 1 || dwProtectMode == 2){
        for (int i = 0; i < HOOK_REDIRECT_COUNT; i++){
            if (!strcmp(lpProcName, funcvardir[i])){
                MessageBoxA(NULL, "GetProcAddress", funcvardir[i], MB_OK);
                strcpy(funcname, originalfuncvardir[i]);
                hModule = GetModuleHandleA("ransomwarevaccine_vaccinedll.dll");
            }
        }
    }
    FARPROC returnvar = original(hModule, funcname);
    char errorfuncname[256];
    sprintf(errorfuncname, "GetProcAddress error, Call Func:%s", lpProcName);
    if (returnvar == NULL) MessageBoxA(NULL, errorfuncname, "Error", MB_OK);
    return returnvar;
}

// FreeLibrary フック: ワクチンDLL のアンロードのみ防止
BOOL WINAPI OriginalFreeLibraryFunc(HMODULE hModule){
    // hModule のファイルパスを取得
    wchar_t modulePath[MAX_PATH];
    if (GetModuleFileNameW(hModule, modulePath, MAX_PATH) == 0){
        // パス取得失敗時は元の関数を呼ぶ
        OriginalFreeLibrary original = (OriginalFreeLibrary)originalfreelibraryvar;
        return original(hModule);
    }
    
    // ワクチンDLL のパスを取得（環境変数から）
    wchar_t vaccineDllPathW[MAX_PATH];
    MultiByteToWideChar(CP_ACP, 0, dll_path, -1, vaccineDllPathW, MAX_PATH);
    
    if (_wcsicmp(modulePath, vaccineDllPathW) == 0){
        if (dwNoticeMode == 1) MessageBoxA(NULL, "FreeLibrary blocked (vaccine DLL)", "vaccinedll-freelibrary", MB_OK);
        else if (dwNoticeMode == 2) printf("\n\x1b[35mFreeLibrary blocked for vaccine DLL\x1b[39m\n");
        return TRUE;
    }
    
    OriginalFreeLibrary original = (OriginalFreeLibrary)originalfreelibraryvar;
    return original(hModule);
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

    HMODULE hTargetDll = GetModuleHandleA(dllname);
    if (hTargetDll == NULL) return 0;
    FARPROC originaldebug = GetProcAddress(hTargetDll, funcname);
    if (originaldebug == NULL) return 0;
    *pOriginalOut = originaldebug;

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
        iathook(NULL,            "Kernel32.dll",   "CreateFile2",      (HookTargetFunc)OriginalCreateFile2Func,      &originalcreatefile2var);
        iathook(NULL,            "Kernel32.dll",   "SetFileInformationByHandle", (HookTargetFunc)OriginalSetFileInformationByHandleFunc, &originalsetfileinformationbyhandlevar);
        iathook(NULL,            "Kernel32.dll",   "MoveFileExA",      (HookTargetFunc)OriginalMoveFileExAFunc,      &originalmovefileexavar);
        iathook(NULL,            "Kernel32.dll",   "MoveFileExW",      (HookTargetFunc)OriginalMoveFileExWFunc,      &originalmovefileexwvar);
        iathook(NULL,            "Kernel32.dll",   "CopyFileExA",      (HookTargetFunc)OriginalCopyFileExAFunc,      &originalcopyfileexavar);
        iathook(NULL,            "Kernel32.dll",   "CopyFileExW",      (HookTargetFunc)OriginalCopyFileExWFunc,      &originalcopyfileexwvar);
        iathook(NULL,            "Kernel32.dll",   "ReplaceFileA",     (HookTargetFunc)OriginalReplaceFileAFunc,     &originalreplacefileavar);
        iathook(NULL,            "Kernel32.dll",   "ReplaceFileW",     (HookTargetFunc)OriginalReplaceFileWFunc,     &originalreplacefilewvar);
        iathook(NULL,            "Kernelbase.dll", "CreateFile2",      (HookTargetFunc)OriginalCreateFile2Func,      &originalcreatefile2var);
        iathook(NULL,            "Kernelbase.dll", "SetFileInformationByHandle", (HookTargetFunc)OriginalSetFileInformationByHandleFunc, &originalsetfileinformationbyhandlevar);
        iathook(NULL,            "Kernelbase.dll", "MoveFileExA",      (HookTargetFunc)OriginalMoveFileExAFunc,      &originalmovefileexavar);
        iathook(NULL,            "Kernelbase.dll", "MoveFileExW",      (HookTargetFunc)OriginalMoveFileExWFunc,      &originalmovefileexwvar);
        iathook(NULL,            "Kernelbase.dll", "CopyFileExA",      (HookTargetFunc)OriginalCopyFileExAFunc,      &originalcopyfileexavar);
        iathook(NULL,            "Kernelbase.dll", "CopyFileExW",      (HookTargetFunc)OriginalCopyFileExWFunc,      &originalcopyfileexwvar);
        iathook(NULL,            "Kernelbase.dll", "ReplaceFileA",     (HookTargetFunc)OriginalReplaceFileAFunc,     &originalreplacefileavar);
        iathook(NULL,            "Kernelbase.dll", "ReplaceFileW",     (HookTargetFunc)OriginalReplaceFileWFunc,     &originalreplacefilewvar);
        iathook(NULL,            "ntdll.dll",      "NtCreateFile",     (HookTargetFunc)OriginalNtCreateFileFunc,     &originalntcreatefilevar);
        iathook(NULL,            "ntdll.dll",      "NtWriteFile",      (HookTargetFunc)OriginalNtWriteFileFunc,      &originalntwritefilevar);
        iathook(NULL,            "ntdll.dll",      "NtReadFile",       (HookTargetFunc)OriginalNtReadFileFunc,       &originalntreadfilevar);
        iathook(NULL,            "ntdll.dll",      "NtDeleteFile",     (HookTargetFunc)OriginalNtDeleteFileFunc,     &originalntdeletefilevar);
        iathook(NULL,            "ntdll.dll",      "NtSetInformationFile", (HookTargetFunc)OriginalNtSetInformationFileFunc, &originalntsetinformationfilevar);
        iathook(NULL,            "Kernel32.dll",   "CreateProcessW",   (HookTargetFunc)OriginalCreateProcessWFunc,   &originalcreateprocesswvar);
        iathook(NULL,            "Kernel32.dll",   "CreateProcessA",   (HookTargetFunc)OriginalCreateProcessAFunc,   &originalcreateprocessavar);
        iathook(NULL,            "Kernel32.dll",   "GetProcAddress",   (HookTargetFunc)OriginalGetProcAddressFunc,   &originalgetproccaddressvar);
        iathook(NULL,            "Kernel32.dll",   "FreeLibrary",      (HookTargetFunc)OriginalFreeLibraryFunc,      &originalfreelibraryvar);
        iathook(NULL,            "Kernel32.dll",   "VirtualAlloc",     (HookTargetFunc)OriginalVirtualAllocFunc,     &originalvirtualallocvar);
    }
    initialize_honeypot_dummy_files();
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
