#include <stdio.h>
#include <windows.h>

void helpoption(){
    printf("Ransomware Vaccine - File Access Monitor & Protection Tool\n\n");
    
    printf("USAGE:\n");
    printf("    ransomwarevaccine_injection.exe <target> [options]\n\n");
    
    printf("ARGUMENTS:\n");
    printf("    <target>    Target file path or process name (e.g., target.exe)\n\n");
    
    printf("PROTECTION MODE OPTIONS:\n");
    printf("    -AB         All Block mode\n");
    printf("                Blocks all file operations: create, write, read, delete, and\n");
    printf("                directory creation\n");
    printf("                [RECOMMENDED FOR] Spyware-equipped ransomware that may steal\n");
    printf("                file information, or when protecting highly sensitive data\n\n");
    
    printf("    -AN         All No block mode\n");
    printf("                Monitors all file access functions without blocking any operations\n");
    printf("                [RECOMMENDED FOR] Use to observe and analyze suspicious behavior\n");
    printf("                patterns without interfering with process execution\n\n");

    printf("    -WDB        Write/Delete Block mode\n");
    printf("                Blocks only write and delete operations (allows read access)\n");
    printf("                [RECOMMENDED FOR] Suspected ransomware without spyware features\n\n");

    printf("NOTIFICATION OPTIONS:\n");
    printf("    -TM         Enable notifications via MessageBox (GUI popup)\n");
    printf("    -TC         Enable notifications via console output (CUI)\n");
    printf("    -F          Disable all notifications (silent mode)\n\n");
    
    printf("LOGGING OPTIONS:\n");
    printf("    -T          Enable logging\n");
    printf("    -F          Disable logging\n\n");
    
    printf("EXAMPLES:\n");
    printf("    ransomwarevaccine_injection.exe suspicious.exe -AB -TM -T\n");
    printf("        Monitor suspicious.exe with full blocking, GUI notifications, and logging\n\n");
    
    printf("    ransomwarevaccine_injection.exe malware.exe -WDB -TC -F\n");
    printf("        Block writes/deletes only, console notifications, no logging\n\n");
    
    printf("    ransomwarevaccine_injection.exe unknown.exe -AN -F -T\n");
    printf("        No Block Mode. Monitoring and notification only.\n");
}


