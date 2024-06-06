#include <Windows.h>
#include <tlhelp32.h>
#include "threads.h"
#include "console.h"

void pauseAllThreads(bool paused) {
    FILE* consolePipe = Con::fpout;
    fprintf(consolePipe, "%s all threads\n", paused ? "Suspend" : "Resume");
    fflush(consolePipe);
    DWORD currentThreadId = GetCurrentThreadId();
    DWORD currentProcessId = GetCurrentProcessId();
    HANDLE h = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    if (h != INVALID_HANDLE_VALUE) {
        THREADENTRY32 te;
        te.dwSize = sizeof(te);
        if (Thread32First(h, &te)) {
            do {
                if (te.th32OwnerProcessID == currentProcessId && te.th32ThreadID != currentThreadId) {
                    HANDLE th = OpenThread(THREAD_SUSPEND_RESUME, false, te.th32ThreadID);
                    if (th != NULL) {
                        if (paused) {
                            if (Wow64SuspendThread(th) == -1) {
                                fprintf(Con::fpout, "Failed to suspend thread widh ID: %lu\n", te.th32ThreadID);
                            }
                        }
                        else {
                            if (ResumeThread(th) == -1) {
                                fprintf(Con::fpout, "Failed to resume thread with ID: %lu\n", te.th32ThreadID);
                            }
                        }
                        CloseHandle(th);
                    }
                    else {
                        // Handle failure to open thread handle
                        fprintf(Con::fpout, "WARNING: Failed to open thread for handle ID %lu\n", te.th32ThreadID);
                        // Additional error handling code here
                    }
                }
                te.dwSize = sizeof(te);
            } 
            while (Thread32Next(h, &te));
        }
		else {
			// Handle failure to iterate over threads
			fprintf(Con::fpout, "Failed to get first thread\n");
		}
        CloseHandle(h);
    }
    else {
        // Handle failure to create snapshot handle
        fprintf(Con::fpout, "Failed to create snapshot handle\n");
    }
}