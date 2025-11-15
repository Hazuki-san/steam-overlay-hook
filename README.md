### steam-overlay-hook
Hijack Steam's GameOverlayRenderer to render with DirectX11.

Displays a stylish system clock overlay with real-time updates in the top-right corner of Steam games.

## Features
- Real-time clock display (HH:MM:SS) in cyan color with 2x font scale
- Date display showing day of week, month, day, and year in light gray
- Semi-transparent background (75% opacity)
- Borderless, auto-resizing window positioned in top-right corner
- Works with any DirectX 11 game

## Installation

### Method 1: Windhawk (Recommended - Automatic Injection)

1. **Install Windhawk**
   - Download from [windhawk.net](https://windhawk.net/)
   - Install and run Windhawk

2. **Build the DLL**
   - Open `SteamOverlay/SteamOverlay.sln` in Visual Studio
   - Set configuration to **Release** and platform to **x64** (for 64-bit games)
   - Build the solution
   - Find `SteamOverlay.dll` in the output directory

3. **Install the Windhawk Mod**
   - Copy `steam-overlay-clock.wh.cpp` to Windhawk's mod directory, or
   - Use Windhawk's "Install from file" option and select `steam-overlay-clock.wh.cpp`

4. **Deploy the DLL**
   - Copy `SteamOverlay.dll` to: `%LocalAppData%\Programs\Windhawk\Engine\Mods\steam-overlay-clock\`
   - Alternatively, specify a custom path in the Windhawk mod settings

5. **Enable the Mod**
   - Enable "Steam Overlay System Clock" in Windhawk
   - Launch any Steam game with DirectX 11

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
1. Open `SteamOverlay/SteamOverlay.sln` in Visual Studio
2. Ensure all dependencies are installed (Windows SDK, DirectX SDK)
3. Build in Release mode for x64 architecture
4. The output DLL will be in `SteamOverlay/x64/Release/SteamOverlay.dll`

## Customization
You can customize the clock appearance by modifying `Hooks.cpp`:
- Change colors in the `ImVec4` color values
- Adjust position by modifying `window_pos` calculations
- Modify transparency with `SetNextWindowBgAlpha()`
- Change font scale with `SetWindowFontScale()`
