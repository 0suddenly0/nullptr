#include "Utils.h"
#include "../hooks/hooks.h"

#define NOMINMAX
#include <Windows.h>
#include <stdio.h>
#include <vadefs.h>
#include <chrono>
#include <string>
#include <vector>

#include "../valve_sdk/csgostructs.h"
#include "math.h"

HANDLE _out = NULL, _old_out = NULL;
HANDLE _err = NULL, _old_err = NULL;
HANDLE _in = NULL, _old_in = NULL;

namespace utils
{
    unsigned int find_in_data_map(datamap_t* p_map, const char* name)
    {
        while (p_map)
        {
            for (int i = 0; i < p_map->dataNumFields; i++)
            {
                if (p_map->dataDesc[i].fieldName == NULL)
                    continue;

                if (strcmp(name, p_map->dataDesc[i].fieldName) == 0)
                    return p_map->dataDesc[i].fieldOffset[TD_OFFSET_NORMAL];

                if (p_map->dataDesc[i].fieldType == FIELD_EMBEDDED)
                {
                    if (p_map->dataDesc[i].td)
                    {
                        unsigned int offset;

                        if ((offset = find_in_data_map(p_map->dataDesc[i].td, name)) != 0)
                            return offset;
                    }
                }
            }
            p_map = p_map->baseMap;
        }

        return 0;
    }

    float lerp(float a, float b, float f)
    {
        return a + f * (b - a);
    }

    float lerp_min(float a, float b, float f)
    {
        float factor = std::clamp(b - a, (b > a ? 0.3f : -FLOAT32_MAX), (b > a ? FLOAT32_MAX : -0.3f));

        if (b - 0.2 < a && a < b + 0.2)
            return b;

        return a + f * factor;
    }

    void clear_console()
    {
        g_engine_client->execute_client_cmd("clear");
    }


    void attach_console()
    {
        _old_out = GetStdHandle(STD_OUTPUT_HANDLE);
        _old_err = GetStdHandle(STD_ERROR_HANDLE);
        _old_in = GetStdHandle(STD_INPUT_HANDLE);

        ::AllocConsole() && ::AttachConsole(GetCurrentProcessId());

        _out = GetStdHandle(STD_OUTPUT_HANDLE);
        _err = GetStdHandle(STD_ERROR_HANDLE);
        _in = GetStdHandle(STD_INPUT_HANDLE);

        SetConsoleMode(_out,
            ENABLE_PROCESSED_OUTPUT | ENABLE_WRAP_AT_EOL_OUTPUT);

        SetConsoleMode(_in,
            ENABLE_INSERT_MODE | ENABLE_EXTENDED_FLAGS |
            ENABLE_PROCESSED_INPUT | ENABLE_QUICK_EDIT_MODE);
    }

    bool message_send()
    {
        globals::profile::update_request = true;
        hooks::retrieve_message::write.send_client_hello();
        hooks::retrieve_message::write.send_client_gc_rank_update();
        hooks::retrieve_message::write.send_matchmaking_client_2gc_hello();

        return true;
    }

    float get_curtime(c_usercmd* ucmd)
    {
        if (!g_local_player)
            return 0;

        int g_tick = 0;
        c_usercmd* g_pLastCmd = nullptr;
        if (!g_pLastCmd || g_pLastCmd->hasbeenpredicted) {
            g_tick = (float)g_local_player->tick_base();
        }
        else {
            ++g_tick;
        }
        g_pLastCmd = ucmd;
        float curtime = g_tick * g_global_vars->interval_per_tick;
        return curtime;
    }

    float get_servertime(c_usercmd* cmd)
    {
        static int tick;
        static c_usercmd* lastCmd;

        if (cmd) {
            if (g_local_player && (!lastCmd || lastCmd->hasbeenpredicted))
                tick = g_local_player->tick_base();
            else
                tick++;
            lastCmd = cmd;
        }
        return tick * g_global_vars->interval_per_tick;
    }

    bool can_lag(const bool& voice_check)
    {
        if (g_global_vars->interval_per_tick * 0.9f < g_global_vars->absoluteframetime)
            return false;

        if (voice_check && g_engine_client->is_voice_recording())
            return false;

        auto* channel_info = g_engine_client->get_net_channel_info();
        if (channel_info && (channel_info->get_avg_loss(1) > 0.f || channel_info->get_avg_loss(0) > 0.f))
            return false;

        return true;
    }

    int random(const int& min, const int& max)
    {
        return rand() % (max - min + 1) + min;
    }

    float random(const float& min, const float& max)
    {
        return ((max - min) * ((float)rand() / RAND_MAX)) + min;
    }

