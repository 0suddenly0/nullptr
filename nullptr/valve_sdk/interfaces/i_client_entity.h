#pragma once 

#include "i_client_networkable.h"
#include "i_client_renderable.h"
#include "i_client_unknown.h"
#include "i_client_thinkable.h"

struct spatialization_info_t;

class i_client_entity : public i_client_unknown, public i_client_renderable, public i_client_networkable, public i_client_thinkable
{
public:
	virtual void release(void) = 0;
};

#pragma pack( push, 1 )
class c_cs_weapon_info {
public:
	void* vftbl;
	char* console_name;                   // 0x0004
	char pad_0008[12];                   // 0x0008
	int max_clip1;                       // 0x0014
	int max_clip2;                       // 0x0018
	int default_clip1;                   // 0x001C
	int default_clip2;                   // 0x0020
	char pad_0024[8];                    // 0x0024
	char* world_model;                  // 0x002C
	char* ciew_model;                   // 0x0030
	char* dropped_model;                // 0x0034
	char pad_0038[4];                    // 0x0038
	char* N0000023E;                     // 0x003C
	char pad_0040[60];                   // 0x0040
	char* empty_sound;                  // 0x0078
	char pad_007C[4];                    // 0x007C
	char* bullet_type;                  // 0x0080
	char pad_0084[4];                    // 0x0084
	char* hud_name;                     // 0x0088
	char* weapon_name;                  // 0x008C
	char pad_0090[52];                   // 0x0090
	int weapon_type;                      // 0x00C8
	int weapon_pad;                       // 0x0CC
	int weapon_price;                    // 0x00D0
	int kill_award;                      // 0x00D4
	char* animation_prefix;             // 0x00D8
	float cycle_time;                   // 0x00DC
	float cycle_time_alt;                // 0x00E0
	float time_to_idle;                  // 0x00E4
	float idle_interval;                // 0x00E8
	bool full_auto;                      // 0x00E9
	char pad_0x00EC[3];                  // 0x00EC
	int damage;                         // 0x00F0
	float armor_ratio;                  // 0x00F4
	int bullets;                        // 0x00F8
	float penetration;                 // 0x00FC
	float flinch_velocity_modifier_large; // 0x0100
	float flinch_velocity_modifier_small; // 0x0104
	float range;                       // 0x0108
	float range_modifier;               // 0x010C
	float throw_velocity;               // 0x0110
	char pad_0x010C[12];                 // 0x011C
	bool has_silencer;                   // 0x011D
	char pad_0x0119[3];                  // 0x0120
	char* silencer_model;                // 0x0124
	int crosshair_min_distance;           // 0x0128
	int crosshair_delta_distance;         // 0x012C - iTeam?
	float max_player_speed;              // 0x0130
	float max_player_speed_alt;           // 0x0134
	float spread;                      // 0x0134
	float spread_alt;                   // 0x0138
	float inaccuracy_crouch;            // 0x013C
	float inaccuracy_crouch_alt;         // 0x0140
	float inaccuracy_stand;             // 0x0144
	float inaccuracy_stand_alt;          // 0x0148
	float inaccuracy_jump_initial;       // 0x014C
	float inaccuracy_jump;              // 0x0150
	float inaccuracy_jmp_alt;           // 0x0154
	float inaccuracy_land;              // 0x0158
	float inaccuracy_land_alt;           // 0x015C
	float inaccuracy_ladder;            // 0x0160
	float inaccuracy_ladder_alt;         // 0x0164
	float inaccuracy_fire;              // 0x0168
	float inaccuracy_fire_alt;           // 0x016C
	float inaccuracy_move;              // 0x0170
	float inaccuracy_move_alt;           // 0x0174
	float inaccuracy_reload;            // 0x0178
	int recoil_seed;                     // 0x017C
	float recoil_angle;                 // 0x0180
	float recoil_angle_alt;              // 0x0184
	float recoil_angle_variance;         // 0x0188
	float recoil_angle_variance_alt;      // 0x018C
	float recoil_magnitude;             // 0x0190
	float recoil_magnitude_alt;          // 0x0194
	float recoil_magnitude_variance;     // 0x0198
	float recoil_magnitude_variance_alt;  // 0x019C
	float recovery_time_crouch;          // 0x01A0
	float recovery_time_stand;           // 0x01A4
	float recovery_time_crouch_final;     // 0x01A8
	float recovery_time_stand_final;      // 0x01AC
	int recovery_transition_start_bullet;  // 0x01B0
	int recovery_transition_end_bullet;    // 0x01B4
	bool unzoom_after_shot;               // 0x01B8
	bool hide_view_model_zoomed;           // 0x01B9
	char pad_0x01B5[2];                  // 0x01BA
	char zoom_levels[4];                 // 0x01BC
	int zoom_fov[2];                     // 0x01C0
	float zoom_time[3];                  // 0x01C4
	char* weapon_class;                 // 0x01D4
	float addon_scale;                  // 0x01D8
	char pad_0x01DC[4];                  // 0x01DC
	char* eject_brass_effect;            // 0x01E0
	char* tracer_effect;                // 0x01E4
	int tracer_frequency;                // 0x01E8
	int tracer_frequency_alt;             // 0x01EC
	char* muzzle_flash_effect_1st_person; // 0x01F0
	char pad_0x01F4[4];                  // 0x01F4
	char* muzzle_flash_effect_3rd_person; // 0x01F8
	char pad_0x01FC[4];                  // 0x01FC
	char* muzzle_smoke_effect;           // 0x0200
	float geat_per_shot;                 // 0x0204
	char* zoom_in_sound;                 // 0x0208
	char* zoom_out_sound;                // 0x020C
	float inaccuracy_pitch_shift;        // 0x0210
	float inaccuracy_sound_threshold;    // 0x0214
	float bot_audible_range;             // 0x0218
	char pad_0x0218[8];                  // 0x0220
	char* wrong_team_msg;                 // 0x0224
	bool has_burst_mode;                  // 0x0228
	char pad_0x0225[3];                  // 0x0229
	bool is_revolver;                    // 0x022C
	bool cannot_shoot_underwater;         // 0x0230
};
#pragma pack( pop )


class i_weapon_system
{
	virtual void unused0() = 0;
	virtual void unused1() = 0;
public:
	virtual c_cs_weapon_info* get_wpn_data(unsigned ItemDefinitionIndex) = 0;
};
