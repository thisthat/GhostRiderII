#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "console.h"
#include "DataStruct.h"
#include "Ghost.h"


enum class SerializerMode { read, write };

class SerializerConf {
private:
    std::string _path = "F:\\obs\\output.tr2";
    SerializerMode _mode = SerializerMode::write;

public:
    SerializerConf() {};
    void init() {
        char path[_MAX_PATH + 1];
        GetModuleFileNameA(NULL, path, _MAX_PATH);
        std::string str = std::string(path);
        std::string dll_base_path = str.substr(0, str.find_last_of("\\/") + 1);
        std::string conf_path = dll_base_path + "/ghostraider.ini";
        println(color::white, "Loading info from %s", conf_path.c_str());
        std::ifstream file(conf_path);
        std::string mode;
        // get mode
        std::getline(file, mode);
        println(color::white, "[mode] %s", mode.c_str());
        _mode = SerializerMode::read;
        if (mode == "write") {
            _mode = SerializerMode::write;
        }
        std::getline(file, _path);
        println(color::white, "[path] %s", _path.c_str());
        
    };
    SerializerMode getMode() {
        return _mode;
    };
    std::string getDataPath() {
        return _path;
    };
};

class Serializer {
private:
    long MAX_LARAS = 1024 * 1024;
    std::vector<Ghost> laras;
    bool _isMenu = true;
    size_t _index = 0;
    SerializerConf _conf;
    bool _isInit = false;
    int32_t _level_id = 0;

    std::string _getFileName() {
        return _conf.getDataPath() + std::to_string(_level_id) + ".tr2";
    };

public:
    Serializer();
    void init(int32_t level_id);
    void open_read();
    void close();
    void serialize(Entity* lara);
    bool read(Entity* newLara, Entity* oldLara);
    void is_menu(DWORD isMenu);
    bool isWrite();
    bool isRead();
    bool isInit();
};
