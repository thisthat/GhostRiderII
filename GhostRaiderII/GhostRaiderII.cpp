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

DWORD *baseAddress;
uintptr_t loadLevelFuncAddr;
uintptr_t renderLaraFuncAddr;
uintptr_t titleScreen;

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

    // Resolve offsets
    baseAddress = (DWORD*)GetModuleHandle(NULL);
    if (Offsets::isMulti(baseAddress)) {
        println(color::dark_green, "Identified Multipatch");
        loadLevelFuncAddr = Offsets::MULTI::loadLevelFunc;
        renderLaraFuncAddr = Offsets::MULTI::renderLaraFunc;
        titleScreen = Offsets::MULTI::title;
    }
    else if (Offsets::isEPC(baseAddress)) {
        println(color::dark_green, "Identified Eidos Premier Collection");
        loadLevelFuncAddr = Offsets::EPC::loadLevelFunc;
        renderLaraFuncAddr = Offsets::EPC::renderLaraFunc;
        titleScreen = Offsets::EPC::title;
    }
    else if (Offsets::isUK(baseAddress)) {
        println(color::dark_green, "Identified UK Box"); 
        loadLevelFuncAddr = Offsets::UK::loadLevelFunc;
        renderLaraFuncAddr = Offsets::UK::renderLaraFunc;
        titleScreen = Offsets::UK::title;
    }
    else if (Offsets::isPatched(baseAddress)) {
        println(color::dark_green, "Identified CORE's Patch 1");
        loadLevelFuncAddr = Offsets::PATCH::loadLevelFunc;
        renderLaraFuncAddr = Offsets::PATCH::renderLaraFunc;
        titleScreen = Offsets::PATCH::title;
    }
    else {
        println(color::red, "Something is off");
        return 2;
    }

    // Create a hook for MessageBoxW, in disabled state.
    if (!hook_fn(hk_load_level, loadLevelFuncAddr, &loadLevel)) {
        println(color::red, "Failed to hook loadLevel");
    }

    // Default hook
    if (!hook_fn(hk_render_lara_reproduce, renderLaraFuncAddr, &renderLara)) {
        println(color::red, "Failed to hook renderLara");
    } 

    
    DWORD* isMenu = (DWORD*)((char*)baseAddress + titleScreen);
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
