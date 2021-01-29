#pragma once

#include <vector>
#include "console.h"
#include "DataStruct.h"
#include "Ghost.h"


class Serializer {
private:
    long MAX_LARAS = 1024*1024;
    std::vector<Ghost> laras;
    bool _isMenu = true;
    long long int _index = 0;

public:
    Serializer();
    void open_read();
    void close();
    void serialize(Entity* lara, int frame);
    bool read(Entity* newLara, Entity* oldLara);
    void is_menu(DWORD isMenu);
};

