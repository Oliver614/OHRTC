#pragma once

#include <cstdint>
#include <codecvt>
#include <exception>
#include <iostream>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h> // For HRESULT
#include <comdef.h> // For _com_error class (used to decode HR result codes).
#include <corecrt_io.h>
#include <fcntl.h>

#include <d3d12.h>


// From DXSampleHelper.h 
// Source: https://github.com/Microsoft/DirectX-Graphics-Samples

inline std::string HrToString(HRESULT hr)
{
    char s_str[64] = {};
    sprintf_s(s_str, "HRESULT of 0x%08X", static_cast<UINT>(hr));
    return std::string(s_str);
}

class HrException : public std::runtime_error
{
public:
    HrException(HRESULT hr) : std::runtime_error(HrToString(hr)), m_hr(hr) {}
    HRESULT Error() const { return m_hr; }
private:
    const HRESULT m_hr;
};

inline void ThrowIfFailed(HRESULT hr)
{
    if (FAILED(hr))
    {
        throw HrException(hr);
    }
}

// Create a console in debug mode.

constexpr int MAX_CONSOLE_LINES = 500;

static void CreateConsole()
{
    // Allocate a console.
    if (AllocConsole())
    {
        HANDLE lStdHandle = GetStdHandle(STD_OUTPUT_HANDLE);

        // Increase screen buffer to allow more lines of text than the default.
        CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
        GetConsoleScreenBufferInfo(lStdHandle, &consoleInfo);
        consoleInfo.dwSize.Y = MAX_CONSOLE_LINES;
        SetConsoleScreenBufferSize(lStdHandle, consoleInfo.dwSize);
        SetConsoleCursorPosition(lStdHandle, { 0, 0 });

        // Redirect unbuffered STDOUT to the console.
        int   hConHandle = _open_osfhandle((intptr_t)lStdHandle, _O_TEXT);
        FILE* fp = _fdopen(hConHandle, "w");
        freopen_s(&fp, "CONOUT$", "w", stdout);
        setvbuf(stdout, nullptr, _IONBF, 0);

        // Redirect unbuffered STDIN to the console.
        lStdHandle = GetStdHandle(STD_INPUT_HANDLE);
        hConHandle = _open_osfhandle((intptr_t)lStdHandle, _O_TEXT);
        fp = _fdopen(hConHandle, "r");
        freopen_s(&fp, "CONIN$", "r", stdin);
        setvbuf(stdin, nullptr, _IONBF, 0);

        // Redirect unbuffered STDERR to the console.
        lStdHandle = GetStdHandle(STD_ERROR_HANDLE);
        hConHandle = _open_osfhandle((intptr_t)lStdHandle, _O_TEXT);
        fp = _fdopen(hConHandle, "w");
        freopen_s(&fp, "CONOUT$", "w", stderr);
        setvbuf(stderr, nullptr, _IONBF, 0);

        // Clear the error state for each of the C++ standard stream objects. We
        // need to do this, as attempts to access the standard streams before
        // they refer to a valid target will cause the iostream objects to enter
        // an error state. In versions of Visual Studio after 2005, this seems
        // to always occur during startup regardless of whether anything has
        // been read from or written to the console or not.
        std::wcout.clear();
        std::cout.clear();
        std::wcerr.clear();
        std::cerr.clear();
        std::wcin.clear();
        std::cin.clear();
    }
}

namespace Utility
{
#ifdef _CONSOLE
    inline void Print(const char* msg) { printf("%s", msg); }
    inline void Print(const wchar_t* msg) { wprintf(L"%ws", msg); }
#else
    inline void Print(const char* msg) { OutputDebugStringA(msg); }
    inline void Print(const wchar_t* msg) { OutputDebugString(msg); }
#endif

    inline void Printf(const char* format, ...)
    {
        char buffer[256];
        va_list ap;
        va_start(ap, format);
        vsprintf_s(buffer, 256, format, ap);
        va_end(ap);
        Print(buffer);
    }

    inline void Printf(const wchar_t* format, ...)
    {
        wchar_t buffer[256];
        va_list ap;
        va_start(ap, format);
        vswprintf(buffer, 256, format, ap);
        va_end(ap);
        Print(buffer);
    }

#ifndef RELEASE
    inline void PrintSubMessage(const char* format, ...)
    {
        Print("--> ");
        char buffer[256];
        va_list ap;
        va_start(ap, format);
        vsprintf_s(buffer, 256, format, ap);
        va_end(ap);
        Print(buffer);
        Print("\n");
    }
    inline void PrintSubMessage(const wchar_t* format, ...)
    {
        Print("--> ");
        wchar_t buffer[256];
        va_list ap;
        va_start(ap, format);
        vswprintf(buffer, 256, format, ap);
        va_end(ap);
        Print(buffer);
        Print("\n");
    }
    inline void PrintSubMessage(void)
    {
    }
#endif
} // namespace Utility

#define STRINGIFY(x) #x
#define STRINGIFY_BUILTIN(x) STRINGIFY(x)
#define ASSERT( isFalse, ... ) \
        if (!(bool)(isFalse)) { \
            Utility::Print("\nAssertion failed in " STRINGIFY_BUILTIN(__FILE__) " @ " STRINGIFY_BUILTIN(__LINE__) "\n"); \
            Utility::PrintSubMessage("\'" #isFalse "\' is false"); \
            Utility::PrintSubMessage(__VA_ARGS__); \
            Utility::Print("\n"); \
            __debugbreak(); \
        }

#define ASSERT_SUCCEEDED( hr, ... ) \
        if (FAILED(hr)) { \
            Utility::Print("\nHRESULT failed in " STRINGIFY_BUILTIN(__FILE__) " @ " STRINGIFY_BUILTIN(__LINE__) "\n"); \
            Utility::PrintSubMessage("hr = 0x%08X", hr); \
            Utility::PrintSubMessage(__VA_ARGS__); \
            Utility::Print("\n"); \
            __debugbreak(); \
        }
