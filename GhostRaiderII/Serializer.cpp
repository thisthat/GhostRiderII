#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "console.h"
#include "Serializer.h"

// write in memory


Serializer::Serializer() {
    laras.reserve(MAX_LARAS);
}

void Serializer::open_read() {
    FILE* file;
    fopen_s(&file, "F:\\obs\\output.tr2", "rb");
    _index = 0;
    long elm;
    fread((char*)&elm, sizeof(elm), 1, file);
    laras.resize(elm);
    long long int size = elm * (long long int) sizeof(Ghost);
    fread(&laras[0], 1, size, file);
}

bool Serializer::read(Entity* newLara, Entity* lara) {
    if (_index >= laras.size()) {
        return false;
    }
    Ghost g = laras.at(_index++);
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
    newLara->position = ivec3(g.position.x, g.position.y, g.position.z);
    newLara->rotation = small_ivec3(g.rotation.x, g.rotation.y, g.rotation.z);
    newLara->flags = 0;
    return true;
}

void Serializer::close() {
    long elm = laras.size();
    long long int size = elm * (long long int) sizeof(Ghost);
    println(color::dark_yellow, "Storing %d", laras.size());
    FILE* file;
    errno_t err;
    if ((err = fopen_s(&file, "F:\\obs\\output.tr2", "w+b")) != 0) {
        println(color::dark_yellow, "CANNOT WRITE: %s", strerror(err));
        return;
    }
    fwrite(&elm, sizeof(elm), 1, file);
    fwrite(&laras[0], 1, size, file);
    fclose(file);
    println(color::dark_yellow, "File wrote (%d B)", size);
}

void Serializer::serialize(Entity* lara, int frame) {
    struct Ghost* ghostyLara = (Ghost*)malloc(sizeof(Ghost));
    ghostyLara->position = lara->position;
    ghostyLara->rotation = lara->rotation;
    laras.push_back(*ghostyLara);
    println(color::purple, "%d - %d / %d", frame, laras.size(), laras.capacity());
}


void Serializer::is_menu(DWORD isMenu) {
    if (_isMenu && isMenu == 0) {
        _isMenu = false;
        return;
    }
    if (!_isMenu && isMenu == 1) {
        println(color::dark_yellow, "%d - %d", isMenu, _isMenu);
        _isMenu = true;
        this->close();
    }
}
