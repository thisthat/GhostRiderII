#pragma once

#include <vector>
#include "console.h"
#include "DataStruct.h"
#include "Ghost.h"


class Serializer {
private:
    FILE* fd = NULL;
    long MAX_LARAS = 1024;
    std::vector<Ghost> laras;
    bool _isMenu = true;

public:
    Serializer();
    void open_read();
    void close();
    void serialize(Entity* lara, int frame);
    Entity* read();
    void is_menu(DWORD isMenu);
};

