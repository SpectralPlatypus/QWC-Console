#include <windows.h>
#include <d3d8.h>
#include "Direct3D8Wrapper.h"
#include "Direct3DDevice8Wrapper.h"

// Source:
// https://github.com/ThirteenAG/d3d8-wrapper

struct d3d8_dll
{
    HMODULE dll;
    FARPROC Direct3DCreate8;
} d3d8;

__declspec(naked) void _Direct3DCreate8() { _asm { jmp[d3d8.Direct3DCreate8] } }

IDirect3D8*	(WINAPI *OriginalDirect3DCreate8) (UINT SDKVersion);

IDirect3D8* WINAPI Direct3DCreate8Callback(UINT SDKVersion)
{
    IDirect3D8* Direct3D = OriginalDirect3DCreate8(SDKVersion);
    IDirect3D8* WrappedDirect3D = new Direct3D8Wrapper(Direct3D);
   return WrappedDirect3D;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    char path[MAX_PATH];
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    {
        CopyMemory(path + GetSystemDirectory(path, MAX_PATH - 9), "\\d3d8.dll", 10);
        d3d8.dll = LoadLibrary(path);
        if (d3d8.dll == false)
        {
            ExitProcess(0);
        }

        d3d8.Direct3DCreate8 = GetProcAddress(d3d8.dll, "Direct3DCreate8");

        //wrapping Direct3DCreate8
        OriginalDirect3DCreate8 = (IDirect3D8 *(__stdcall *)(UINT))GetProcAddress(d3d8.dll, "Direct3DCreate8");
        d3d8.Direct3DCreate8 = (FARPROC)Direct3DCreate8Callback;
    }
    break;

    case DLL_PROCESS_DETACH:
        FreeLibrary(d3d8.dll);
        break;
    }
    return TRUE;
}