// ==WindhawkMod==
// @id              steam-overlay-clock
// @name            Steam Overlay System Clock
// @description     Adds a stylish system clock overlay to Steam games using DirectX11
// @version         1.0
// @author          Hazuki-san
// @github          https://github.com/Hazuki-san/steam-overlay-hook
// @include         *
// @compilerOptions -lversion
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Steam Overlay System Clock

This Windhawk mod automatically injects a stylish system clock overlay into Steam games
that use DirectX 11.

## Features
- Real-time clock display (HH:MM:SS) in cyan color
- Date display with day of week, month, day, and year
- Semi-transparent background positioned in top-right corner
- Borderless, non-intrusive design

## Requirements
- The compiled SteamOverlay.dll must be placed in the mod's directory
- Game must use DirectX 11 rendering
- Steam overlay must be enabled for the game

## Installation
1. Build the SteamOverlay project to generate SteamOverlay.dll
2. Copy SteamOverlay.dll to: %LocalAppData%\Programs\Windhawk\Engine\Mods\steam-overlay-clock\
3. Enable this mod in Windhawk
4. Launch any Steam game with DX11

## Configuration
You can specify a custom DLL path in the settings below.

## Notes
- This mod only works with DirectX 11 games
- May not work with games that have anti-cheat protection
- Use responsibly and only in allowed contexts
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- dllPath: ""
  $name: Custom DLL Path
  $description: >-
    Optional: Specify a custom path to SteamOverlay.dll.
    Leave empty to use the default location in the mod's directory.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <string>
#include <shlobj.h>

struct {
    PCWSTR dllPath;
} settings;

HMODULE g_injectedModule = nullptr;
bool g_attemptedInjection = false;

std::wstring GetModulePath()
{
    WCHAR path[MAX_PATH];
    if (SHGetFolderPathW(nullptr, CSIDL_LOCAL_APPDATA, nullptr, 0, path) == S_OK)
    {
        std::wstring modPath = path;
        modPath += L"\\Programs\\Windhawk\\Engine\\Mods\\steam-overlay-clock\\SteamOverlay.dll";
        return modPath;
    }
    return L"";
}

bool InjectDLL()
{
    if (g_attemptedInjection)
        return g_injectedModule != nullptr;

    g_attemptedInjection = true;

    // Get DLL path
    std::wstring dllPath;
    if (settings.dllPath && settings.dllPath[0] != L'\0')
    {
        dllPath = settings.dllPath;
    }
    else
    {
        dllPath = GetModulePath();
    }

    if (dllPath.empty())
    {
        Wh_Log(L"Failed to determine DLL path");
        return false;
    }

    // Check if file exists
    DWORD fileAttrib = GetFileAttributesW(dllPath.c_str());
    if (fileAttrib == INVALID_FILE_ATTRIBUTES)
    {
        Wh_Log(L"DLL not found at: %s", dllPath.c_str());
        return false;
    }

    // Load the DLL
    g_injectedModule = LoadLibraryW(dllPath.c_str());
    if (!g_injectedModule)
    {
        Wh_Log(L"Failed to load DLL: %s (Error: %d)", dllPath.c_str(), GetLastError());
        return false;
    }

    Wh_Log(L"Successfully injected DLL: %s", dllPath.c_str());
    return true;
}

bool IsGameProcess()
{
    // Check if this process has loaded common game/DirectX DLLs
    HMODULE dxgiModule = GetModuleHandleW(L"dxgi.dll");
    HMODULE d3d11Module = GetModuleHandleW(L"d3d11.dll");

    // Also check for Steam's GameOverlayRenderer
    HMODULE steamOverlay = GetModuleHandleW(L"GameOverlayRenderer64.dll");
    if (!steamOverlay)
        steamOverlay = GetModuleHandleW(L"GameOverlayRenderer.dll");

    // Only inject if we have DX11 and Steam overlay
    return (dxgiModule && d3d11Module && steamOverlay);
}

// Hook a dummy function to trigger injection check
using CreateWindowExW_t = decltype(&CreateWindowExW);
CreateWindowExW_t CreateWindowExW_Original;

HWND WINAPI CreateWindowExW_Hook(
    DWORD dwExStyle,
    LPCWSTR lpClassName,
    LPCWSTR lpWindowName,
    DWORD dwStyle,
    int X,
    int Y,
    int nWidth,
    int nHeight,
    HWND hWndParent,
    HMENU hMenu,
    HINSTANCE hInstance,
    LPVOID lpParam
)
{
    // On first window creation, check if we should inject
    static bool checkedForInjection = false;
    if (!checkedForInjection)
    {
        checkedForInjection = true;

        // Wait a bit for all DLLs to load
        Sleep(500);

        if (IsGameProcess())
        {
            Wh_Log(L"Detected Steam game process, injecting overlay...");
            InjectDLL();
        }
    }

    return CreateWindowExW_Original(dwExStyle, lpClassName, lpWindowName, dwStyle,
                                    X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
}

// Windhawk mod callbacks
BOOL Wh_ModInit()
{
    Wh_Log(L"Steam Overlay Clock mod initialized");

    // Hook CreateWindowExW to detect when the game is setting up
    Wh_SetFunctionHook((void*)CreateWindowExW, (void*)CreateWindowExW_Hook,
                       (void**)&CreateWindowExW_Original);

    return TRUE;
}

void Wh_ModUninit()
{
    Wh_Log(L"Steam Overlay Clock mod uninitialized");

    if (g_injectedModule)
    {
        FreeLibrary(g_injectedModule);
        g_injectedModule = nullptr;
    }
}

void Wh_ModSettingsChanged()
{
    Wh_Log(L"Settings changed");

    // Reset injection attempt to allow retry with new settings
    g_attemptedInjection = false;

    if (g_injectedModule)
    {
        FreeLibrary(g_injectedModule);
        g_injectedModule = nullptr;
    }
}

BOOL Wh_ModSettingsStringSet(PCWSTR name, PCWSTR value)
{
    if (wcscmp(name, L"dllPath") == 0)
    {
        settings.dllPath = value;
        return TRUE;
    }
    return FALSE;
}
