#include <cstdio>
#include <iostream>
#include <cstdint>
#include <Windows.h>
#include <string>
#include <sstream>
#include "console.h"
#include "DataStruct.h"
#include "MinHook.h"

// Data types for function hook
using load_level_t = void* (*)(int32_t, int32_t);
using render_lara_t = int32_t(*)(Entity*);

// Pointer for calling original Tomb Raider II calls.
load_level_t loadLevel = NULL;
render_lara_t renderLara = NULL;

constexpr uintptr_t loadLevelFuncAddr = 0x43C790;
constexpr uintptr_t renderLaraFuncAddr = 0x419DD0;

bool init() { return (MH_Initialize() == MH_OK); }
bool destroy() { return (MH_Uninitialize() == MH_OK); }

template <typename T, typename F>
bool hook_fn(T trampoline, uintptr_t target, F* o_fn)
{
    if (MH_CreateHook((void*)target, (void*)trampoline, (void**)o_fn) != MH_OK)
        return false;

    return (MH_EnableHook((void*)target) == MH_OK);
}

bool unhook_fn(uintptr_t target)
{
    return (MH_RemoveHook((void*)target) == MH_OK);
}

int i;

void* hk_load_level(int32_t a1, int32_t a2) {
    println(color::cyan, "Level loaded");
    i = 0;
    return loadLevel(a1, a2);
}

int32_t hk_render_lara(Entity* lara) {
    std::ostringstream os;
    // Print debug info every other 100 frames
    if (i % 100 == 0) {
        os << "Position: " << lara->position.x << ", " << lara->position.y << ", " << lara->position.z << " \n";
        println(color::red, os.str().c_str());
    }
    // Render original
    int32_t res = renderLara(lara);

    // Render Ghost
    struct Entity* newLara = (Entity*)malloc(sizeof(Entity));
    newLara->floor_y = lara->floor_y;
    newLara->mesh_bits = lara->mesh_bits;
    newLara->model = lara->model;
    newLara->anim_id = 0;
    newLara->anim_frame = 0;
    newLara->room_id = lara->room_id;
    newLara->room_unk_data = lara->room_unk_data;
    newLara->interactive_id = lara->interactive_id;
    newLara->xz_velocity = lara->xz_velocity;
    newLara->y_velocity = lara->y_velocity;
    newLara->health = lara->health;
    newLara->activation_flags = 0;
    newLara->intensity = 0;
    newLara->pad3 = lara->pad3;
    newLara->parent = lara->parent;
    newLara->ai_manager = lara->ai_manager;
    newLara->position = ivec3(lara->position.x + 500, lara->position.y, lara->position.z - i++ * 5);
    newLara->rotation = small_ivec3(lara->rotation.x, lara->rotation.y, lara->rotation.z);
    newLara->flags = 0;

    if (i % 100 == 0) {
        os << "Second Position: " << newLara->position.x << ", " << newLara->position.y << ", " << newLara->position.z << " \n";
        println(color::red, os.str().c_str());
    }
    return renderLara(newLara);
}

inline const wchar_t* ToString(MH_STATUS v)
{
    switch (v)
    {
    case MH_UNKNOWN:   return L"MH_UNKNOWN";
    case MH_OK:   return L"MH_OK";
    case MH_ERROR_ALREADY_INITIALIZED: return L"MH_ERROR_ALREADY_INITIALIZED";
    case MH_ERROR_NOT_INITIALIZED:   return L"MH_ERROR_NOT_INITIALIZED";
    case MH_ERROR_ALREADY_CREATED:   return L"MH_ERROR_ALREADY_CREATED";
    case MH_ERROR_NOT_CREATED:   return L"MH_ERROR_NOT_CREATED";
    case MH_ERROR_ENABLED:   return L"MH_ERROR_ENABLED";
    case MH_ERROR_DISABLED:   return L"MH_ERROR_DISABLED";
    case MH_ERROR_NOT_EXECUTABLE:   return L"MH_ERROR_NOT_EXECUTABLE";
    case MH_ERROR_UNSUPPORTED_FUNCTION:   return L"MH_ERROR_UNSUPPORTED_FUNCTION";
    case MH_ERROR_MEMORY_ALLOC:   return L"MH_ERROR_MEMORY_ALLOC";
    case MH_ERROR_MEMORY_PROTECT:   return L"MH_ERROR_MEMORY_PROTECT";
    case MH_ERROR_MODULE_NOT_FOUND:   return L"MH_ERROR_MODULE_NOT_FOUND";
    case MH_ERROR_FUNCTION_NOT_FOUND:   return L"MH_ERROR_FUNCTION_NOT_FOUND";
    default:      return L"[Unknown]";
    }
}

int main_th()
{
    console::init();
    // Initialize MinHook.
    if (!init())
    {
        MessageBoxW(NULL, L"Failed to init.", L"MinHook Sample", MB_OK);
        return 1;
    }

    // Create a hook for MessageBoxW, in disabled state.
    if (!hook_fn(hk_load_level, loadLevelFuncAddr, &loadLevel)) {
        MessageBoxW(NULL, L"Failed to hook.", L"MinHook Sample", MB_OK);
    }

    if (!hook_fn(hk_render_lara, renderLaraFuncAddr, &renderLara)) {
        MessageBoxW(NULL, L"Failed to render hook.", L"MinHook Sample", MB_OK);
    }
    else {
        MessageBoxW(NULL, L"Hooked to hook.", L"MinHook Sample", MB_OK);
    }

    return 0;
}

BOOL __stdcall DllMain(HINSTANCE dll, DWORD reason, LPVOID reserved)
{
    if (reason == DLL_PROCESS_ATTACH)
    {
        CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)main_th, nullptr, 0, nullptr);
    }

    return true;
}
