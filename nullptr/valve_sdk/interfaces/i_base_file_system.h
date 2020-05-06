#pragma once

#include <fstream>
#include <string>
#include <optional>
#include <map>

class i_base_file_system
{
public:

    int read(void* output, int size, void* file)
    {
        return call_vfunction<int(__thiscall*)(void*, void*, int, void*)>(this, 0)(this, output, size, file);
    }

    void* open(const char* file_name, const char* options, const char* path_id)
    {
        return call_vfunction<void* (__thiscall*)(void*, const char*, const char*, const char*)>(this, 2)(this, file_name, options, path_id);
    }

    void close(void* file)
    {
        return call_vfunction<void(__thiscall*)(void*, void*)>(this, 3)(this, file);
    }

    unsigned int size(void* file)
    {
        return call_vfunction<unsigned int(__thiscall*)(void*, void*)>(this, 7)(this, file);
    }
};