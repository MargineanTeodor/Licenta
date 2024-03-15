#include <windows.h>
#include <stdio.h>

DWORD WINAPI threadFunc1(LPVOID lpParam) {
    HANDLE writeHandle = *(HANDLE*)lpParam;
     const char* message = "Hello from Thread 1";
    DWORD written;

    WriteFile(writeHandle, message, strlen(message) + 1, &written, NULL);
    CloseHandle(writeHandle);
    return 0;
}

DWORD WINAPI threadFunc2(LPVOID lpParam) {
    HANDLE readHandle = *(HANDLE*)lpParam;
    char buffer[100];
    DWORD read;

    ReadFile(readHandle, buffer, 100, &read, NULL);
    printf("Thread 2 received: %s\n", buffer);
    CloseHandle(readHandle);
    return 0;
}
//int main2() {
int mai2n() {
    HANDLE readHandle, writeHandle;
    SECURITY_ATTRIBUTES sa = { sizeof(SECURITY_ATTRIBUTES), NULL, TRUE };
    HANDLE thread1, thread2;
    DWORD threadId1, threadId2;

    // Create an anonymous pipe
    if (!CreatePipe(&readHandle, &writeHandle, &sa, 0)) {
        fprintf(stderr, "CreatePipe failed");
        return 1;
    }

    // Create the first thread
    thread1 = CreateThread(NULL, 0, threadFunc1, &writeHandle, 0, &threadId1);
    if (thread1 == NULL) {
        fprintf(stderr, "CreateThread failed");
        CloseHandle(writeHandle);
        CloseHandle(readHandle);
        return 1;
    }

    // Create the second thread
    thread2 = CreateThread(NULL, 0, threadFunc2, &readHandle, 0, &threadId2);
    if (thread2 == NULL) {
        fprintf(stderr, "CreateThread failed");
        CloseHandle(writeHandle);
        CloseHandle(readHandle);
        return 1;
    }

    // Wait for the threads to finish
    WaitForSingleObject(thread1, INFINITE);
    WaitForSingleObject(thread2, INFINITE);

    // Cleanup
    CloseHandle(thread1);
    CloseHandle(thread2);

    return 0;
}