int main(int argc, char *selectmode[]){
    if (argc < 5){
        if (argc >= 2 && (!strcmp(selectmode[1], "-help") || !strcmp(selectmode[1], "-Help") || !strcmp(selectmode[1], "-h") || !strcmp(selectmode[1], "-H"))){
            helpoption();
            return 0;
        }
        printf("plz <target file path or name> <option> <notice> <log>\nFor details on options, use the help command. -help");
        return 1;
    }

    if (!strcmp(selectmode[2], "-AB")) SetEnvironmentVariableA("ransomwarevaccine_protect_mode", "1");
    else if (!strcmp(selectmode[2], "-WDB")) SetEnvironmentVariableA("ransomwarevaccine_protect_mode", "2");
    else if (!strcmp(selectmode[2], "-AN")) SetEnvironmentVariableA("ransomwarevaccine_protect_mode", "3");
    else{
        printf("Selecting the protect mode -AB(All Block), -WDB(Write/Delete Block), -AN(All No block)\n");
        return 1;
    }

    char targetfilename[256];
    if (argc>=6){
        sprintf(targetfilename, "%s\\%s", selectmode[5], "ransomwarevaccine_vaccinedll.dll");
        if (GetFileAttributesA(targetfilename) == INVALID_FILE_ATTRIBUTES) {
            MessageBoxA(NULL, "This directory cannot be found", "ERROR MESSAGE", MB_ICONERROR);
            return 1;
        }
    }else {
        char szInjectionPath[MAX_PATH];
        char szInjectionDir[MAX_PATH];
        GetModuleFileNameA(NULL, szInjectionPath, MAX_PATH);
    
        char *lastBackslash = strrchr(szInjectionPath, '\\');
        if (lastBackslash) {
            int dirLength = lastBackslash - szInjectionPath;
            strncpy(szInjectionDir, szInjectionPath, dirLength);
            szInjectionDir[dirLength] = '\0';
        } else {
            strcpy(szInjectionDir, ".");
        }
        
        sprintf(targetfilename, "%s\\%s", szInjectionDir, "ransomwarevaccine_vaccinedll.dll");
    }
    
    if (!strcmp(selectmode[3], "-TM")) SetEnvironmentVariableA("ransomwarevaccine_notice_mode", "1");
    else if (!strcmp(selectmode[3], "-TC")) SetEnvironmentVariableA("ransomwarevaccine_notice_mode", "2");
    else if (!strcmp(selectmode[3], "-F")) SetEnvironmentVariableA("ransomwarevaccine_notice_mode", "0");
    else{
        printf("Selecting the notice mode -TM (notice on messagebox), -TC(notice on CUI), -F(notice off)");
        return 1;
    }

    if (!strcmp(selectmode[4], "-T")) SetEnvironmentVariableA("ransomwarevaccine_log_mode", "1");
    else if (!strcmp(selectmode[4], "-F")) SetEnvironmentVariableA("ransomwarevaccine_log_mode", "0");
    else{
        printf("Selecting the notice mode -T (log on), -F(log off)");
        return 1;
    }
    
    wchar_t applicationName[256];
    MultiByteToWideChar(CP_ACP, 0, selectmode[1], -1, applicationName, strlen(selectmode[1])+1);

    wchar_t currentDirectory[1024];
    GetCurrentDirectoryW(1024, currentDirectory);
    STARTUPINFOW startupInfo = { sizeof(startupInfo) };
    PROCESS_INFORMATION processInfo;
    LPTHREAD_START_ROUTINE LoadLibAddr = (LPTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandleA("kernel32.dll"), "LoadLibraryA");

    if (LoadLibAddr == NULL) MessageBoxA(NULL, "LoadLibrary GetProcAddress Error", "Error", MB_OK);

    SetEnvironmentVariableA("ransomwarevaccine_dll_path", targetfilename);

    if (CreateProcessW(applicationName, NULL, NULL, NULL, FALSE, CREATE_SUSPENDED, NULL, currentDirectory, &startupInfo, &processInfo)){
        if (processInfo.hProcess == NULL) MessageBoxA(NULL, "not found process", "ERROR MESSAGE", MB_OK);
        LPVOID lpvoidbuffer = VirtualAllocEx(processInfo.hProcess, NULL, 2048, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
        if (lpvoidbuffer == NULL) MessageBoxA(NULL, "VirtualAllocEx Error", "ERROR MESSAGE", MB_OK);
        if (WriteProcessMemory(processInfo.hProcess, lpvoidbuffer, (LPCVOID)targetfilename, strlen(targetfilename)+1, NULL) == 0) MessageBoxA(NULL, "write memory failure", "ERROR MESSAGE", MB_OK); 
        HANDLE remotethreadhandle = CreateRemoteThread(processInfo.hProcess, NULL, 0, LoadLibAddr, lpvoidbuffer, 0, NULL);
        if (remotethreadhandle == NULL) {
            MessageBoxA(NULL, "create thread failure", "ERROR MESSAGE", MB_OK);
            TerminateProcess(processInfo.hProcess, 1);
            return 1;
        }else{
            WaitForSingleObject(remotethreadhandle, INFINITE);
            CloseHandle(remotethreadhandle);
        }
        ResumeThread(processInfo.hThread);
        WaitForSingleObject(processInfo.hProcess, INFINITE);
    }else{
        if (selectmode[5] == NULL) MessageBoxA(NULL, "CreateProcessW Error", "ERROR MESSAGE", MB_ICONERROR);
        else MessageBoxA(NULL, "The specified path may not exist.", "ERROR MESSAGE", MB_ICONERROR);
        return 0;
    }
    CloseHandle(&processInfo);
    return 0;
}