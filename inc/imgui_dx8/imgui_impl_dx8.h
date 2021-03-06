#pragma once
#include <imgui.h>
// ImGui Win32 + DirectX8 binding
// In this binding, ImTextureID is used to store a 'LPDIRECT3DTEXTURE8' texture identifier. Read the FAQ about ImTextureID in imgui.cpp.

// You can copy and use unmodified imgui_impl_* files in your project. See main.cpp for an example of using this.
// If you use this binding you'll need to call 4 functions: ImGui_ImplXXXX_Init(), ImGui_ImplXXXX_NewFrame(), ImGui::Render() and ImGui_ImplXXXX_Shutdown().
// If you are new to ImGui, see examples/README.txt and documentation at the top of imgui.cpp.
// https://github.com/ocornut/imgui

struct IDirect3DDevice8;

IMGUI_API bool        ImGui_ImplDX8_Init(void* hwnd, IDirect3DDevice8* device);
IMGUI_API void        ImGui_ImplDX8_Shutdown();
IMGUI_API void        ImGui_ImplDX8_NewFrame();

// Use if you want to reset your rendering device without losing ImGui state.
IMGUI_API void        ImGui_ImplDX8_InvalidateDeviceObjects();
IMGUI_API bool        ImGui_ImplDX8_CreateDeviceObjects();