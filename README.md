# Diablo 2 LoD Glide/DDraw to OpenGL Wrapper (D2GL)

Makes Diablo 2's glide/ddraw graphic to modern opengl title. Easier to switch between fullscreen and windowed, fixes some window and cursor related issues.
Few pre included libretro's shader to upscale higher resolutions. Change in game text to high-definition fonts etc.

## Features

- Turns Diablo 2 to modern opengl title.
- Upscale to higher resolution using libretro's shader.
- Switch some parts in game to high-definition (all text, cursor etc.).
- Unlocked FPS (menu screen / ingame).
- Modified version of "D2DX's Unit/Weather Motion Predictor" feature.
- Few graphic post processing effects (sharpen, FXAA etc.).

## Requirements

- Diablo 2 LoD: 1.09d, 1.10f, 1.11, 1.11b, 1.12a, 1.13c, 1.13d, 1.14d.
  - 1.14d dropped software rendering. So don't put ddraw.dll into game folder.
- Windows 10/11
- GPU with minimum OpenGL 3.3 support.

## Usage

Put d2gl.mpq and glide3x.dll or ddraw.dll (or both) into game installation folder.  
Disable all compatibilty mode if you enabled on Game.exe and Diablo II.exe.  
Run Game.exe without any flag will open game in DDraw mode (except 1.14d).  
Run Game.exe with -3dfx flag will open game in Glide mode.
