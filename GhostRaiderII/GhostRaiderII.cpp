#include <cstdio>
#include <iostream>
#include <cstdint>
#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sstream>
#include <thread>
#include "console.h"
#include "DataStruct.h"
#include "MinHookWrapper.h"
#include "Offsets.h"
#include "Serializer.h"

// Data types for function hook
using load_level_t = void* (*)(int32_t, int32_t);
using render_lara_t = int32_t(*)(Entity*);

// Pointer for calling original Tomb Raider II calls.
load_level_t loadLevel = NULL;
render_lara_t renderLara = NULL;

constexpr uintptr_t loadLevelFuncAddr = Offsets::GOG_UK::loadLevelFunc;
constexpr uintptr_t renderLaraFuncAddr = Offsets::GOG_UK::renderLaraFunc;

Serializer serializer;

int32_t hk_render_lara_reproduce(Entity* lara) {
    // Render original
    int32_t res = renderLara(lara);
    // Render ghost
    struct Entity* newLara = (Entity*)malloc(sizeof(Entity));
    if (serializer.read(newLara, lara)) {
        renderLara(newLara);
    }
    return res;
}

int32_t hk_render_lara_write(Entity* lara) {
    // Render original
    int32_t res = renderLara(lara);
    // Store data
    serializer.serialize(lara);
    return res;
}

void* hk_load_level(int32_t level_id, int32_t a2) {
    println(color::cyan, "Level %d - %d loaded", level_id, a2);
    if (serializer.isInit()) {
        serializer.close();
    }
    serializer.init(level_id);
    // patch memory
    if (!unhook_fn(renderLaraFuncAddr)) {
        println(color::red, "Failed to unhook renderLara");
    }
    if (serializer.isWrite()) {
        if (!hook_fn(hk_render_lara_write, renderLaraFuncAddr, &renderLara)) {
            println(color::red, "Failed to hook renderLara");
        }
    }
    else {
        if (!hook_fn(hk_render_lara_reproduce, renderLaraFuncAddr, &renderLara)) {
            println(color::red, "Failed to hook renderLara");
        }
    }
    if (serializer.isRead()) {
        serializer.open_read();
    }
    return loadLevel(level_id, a2);
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
        println(color::red, "Failed to hook loadLevel");
    }

    // Default hook
    if (!hook_fn(hk_render_lara_reproduce, renderLaraFuncAddr, &renderLara)) {
        println(color::red, "Failed to hook renderLara");
    } 

    DWORD* BaseAddress = (DWORD*)GetModuleHandle(NULL);
    DWORD* isMenu = (DWORD*)((char*)BaseAddress + Offsets::GOG_UK::title);
    while (true) {
        serializer.is_menu(*isMenu);
        std::this_thread::sleep_for(std::chrono::microseconds(7812));
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
