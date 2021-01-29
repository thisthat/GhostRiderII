#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "console.h"
#include "Serializer.h"

Serializer::Serializer() {
    laras.reserve(MAX_LARAS);
}

void Serializer::init() {
    _conf.init();
}

bool Serializer::isWrite() {
    return _conf.getMode() == SerializerMode::write;
}

bool Serializer::isRead() {
    return !this->isWrite();
}


bool finish_read = false;
void Serializer::open_read() {
    FILE* file;
    fopen_s(&file, _conf.getDataPath().c_str(), "rb");
    _index = 0;
    long elm;
    if (file != NULL) {
        fread((char*)&elm, sizeof(elm), 1, file);
        laras.resize(elm);
        size_t size = elm * sizeof(Ghost);
        fread(&laras[0], 1, size, file);
        finish_read = false;
        fclose(file);
    }
    else {
        println(color::dark_yellow, "CANNOT READ: %s", _conf.getDataPath().c_str());
    }
}

bool Serializer::read(Entity* newLara, Entity* lara) {
    if (_index >= laras.size()) {
        if (!finish_read) {
            println(color::cyan, "Finish reproducing ghost");
        }
        finish_read = true;
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
    newLara->position = ivec3(g.p_x, g.p_y, g.p_z);
    newLara->rotation = small_ivec3(g.r_x, g.r_y, g.r_z);
    newLara->flags = 0;
    return true;
}

void Serializer::close() {
    long elm = laras.size();
    size_t size = elm * sizeof(Ghost);
    println(color::dark_yellow, "Storing %d", laras.size());
    FILE* file;
    errno_t err;
    if ((err = fopen_s(&file, _conf.getDataPath().c_str(), "w+b")) != 0) {
        println(color::dark_yellow, "CANNOT WRITE: %s", strerror(err));
        return;
    }
    if (file != NULL) {
        fwrite(&elm, sizeof(elm), 1, file);
        fwrite(&laras[0], 1, size, file);
        fclose(file);
    }
    println(color::dark_yellow, "File wrote (%d B)", size);
}

void Serializer::serialize(Entity* lara) {
    struct Ghost* ghostyLara = (Ghost*)malloc(sizeof(Ghost));
    ghostyLara->p_x = lara->position.x;
    ghostyLara->p_y = lara->position.y;
    ghostyLara->p_z = lara->position.z;
    ghostyLara->r_x = lara->rotation.x;
    ghostyLara->r_y = lara->rotation.y;
    ghostyLara->r_z = lara->rotation.z;
    laras.push_back(*ghostyLara);
}


void Serializer::is_menu(DWORD isMenu) {
    if (_isMenu && isMenu == 0) {
        _isMenu = false;
        return;
    }
    if (!_isMenu && isMenu == 1) {
        println(color::dark_yellow, "%d - %d", isMenu, _isMenu);
        _isMenu = true;
        if (this->isWrite()) {
            this->close();
        }
    }
}
