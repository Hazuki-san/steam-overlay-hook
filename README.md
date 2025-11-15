### steam-overlay-hook
Hijack Steam's GameOverlayRenderer to render with DirectX11.

Displays a stylish system clock overlay with real-time updates in the top-right corner of Steam games.

## Features
- Real-time clock display (HH:MM:SS) in cyan color with 2x font scale
- Date display showing day of week, month, day, and year in light gray
- Semi-transparent background (75% opacity)
- Borderless, auto-resizing window positioned in top-right corner
- Works with any DirectX 11 game (both 32-bit and 64-bit)
- Automatic architecture detection when using Windhawk

## Installation

### Method 1: Windhawk (Recommended - Automatic Injection)

1. **Install Windhawk**
   - Download from [windhawk.net](https://windhawk.net/)
   - Install and run Windhawk

2. **Build the DLL for both architectures**
   - Open `SteamOverlay/SteamOverlay.sln` in Visual Studio
   - Build for **x64**:
     - Set configuration to **Release** and platform to **x64**
     - Build the solution
     - Find the DLL in `SteamOverlay/x64/Release/SteamOverlay.dll`
   - Build for **x86** (32-bit):
     - Set platform to **x86** (or Win32)
     - Build the solution
     - Find the DLL in `SteamOverlay/Release/SteamOverlay.dll` or `SteamOverlay/x86/Release/SteamOverlay.dll`

3. **Install the Windhawk Mod**
   - Copy `steam-overlay-clock.wh.cpp` to Windhawk's mod directory, or
   - Use Windhawk's "Install from file" option and select `steam-overlay-clock.wh.cpp`

4. **Deploy the DLLs**
   Create the directory structure and copy both DLLs:
   ```
   %LocalAppData%\Programs\Windhawk\Engine\Mods\steam-overlay-clock\x64\SteamOverlay.dll
   %LocalAppData%\Programs\Windhawk\Engine\Mods\steam-overlay-clock\x86\SteamOverlay.dll
   ```
   - Alternatively, specify custom paths for each architecture in the Windhawk mod settings

5. **Enable the Mod**
   - Enable "Steam Overlay System Clock" in Windhawk
   - Launch any Steam game with DirectX 11
   - The mod will automatically load the correct DLL (x86 or x64) based on the game's architecture

### Method 2: Manual DLL Injection

1. **Build the DLL** (same as above)

2. **Use a DLL Injector**
   - Use tools like Extreme Injector, Xenos Injector, or Process Hacker
   - Target the game process after it launches
   - Inject `SteamOverlay.dll`

## Requirements
- Windows 10/11
- Visual Studio 2019 or later (for building)
- DirectX 11 game
- Steam overlay enabled for the game
- For Windhawk method: [Windhawk](https://windhawk.net/)

## Important Notes
- This only works with DirectX 11 games
- Anti-cheat software may detect and block DLL injection
- Use only in single-player games or where modifications are allowed
- Steam's VAC (Valve Anti-Cheat) may flag injected DLLs in protected games
- Use at your own risk

## Building from Source

### For 64-bit (x64) games:
1. Open `SteamOverlay/SteamOverlay.sln` in Visual Studio
2. Ensure all dependencies are installed (Windows SDK, DirectX SDK)
3. Set configuration to **Release** and platform to **x64**
4. Build the solution
5. The output DLL will be in `SteamOverlay/x64/Release/SteamOverlay.dll`

### For 32-bit (x86) games:
1. Open the same solution in Visual Studio
2. Set configuration to **Release** and platform to **x86** (or Win32)
3. Build the solution
4. The output DLL will be in `SteamOverlay/Release/SteamOverlay.dll` or `SteamOverlay/x86/Release/SteamOverlay.dll`

**Note:** Most modern games use 64-bit, but older games may require the 32-bit version. Build both for maximum compatibility.

## Customization
You can customize the clock appearance by modifying `Hooks.cpp`:
- Change colors in the `ImVec4` color values
- Adjust position by modifying `window_pos` calculations
- Modify transparency with `SetNextWindowBgAlpha()`
- Change font scale with `SetWindowFontScale()`
