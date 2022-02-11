#ifndef __GUICON_H__
#define __GUICON_H__

#include <windows.h>
#include <stdio.h>
#include <fcntl.h>
#include <io.h>
#include <iostream>
#include <fstream>
#include <stdint.h>

using namespace std;

static const WORD MAX_CONSOLE_LINES = 500;

// Redirect IO calls (stdout, stderr, and others) to the console output (if available)
inline void RedirectIOToConsole() {
    // make sure we actually HAVE a console to gawk at
    if (AttachConsole(ATTACH_PARENT_PROCESS))
    {
        // Redirect CRT standard input, output and error handles to the console window.
        FILE * pNewStdout = nullptr;
        FILE * pNewStderr = nullptr;
        FILE * pNewStdin = nullptr;

        ::freopen_s(&pNewStdout, "CONOUT$", "w", stdout);
        ::freopen_s(&pNewStderr, "CONOUT$", "w", stderr);
        ::freopen_s(&pNewStdin, "CONIN$", "r", stdin);

        // Clear the error state for all of the C++ standard streams. Attempting to accessing the streams before they refer
        // to a valid target causes the stream to enter an error state. Clearing the error state will fix this problem,
        // which seems to occur in newer version of Visual Studio even when the console has not been read from or written
        // to yet.
        std::cout.clear();
        std::cerr.clear();
        std::cin.clear();

        std::wcout.clear();
        std::wcerr.clear();
        std::wcin.clear();

        COORD topLeft  = { 0, 0 };
        HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
        CONSOLE_SCREEN_BUFFER_INFO screen;
        DWORD written;

        GetConsoleScreenBufferInfo(console, &screen);
        FillConsoleOutputCharacterA(
            console, ' ', screen.dwSize.X * screen.dwSize.Y, topLeft, &written
        );
        FillConsoleOutputAttribute(
            console, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE,
            screen.dwSize.X * screen.dwSize.Y, topLeft, &written
        );
        SetConsoleCursorPosition(console, topLeft);
    }
}

inline bool ReleaseConsole()
{
    bool result = true;
    FILE* fp;

    // Just to be safe, redirect standard IO to NUL before releasing.

    cout << "END OF OUTPUT, PRESS ENTER" << endl;
    std::cout.flush();

    // Redirect STDIN to NUL
    if (freopen_s(&fp, "NUL:", "r", stdin) != 0)
        result = false;
    else
        setvbuf(stdin, NULL, _IONBF, 0);

    // Redirect STDOUT to NUL
    if (freopen_s(&fp, "NUL:", "w", stdout) != 0)
        result = false;
    else
        setvbuf(stdout, NULL, _IONBF, 0);

    // Redirect STDERR to NUL
    if (freopen_s(&fp, "NUL:", "w", stderr) != 0)
        result = false;
    else
        setvbuf(stderr, NULL, _IONBF, 0);

    // Detach from console
    if (!FreeConsole())
        result = false;

    return result;
}

#endif