    bool hitchance(c_base_entity* entity, const QAngle& angles, const float& chance, const float& hit_count, const int& hit_group)
    {
        if (!g_local_player)
            return false;

        auto weapon = g_local_player->active_weapon();
        if (!weapon || !weapon->is_weapon())
            return false;

        Vector forward, right, up;
        Vector src = g_local_player->get_eye_pos();
        math::angle_vectors(angles, forward, right, up);

        int cHits = 0;
        int cNeededHits = static_cast<int>(hit_count * (chance / 100.f));

        weapon->update_accuracy_penalty();
        float weap_spread = weapon->get_spread();
        float weap_inaccuracy = weapon->get_inaccuracy();

        ray_t ray;
        trace_t tr;
        c_tracefilter_players_only_skip_one filter(g_local_player);

        for (int i = 0; i < hit_count; i++)
        {
            float a = random(0.f, 1.f);
            float b = random(0.f, 2.f * M_PI);
            float c = random(0.f, 1.f);
            float d = random(0.f, 2.f * M_PI);

            float inaccuracy = a * weap_inaccuracy;
            float spread = c * weap_spread;

            if (weapon->item().item_definition_index() == WEAPON_REVOLVER)
            {
                a = 1.f - a * a;
                a = 1.f - c * c;
            }

            Vector spreadView((cos(b) * inaccuracy) + (cos(d) * spread), (sin(b) * inaccuracy) + (sin(d) * spread), 0), direction;

            direction.x = forward.x + (spreadView.x * right.x) + (spreadView.y * direction.x);
            direction.y = forward.y + (spreadView.x * right.y) + (spreadView.y * direction.y);
            direction.z = forward.z + (spreadView.x * right.z) + (spreadView.y * direction.z);
            direction.Normalized();

            QAngle viewAnglesSpread;
            math::vector_angles(direction, viewAnglesSpread);
            viewAnglesSpread.Normalize();

            Vector viewForward;
            math::angle_vectors(viewAnglesSpread, viewForward);
            viewForward.NormalizeInPlace();

            viewForward = src + (viewForward * weapon->get_cs_weapondata()->range);

            ray.init(src, viewForward);
            g_engine_trace->trace_ray(ray, MASK_SHOT | CONTENTS_GRATE, &filter, &tr);

            if (tr.hit_entity == entity && (hit_group == -1 || hit_group == tr.hitgroup))
                ++cHits;

            const auto ch = static_cast<int>((static_cast<float>(cHits) / hit_count) * 100.f);
            if (ch >= chance)
                return true;

            if ((hit_count - i + cHits) < cNeededHits)
                return false;
        }

        return false;
    }

    void detach_console()
    {
        if (_out && _err && _in) {
            FreeConsole();

            if (_old_out)
                SetStdHandle(STD_OUTPUT_HANDLE, _old_out);
            if (_old_err)
                SetStdHandle(STD_ERROR_HANDLE, _old_err);
            if (_old_in)
                SetStdHandle(STD_INPUT_HANDLE, _old_in);
        }
    }

    int epoch_time()
    {
        return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    }

    bool console_print(const char* fmt, ...)
    {
        if (!_out)
            return false;

        char buf[1024];
        va_list va;

        va_start(va, fmt);
        _vsnprintf_s(buf, 1024, fmt, va);
        va_end(va);

        return !!WriteConsoleA(_out, buf, static_cast<DWORD>(strlen(buf)), nullptr, nullptr);
    }

    /*
     * @brief Blocks execution until a key is pressed on the console window
     *
     */
    char console_read_key()
    {
        if (!_in)
            return false;

        auto key = char{ 0 };
        auto keysread = DWORD{ 0 };

        ReadConsoleA(_in, &key, 1, &keysread, nullptr);

        return key;
    }


