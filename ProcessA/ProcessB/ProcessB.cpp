/*
Client Program for Win32 Named Pipes Example.
Copyright (C) 2012 Peter R. Bloomfield.

For an exaplanation of the code, see the associated blog post:
http://avidinsight.uk/2012/03/introduction-to-win32-named-pipes-cpp/

This code is made freely available under the MIT open source license
(see accompanying LICENSE file for details).
It is intended only for educational purposes. and is provide as-is with no
guarantee about its reliability, correctness, or suitability for any purpose.


INSTRUCTIONS:

Run the accompanying server program first.
Before closing it, run this client program.
*/

#include <iostream>
#include <windows.h>
using namespace std;

int wchartToInt(const wchar_t* arr)
{
    int result = 0;
    int sign = 1;

    // skip leading whitespace
    while (*arr == L' ')
        arr++;

    // handle optional sign
    if (*arr == L'+')
        arr++;
    else if (*arr == L'-')
    {
        sign = -1;
        arr++;
    }

    // convert digits to integer
    while (*arr >= L'0' && *arr <= L'9')
    {
        result = result * 10 + (*arr - L'0');
        arr++;
    }

    return sign * result;
}

bool valid(char s) {
    if (s >= 'A' && s <= 'Z') return 1;
    if (s >= 'a' && s <= 'z') return 1;
    if (s >= '0' && s <= '9') return 1;

    return 0;
}

string wchartToString(const wchar_t *arr, int size) {
    // Your wchar_t*
    //wide char array
    //wide char array
    const wchar_t *wc = arr;

    //convert from wide char to narrow char array
    char ch[260];
    char DefChar = ' ';
    WideCharToMultiByte(CP_ACP, 0, wc, -1, ch, 260, &DefChar, NULL);

    //A std:string  using the char* constructor.
    std::string ss(ch);

    while (ss.size() > size) {
        ss.pop_back();
    }

    return ss;
}


int main(int argc, const char** argv)
{
    wcout << "Connecting to pipe..." << endl;

    // Open the named pipe
    // Most of these parameters aren't very relevant for pipes.
    HANDLE pipe = CreateFile(
        L"\\\\.\\pipe\\my_pipe",
        GENERIC_READ, // only need read access
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );

    if (pipe == INVALID_HANDLE_VALUE) {
        wcout << "Failed to connect to pipe." << endl;
        // look up error code here using GetLastError()
        system("pause");
        return 1;
    }

    wcout << "Reading data from pipe..." << endl;

    // The read operation will block until there is data to read
    wchar_t buffer[128];
    DWORD numBytesRead = 0;
    BOOL result = ReadFile(
        pipe,
        buffer, // the data from the pipe will be put here
        127 * sizeof(wchar_t), // number of bytes allocated
        &numBytesRead, // this will store number of bytes actually read
        NULL // not using overlapped IO
    );

    if (result) {
        int dataCount = wchartToInt(buffer);
        buffer[numBytesRead / sizeof(wchar_t)] = '\0'; // null terminate the string

        for (int i = 0; i < dataCount; i++) {
            wchar_t buffer[128];
            DWORD numBytesRead = 0;
            BOOL result = ReadFile(
                pipe,
                buffer, // the data from the pipe will be put here
                127 * sizeof(wchar_t), // number of bytes allocated
                &numBytesRead, // this will store number of bytes actually read
                NULL // not using overlapped IO
            );

            cout << "Service Name: " << wchartToString(buffer, numBytesRead / sizeof(wchar_t)) << endl;

            if (!result) {
                wcout << "Failed to read data from the pipe." << endl;
            }
        }
    }
    else {
        wcout << "Failed to read data from the pipe." << endl;
    }

    // Close our pipe handle
    CloseHandle(pipe);

    wcout << "Done." << endl;

    system("pause");
    return 0;
}