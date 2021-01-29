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


std::string dll_base_path;
unsigned long long int nFrame = 0;

Serializer serializer;

void* hk_load_level(int32_t a1, int32_t a2) {
    println(color::cyan, "Level %d - %d loaded", a1, a2);
    nFrame = 0;
    return loadLevel(a1, a2);
}

int32_t hk_render_lara(Entity* lara) {
    // Render original
    nFrame++;
    int32_t res = renderLara(lara);
    serializer.serialize(lara, nFrame);
    // Render Ghost
    //struct Entity* newLara = (Entity*)malloc(sizeof(Entity));
    //newLara->floor_y = lara->floor_y;
    //newLara->mesh_bits = lara->mesh_bits;
    //newLara->model = lara->model;
    //newLara->anim_id = 0;
    //newLara->anim_frame = 0;
    //newLara->room_id = lara->room_id;
    //newLara->room_unk_data = lara->room_unk_data;
    //newLara->interactive_id = lara->interactive_id;
    //newLara->xz_velocity = lara->xz_velocity;
    //newLara->y_velocity = lara->y_velocity;
    //newLara->health = lara->health;
    //newLara->activation_flags = 0;
    //newLara->intensity = 0;
    //newLara->pad3 = lara->pad3;
    //newLara->parent = lara->parent;
    //newLara->ai_manager = lara->ai_manager;
    //newLara->position = ivec3(lara->position.x, lara->position.y, lara->position.z);
    //newLara->rotation = small_ivec3(lara->rotation.x, lara->rotation.y, lara->rotation.z);
    //newLara->flags = 0;
    return res;
    //return renderLara(newLara);
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

    char path[_MAX_PATH + 1];
    GetModuleFileNameA(NULL, path, _MAX_PATH);
    std::string str = std::string(path);
    dll_base_path = str.substr(0, str.find_last_of("\\/") + 1);
    println(color::blue, dll_base_path.c_str());

    // Create a hook for MessageBoxW, in disabled state.
    if (!hook_fn(hk_load_level, loadLevelFuncAddr, &loadLevel)) {
        println(color::red, "Failed to hook loadLevel");
    }

    if (!hook_fn(hk_render_lara, renderLaraFuncAddr, &renderLara)) {
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
