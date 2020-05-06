#pragma once

#define NOMINMAX
#include <Windows.h>
#include <string>
#include <initializer_list>
#include "../valve_sdk/math/QAngle.h"
#include "../valve_sdk/sdk.h"

namespace utils 
{
	unsigned int find_in_data_map(datamap_t * p_map, const char * name);
    int wait_for_modules(std::int32_t timeout, const std::initializer_list<std::wstring>& modules);
    std::uint8_t* pattern_scan(void* module, const char* signature);

	void attach_console();
    bool console_print(const char* fmt, ...);
    char console_read_key();
    void detach_console();

    bool message_send();

    int epoch_time();
    float get_curtime(c_usercmd* ucmd);
    float get_servertime(c_usercmd* ucmd);
	bool can_lag(const bool& voice_check = true);
    float lerp(float a, float b, float f);
    float lerp_min(float a, float b, float f);
    void clear_console();
	int random(const int& min, const int& max);
	float random(const float& min, const float& max);
    void ReadWavFileIntoMemory(std::string fname, BYTE** pb, DWORD* fsize);
    void play_sound(const char* file);
	bool hitchance(c_base_entity* entity, const QAngle& angles, const float& chance, const float& hit_count = 150.f, const int& hit_group = -1);
    void set_clantag(std::string tag, bool anim = true);
    bool set_name(bool reconnect, const char* newName, float delay);
    void rank_reveal_all();

    std::string snprintf(const char* text, ...);
}
