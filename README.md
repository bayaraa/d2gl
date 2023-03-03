# Diablo 2 LoD Glide/DDraw to OpenGL Wrapper (D2GL)

Makes Diablo 2's wrap glide/ddraw graphic to opengl calls. Easier to switch between fullscreen and windowed, fixes some window and cursor related issues.
Few pre included libretro's shader to upscale higher resolutions. Change in game text to high-definition text etc.

## Features

- Turns Diablo 2 to modern opengl title.
- Upscale to higher resolution using some libretro's shader.
- Switch some parts in game to high-definition (all text, cursor etc.).
- Unlocked FPS (menu screen 45fps / ingame).
- Modified version of "D2DX's Unit/Weather Motion Predictor" feature (better with v-sync on).
- Few graphic post processing effects (Lut, sharpen, FXAA etc.).

## Requirements

- Diablo 2 LoD: ``1.09d``, ``1.10f``, ``1.11``, ``1.11b``, ``1.12a``, ``1.13c``, ``1.13d``, ``1.14d``.
  - 1.14d dropped software rendering. So don't put ddraw.dll into game folder.
- Windows 10/11
- GPU with minimum OpenGL 3.3 support.
  Currently i don't have access to NVidia or AMD or any dedicated GPU. Tested only Intel 550 iGPU but fps seems ok.

## Usage

Don't use custom wrapper on official servers (like battle.net) you will be banned.  
Put d2gl.mpq and glide3x.dll or ddraw.dll (or both) into game installation folder.  
Disable all compatibilty mode if you enabled on Game.exe and Diablo II.exe.  
Run Game.exe without any flag will open game in DDraw mode (except 1.14d).  
Run Game.exe with ``-3dfx`` flag will open game in Glide mode (don't use ``-w`` flag).  
``CTRL+O`` ("O" as a letter) brings option menu and almost every setting can be changed directly ingame.

If you want to play in widescreen (with SGD2FreeRes) custom SGD2FreeRes is included in zip file.  
If you want to play PD2 with this wrapper disable PD2's default hd mod D2HD.dll (rename or delete) and open d2gl.ini and set
``pd2_fix=true``. It will fix some glide related crashes (Not fully tested but Rathma, DClone, some maps tested).

To load custom dll there are 2 settings in d2gl.ini ``load_dlls_early``, ``load_dlls_late``.  
``load_dlls_early`` is loads right after wrapper attached.  
``load_dlls_late`` is loads right after game window created (if you play PD2 use ``load_dlls_late=SGD2FreeRes.dll``).

## Compatibility

I did not really tested with other mods.
Briefly tested on D2LoD 1.13c on single player with Basemod(1.1.3.9), BH Maphack(1.9.9), Plugy(14.0.3).
Briefly tested on Project Diablo 2 Season 6 single player with/without plugy.

## Credits

Diablo II modding community (The Phrozen Keep) and Everyone who makes d2 mod and their source codes available.

- SGD2FreeRes D2 Custom HD resolution mod (Mir Drualga).
- D2DX's Unit/Weather Motion Predictor (Bolrog).
- Libretro's common-shaders.
- Detours (Microsoft).
- The OpenGL Extension Wrangler Library (Brian Paul).
- OpenGL Mathematics (GLM) (G-Truc Creation).
- FXAA implementation by Timothy Lottes (NVIDIA Corporation).
- Dear ImGui (Omar Cornut).
- stb_image, stb_image_write (Sean Barrett).
- MurmurHash3 (Austin Appleby).

## Some Screenshots

![Screenshot010](https://user-images.githubusercontent.com/2043880/220664490-2a9b34d8-ca7c-4e52-a57d-d43c508f5813.png)
![Screenshot012](https://user-images.githubusercontent.com/2043880/220668775-3351be3b-27fa-4800-883f-09e5eb935c47.png)
![Screenshot013](https://user-images.githubusercontent.com/2043880/220666692-967a8c13-f480-4ac6-af1e-b45fda3bdee3.png)
![Screenshot002](https://user-images.githubusercontent.com/2043880/220667272-a83aa2cd-d038-41ea-a878-a6e148b8f9f6.png)