    /*
     * @brief Wait for all the given modules to be loaded
     *
     * @param timeout How long to wait
     * @param modules List of modules to wait for
     *
     * @returns See WaitForSingleObject return values.
     */
    int wait_for_modules(std::int32_t timeout, const std::initializer_list<std::wstring>& modules)
    {
        bool signaled[32] = { 0 };
        bool success = false;

        std::uint32_t totalSlept = 0;

        if (timeout == 0)
        {
            for (auto& mod : modules)
            {
                if (GetModuleHandleW(std::data(mod)) == NULL)
                    return WAIT_TIMEOUT;
            }
            return WAIT_OBJECT_0;
        }

        if (timeout < 0)
            timeout = INT32_MAX;

        while (true) {
            for (auto i = 0u; i < modules.size(); ++i)
            {
                auto& module = *(modules.begin() + i);
                if (!signaled[i] && GetModuleHandleW(std::data(module)) != NULL)
                {
                    signaled[i] = true;

                    //
                    // Checks if all modules are signaled
                    //
                    bool done = true;
                    for (auto j = 0u; j < modules.size(); ++j)
                    {
                        if (!signaled[j])
                        {
                            done = false;
                            break;
                        }
                    }
                    if (done)
                    {
                        success = true;
                        goto exit;
                    }
                }
            }
            if (totalSlept > std::uint32_t(timeout))
            {
                break;
            }
            Sleep(10);
            totalSlept += 10;
        }

    exit:
        return success ? WAIT_OBJECT_0 : WAIT_TIMEOUT;
    }
   /*
     * @brief Scan for a given byte pattern on a module
     *
     * @param module    Base of the module to search
     * @param signature IDA-style byte array pattern
     *
     * @returns Address of the first occurence
     */
    std::uint8_t* pattern_scan(void* module, const char* signature)
    {
        static auto pattern_to_byte = [](const char* pattern)
        {
            auto bytes = std::vector<int>{};
            auto start = const_cast<char*>(pattern);
            auto end = const_cast<char*>(pattern) + strlen(pattern);

            for (auto current = start; current < end; ++current)
            {
                if (*current == '?')
                {
                    ++current;
                    if (*current == '?')
                        ++current;
                    bytes.push_back(-1);
                }
                else
                {
                    bytes.push_back(strtoul(current, &current, 16));
                }
            }
            return bytes;
        };

        auto dosHeader = (PIMAGE_DOS_HEADER)module;
        auto ntHeaders = (PIMAGE_NT_HEADERS)((std::uint8_t*)module + dosHeader->e_lfanew);

        auto sizeOfImage = ntHeaders->OptionalHeader.SizeOfImage;
        auto patternBytes = pattern_to_byte(signature);
        auto scanBytes = reinterpret_cast<std::uint8_t*>(module);

        auto s = patternBytes.size();
        auto d = patternBytes.data();

        for (auto i = 0ul; i < sizeOfImage - s; ++i)
        {
            bool found = true;
            for (auto j = 0ul; j < s; ++j)
            {
                if (scanBytes[i + j] != d[j] && d[j] != -1)
                {
                    found = false;
                    break;
                }
            }
            if (found)
            {
                return &scanBytes[i];
            }
        }
        return nullptr;
    }


    /*
     * @brief Set player clantag
     *
     * @param tag New clantag
     */
    void set_clantag(std::string tag, bool anim)
    {
        static std::string old_tag;
        std::string p_tag = "";

        if (g_player_resource && g_local_player)
        {
            p_tag = std::string(g_player_resource->clantag()[g_local_player->get_index()]);
        }

        if (old_tag != tag || (!anim && (p_tag != "" && p_tag != tag)))
        {
            static auto fnClantagChanged = (int(__fastcall*)(const char*, const char*))pattern_scan(GetModuleHandleW(L"engine.dll"), "53 56 57 8B DA 8B F9 FF 15");
            fnClantagChanged(tag.c_str(), tag.c_str());

            old_tag = tag;
        }
    }

    /*
     * @brief Set player name
     *
     * @param name New name
     */
    bool set_name(bool reconnect, const char* newName, float delay)
    {
        /*static auto nameconvar = g_cvar->find_var("name");
        nameconvar->m_fnChangeCallbacks.m_Size = 0;

        // Fix so we can change names how many times we want
        // This code will only run once because of `static`
        auto do_once = (nameconvar->SetValue("\n���"), true);

        nameconvar->SetValue(name);*/

        static auto exploitInitialized{ false };

        static auto name{ g_cvar->find_var("name") };

        if (reconnect)
        {
            exploitInitialized = false;
            return false;
        }

        if (!exploitInitialized && g_engine_client->is_ingame())
        {
            player_info_s playerInfo;

            if (g_engine_client->get_player_info(g_local_player->ent_index(), &playerInfo) && (!strcmp(playerInfo.szName, "?empty") || !strcmp(playerInfo.szName, "\n\xAD\xAD\xAD")))
            {
                exploitInitialized = true;
            }
            else
            {
                name->m_fnChangeCallbacks.m_Size = 0;
                name->set_value("\n\xAD\xAD\xAD");
                return false;
            }
        }

        static auto nextChangeTime{ 0.0f };
        if (nextChangeTime <= g_global_vars->realtime)
        {
            name->set_value(newName);
            nextChangeTime = g_global_vars->realtime + delay;
            return true;
        }
        return false;
    }

    void rank_reveal_all()
    {
        g_chl_client->dispatch_user_message(50, 0, 0, nullptr);
    }

    std::string snprintf(const char* text, ...)
    {
        char buf[1024];
        va_list va;

        va_start(va, text);
        _vsnprintf_s(buf, 1024, text, va);
        va_end(va);

        return std::string(buf);
    }
}