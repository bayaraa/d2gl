# Diablo 2 LoD Glide/DDraw to OpenGL Wrapper (D2GL)

D2GL is a Diablo 2 LoD Glide/DDraw to OpenGL wrapper that translates glide/ddraw calls to OpenGL calls. This makes D2 run better on modern hardware/OS and easier to switch between fullscreen and windowed modes, and resolves certain window and cursor-related issues. Additionally, it includes several libretro shaders for upscaling to higher resolutions, and allows for in-game text to be displayed in high-definition, D2DX's motion prediction, other QOL features.

### Bug/Crash Report (***Important!***)

Please don't just say it crashes. I can't do anything without additional information!  
If you want to report bug or crash please include details about your system (OS, hardware), what you doing when crash occured, are other mods included (like plugy, basemod etc), is crashing on both wrappers (glide, ddraw).  
Also run game.exe with `-3dfx -log` it will create `d2gl.log` and attach this along with diablo2's log file like this `D2230410.txt`. It helps investigation.

## Features

- In-Game Option Menu(`Ctrl+O`) to change settings on fly.
- Upscale to higher resolution using pre-included libretro's shader.
- Switch some parts in game to high-definition (all text, cursor etc.).
- Unlocked FPS (menu screen 45fps / ingame unlocked).
- Modified version of "D2DX's Unit/Weather Motion Predictor" feature (better with v-sync on).
- Few graphic post processing effects (Lut, sharpen, FXAA etc.).
- Fast switching windowed to fullscreen with `Alt+Enter` key.

## Requirements

- Diablo 2 LoD: ``1.09d``, ``1.10f``, ``1.11``, ``1.11b``, ``1.12a``, ``1.13c``, ``1.13d``, ``1.14d``.
  - 1.14d dropped software rendering. So don't put ddraw.dll into game folder.
- Windows 10/11, Linux with Wine(Proton, Lutris etc), MacOS with Wine(Crossover).
- GPU with minimum OpenGL 3.3 support.

## Installation

See [Installation](https://github.com/bayaraa/d2gl/wiki/Installation).

## Configuration

See [Configuration](https://github.com/bayaraa/d2gl/wiki/Configuration).

## Compatibility

See [Compatibility](https://github.com/bayaraa/d2gl/wiki/Compatibility).

## Credits

Diablo II modding community (The Phrozen Keep) and **Everyone** who makes d2 mod and their source codes available.

- SGD2FreeRes D2 Custom Resolution (Mir Drualga).
- D2DX's Unit/Weather Motion Predictor (Bolrog).
- Libretro's common-shaders.
- Detours (Microsoft).
- The OpenGL Extension Wrangler Library (Brian Paul).
- OpenGL Mathematics (GLM) (G-Truc Creation).
- FXAA implementation by Timothy Lottes (NVIDIA Corporation).
- Dear ImGui (Omar Cornut).
- stb_image, stb_image_write (Sean Barrett).
- MurmurHash3 (Austin Appleby).
- MSDF Atlas Generator (Chlumsky).
- Shader Minifier (laurentlb).

## Some Screenshots

![Screenshot010](https://user-images.githubusercontent.com/2043880/220664490-2a9b34d8-ca7c-4e52-a57d-d43c508f5813.png)
![Screenshot012](https://user-images.githubusercontent.com/2043880/220668775-3351be3b-27fa-4800-883f-09e5eb935c47.png)
![Screenshot013](https://user-images.githubusercontent.com/2043880/220666692-967a8c13-f480-4ac6-af1e-b45fda3bdee3.png)
![Screenshot002](https://user-images.githubusercontent.com/2043880/220667272-a83aa2cd-d038-41ea-a878-a6e148b8f9f6.png)
