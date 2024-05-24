#include <windows.h>
#include <stdio.h>

int ActivatePrivilege(LUID luid, HANDLE hToken) {
    TOKEN_PRIVILEGES tp;
    tp.PrivilegeCount = 1;
    tp.Privileges[0].Luid = luid;
    tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

    if (!AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(TOKEN_PRIVILEGES), (PTOKEN_PRIVILEGES)NULL, (PDWORD)NULL)) {
        printf("AdjustTokenPrivileges error: %lu\n", GetLastError());
        return FALSE;
    }
    if (GetLastError() == ERROR_NOT_ALL_ASSIGNED) {
        printf("The token does not have the specified privilege. \n");
        return FALSE;
    }
    return TRUE;
}

int main(int argc, char *argv[]) {
    HANDLE hToken = NULL;
    BOOL bRet = FALSE;
    LUID luid;

    if (argc < 3) {
        printf("\nUsage:\n");
        printf(".\\%s [<privilege> : all] <command>\n", argv[0]);
        printf("\nExample usages:\n");
        printf(".\\%s SeDebugPrivilege \"cmd.exe\"\n", argv[0]);
        printf(".\\%s all \"C:\\Windows\\System32\\WindowsPowerShell\\v1.0\\powershell.exe\"\n", argv[0]);
        printf(".\\%s all \"cmd.exe /K whoami /priv\"\n", argv[0]);
        return 0;
    }

    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) {
        printf("Failed to open process token. Error: %lu\n", GetLastError());
        return 1;
    }

    if (strncmp("all", argv[1], 4) == 0) {
        // Get size of token for buffer allocation
        DWORD dwSize = 0;
        GetTokenInformation(hToken, TokenPrivileges, NULL, 0, &dwSize);
        if (GetLastError() != ERROR_INSUFFICIENT_BUFFER) {
            printf("Failed to get token information size: %lu\n", GetLastError());
            CloseHandle(hToken);
            return 1;
        }

        // Allocate buffer for the token privileges
        PTOKEN_PRIVILEGES pTokenPrivileges = (PTOKEN_PRIVILEGES)malloc(dwSize);
        if (!pTokenPrivileges) {
            printf("Failed to allocate memory for token privileges.\n");
            CloseHandle(hToken);
            return 1;
        }

        // Get the token privileges
        if (!GetTokenInformation(hToken, TokenPrivileges, pTokenPrivileges, dwSize, &dwSize)) {
            printf("Failed to get token information. Error: %lu\n", GetLastError());
            free(pTokenPrivileges);
            CloseHandle(hToken);
            return 1;
        }

        // Enumerate the privileges and activate all of them
        for (DWORD i = 0; i < pTokenPrivileges->PrivilegeCount; ++i) {
            LUID_AND_ATTRIBUTES la = pTokenPrivileges->Privileges[i];
            luid = la.Luid;
            bRet = ActivatePrivilege(luid, hToken);
            if (!bRet) {
                printf("Failed to activate privilege: %lu\n", luid.LowPart);
            }
        }
        free(pTokenPrivileges);
    } else {
        if (!LookupPrivilegeValue(NULL, argv[1], &luid)) {
            printf("LookupPrivilegeValue error: %lu\n", GetLastError());
            CloseHandle(hToken);
            return 1;
        }
        bRet = ActivatePrivilege(luid, hToken);
        if (!bRet) {
            printf("Failed to activate privilege: %s\n", argv[1]);
        }
    }

    CloseHandle(hToken);

    if (bRet) {
        STARTUPINFO si;
        PROCESS_INFORMATION pi;

        ZeroMemory(&si, sizeof(si));
        si.cb = sizeof(si);
        ZeroMemory(&pi, sizeof(pi));

        // Create process
        if (!CreateProcess(
                NULL,
                argv[2],
                NULL,
                NULL,
                FALSE,
                0,
                NULL,
                NULL,
                &si,
                &pi)
                )
        {
            printf("CreateProcess failed (%lu).\n", GetLastError());
            return 1;
        }

        // Wait until child process exits.
        WaitForSingleObject(pi.hProcess, INFINITE);

        // Close process and thread handles.
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }
    return 0;
}
