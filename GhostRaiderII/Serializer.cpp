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
    fopen_s(&fd, "F:\\obs\\output.tr2", "r");
}

void Serializer::close() {

    long long int size = laras.size() * (long long int) sizeof(Ghost);
    println(color::dark_yellow, "Storing %d", laras.size());
    FILE* file;
    errno_t err;
    if ((err = fopen_s(&file, "F:\\obs\\output.tr2", "w+b")) != 0) {
        println(color::dark_yellow, "CANNOT WRITE: %s", strerror(err));
        return;
    }
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
