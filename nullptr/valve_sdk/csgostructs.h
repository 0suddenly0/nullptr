#pragma once

#include "sdk.h"
#include <array>
#include "../helpers/utils.h"

/*#define NETVAR(type, funcname, class_name, var_name) \
	NETVAR_OFFSET(type, funcname, class_name, var_name, 0) \

#define NETVAR_OFFSET(type, funcname, class_name, var_name, offset) \
auto funcname() -> std::add_lvalue_reference_t<type> \
{ \
	constexpr auto hash = fnv::hash_constexpr(class_name "->" var_name); \
	const auto addr = std::uintptr_t(this) + offset + NetvarSys::Get().Get<hash>(); \
	return *reinterpret_cast<std::add_pointer_t<type>>(addr); \
} \*/

#define NETVAR(type, name, table, netvar)                           \
    type& name##() const {                                          \
        static int _##name = netvar_sys::get_offset(table, netvar);     \
        return *(type*)((uintptr_t)this + _##name);                 \
    }

#define PNETVAR(type, name, table, netvar)                           \
    type* name##() const {                                          \
        static int _##name = netvar_sys::get_offset(table, netvar);     \
        return (type*)((uintptr_t)this + _##name);                 \
    } \


#define NETVAR_EX( type, name, table, netvar, extra )                         \
  type& name##() const {                                                      \
    static int _##name = netvar_sys::get_offset( table, netvar ) + extra; \
    return *(type*)( (uintptr_t)this + _##name );                             \
  }

#define NETPROP(name, table, netvar) static recv_prop* name() \
{ \
	static auto prop_ptr = netvar_sys::get_netvar_prop(table,netvar); \
	return prop_ptr; \
}

struct datamap_t;
class animation_layer;
class c_base_player_animstate;
class c_csgo_player_animstate;
class c_base_entity;

enum cs_weapon_type
{
	WEAPONTYPE_KNIFE = 0,
	WEAPONTYPE_PISTOL,
	WEAPONTYPE_SUBMACHINEGUN,
	WEAPONTYPE_RIFLE,
	WEAPONTYPE_SHOTGUN,
	WEAPONTYPE_SNIPER_RIFLE,
	WEAPONTYPE_MACHINEGUN,
	WEAPONTYPE_C4,
	WEAPONTYPE_PLACEHOLDER,
	WEAPONTYPE_GRENADE,
	WEAPONTYPE_UNKNOWN
};

class c_base_entity;

// Created with ReClass.NET by KN4CK3R
class c_hud_texture
{
public:
	char short_name[64];    //0x0000
	char texture_file[64];  //0x0040
	bool render_using_font;   //0x0080
	bool precached;         //0x0081
	int8_t character_in_font; //0x0082
	uint8_t pad_0083[1];     //0x0083
	uint32_t font;          //0x0084
	int32_t texture_id;      //0x0088
	float tex_coords[4];    //0x008C
	uint8_t pad_009C[16];    //0x009C
};

class c_econ_item_view
{
private:
	using str_32 = char[32];
public:
	NETVAR( int32_t, initialized, "DT_BaseAttributableItem", "m_bInitialized" );
	NETVAR( int16_t, item_definition_index, "DT_BaseAttributableItem", "m_iItemDefinitionIndex" );
	NETVAR( int32_t, entity_level, "DT_BaseAttributableItem", "m_iEntityLevel" );
	NETVAR( int32_t, account_id, "DT_BaseAttributableItem", "m_iAccountID" );
	NETVAR( int32_t, item_id_low, "DT_BaseAttributableItem", "m_iItemIDLow" );
	NETVAR( int32_t, item_id_high, "DT_BaseAttributableItem", "m_iItemIDHigh" );
	NETVAR( int32_t, entity_quality, "DT_BaseAttributableItem", "m_iEntityQuality" );
	NETVAR( str_32,  custom_name, "DT_BaseAttributableItem", "m_szCustomName" );
};

class c_base_entity : public i_client_entity
{
public:
	datamap_t * get_data_desc_map( ) {
		typedef datamap_t*( __thiscall *o_GetPredDescMap )( void* );
		return call_vfunction<o_GetPredDescMap>( this, 15 )( this );
	}

	datamap_t *get_pred_desc_map( ) {
		typedef datamap_t*( __thiscall *o_GetPredDescMap )( void* );
		return call_vfunction<o_GetPredDescMap>( this, 17 )( this );
	}
	static __forceinline c_base_entity* get_entity_by_index( int index ) {
		return static_cast< c_base_entity* >( g_entity_list->get_client_entity( index ) );
	}
	static __forceinline c_base_entity* get_entity_from_handle(c_base_handle h ) {
		return static_cast< c_base_entity* >( g_entity_list->get_client_entity_from_handle( h ) );
	}

	NETVAR(int32_t, model_index, "DT_BaseEntity", "m_nModelIndex" );
	NETVAR(int32_t, team_num, "DT_BaseEntity", "m_iTeamNum" );
	NETVAR(Vector, vec_origin, "DT_BaseEntity", "m_vecOrigin" );
	NETVAR(Vector, vec_angles, "DT_BaseEntity", "m_vecAngles" );
	NETVAR(bool, should_glow, "DT_DynamicProp", "m_bShouldGlow" );
	NETVAR(int32_t, hitbox_set, "DT_BaseAnimating", "m_nHitboxSet");
	NETVAR(int32_t, life_state, "DT_BasePlayer", "m_lifeState");
	NETVAR(c_handle<c_base_player>, owner_entity, "DT_BaseEntity", "m_hOwnerEntity" );
	NETVAR(bool, spotted, "DT_BaseEntity", "m_bSpotted" );
	NETVAR(float_t, c4_blow, "DT_PlantedC4", "m_flC4Blow" );
	NETVAR_EX(int, get_index, "DT_BaseEntity", "m_bIsAutoaimTarget", +0x4);

	void set_module_index(int index)
	{
		return call_vfunction<void(__thiscall*)(void*, int)>(this, 75)(this, index);
	}

	const matrix3x4_t& coordinate_frame( ) {
		static auto _m_rgflCoordinateFrame = netvar_sys::get_offset( "DT_BaseEntity", "m_CollisionGroup" ) - 0x30;
		return *( matrix3x4_t* )( ( uintptr_t )this + _m_rgflCoordinateFrame );
	}

	bool is_player();
	bool is_loot();
	bool is_weapon();
	bool is_planted_c4();
	bool is_defuse_kit();
	matrix3x4_t get_bone_matrix(int BoneID);
	int get_health();

	bool new_setup_bones(matrix3x4_t* matrix);

	Vector& get_abs_origin()
	{
		return call_vfunction<Vector & (__thiscall*)(void*)>(this, 10)(this);
	}

	void set_abs_original(Vector origin)
	{
		using SetAbsOriginFn = void(__thiscall*)(void*, const Vector & origin);
		static SetAbsOriginFn set_abs_origin;
		if (!set_abs_origin)
		{
			set_abs_origin = (SetAbsOriginFn)((DWORD)utils::pattern_scan(GetModuleHandleW(L"client_panorama.dll"), "55 8B EC 83 E4 F8 51 53 56 57 8B F1 E8"));
		}
		set_abs_origin(this, origin);
	}

	int get_lua_netvar_int(int offset)
	{
		return *(int*)((uintptr_t)this + offset);
	}

	float get_lua_netvar_float(int offset)
	{
		return *(float*)((uintptr_t)this + offset);
	}

	bool get_lua_netvar_bool(int offset)
	{
		return *(bool*)((uintptr_t)this + offset);
	}

	void set_lua_netvar_int(int offset, int val)
	{
		*(int*)((uintptr_t)this + offset) = val;
	}

	void set_lua_netvar_float(int offset, float val)
	{
		*(float*)((uintptr_t)this + offset) = val;
	}

	void set_lua_netvar_bool(int offset, bool val)
	{
		*(bool*)((uintptr_t)this + offset) = val;
	}
};

class c_planted_c4
{
public:
	NETVAR( bool, bomb_ticking, "DT_PlantedC4", "m_bBombTicking" );
	NETVAR( bool, bomb_defused, "DT_PlantedC4", "m_bBombDefused" );
	NETVAR( float, c4_blow, "DT_PlantedC4", "m_flC4Blow" );
	NETVAR(float, timer_length, "DT_PlantedC4", "m_flTimerLength");
	NETVAR( float, defuse_length, "DT_PlantedC4", "m_flDefuseLength" );
	NETVAR( float, defuse_count_down, "DT_PlantedC4", "m_flDefuseCountDown" );
	NETVAR( c_handle<c_base_player>, m_hBombDefuser, "DT_PlantedC4", "m_hBombDefuser" );
};

class c_base_attributable_item : public c_base_entity
{
public:
	NETVAR( uint64_t, original_owner_xuid, "DT_BaseAttributableItem", "m_OriginalOwnerXuidLow" );
	NETVAR( int32_t, original_owner_xuid_low, "DT_BaseAttributableItem", "m_OriginalOwnerXuidLow" );
	NETVAR( int32_t, original_owner_xuid_high, "DT_BaseAttributableItem", "m_OriginalOwnerXuidHigh" );
	NETVAR( int32_t, fallback_stat_trak, "DT_BaseAttributableItem", "m_nFallbackStatTrak" );
	NETVAR( int32_t, fallback_paint_kit, "DT_BaseAttributableItem", "m_nFallbackPaintKit" );
	NETVAR( int32_t, fallback_seed, "DT_BaseAttributableItem", "m_nFallbackSeed" );
	NETVAR( float_t, fallback_wear, "DT_BaseAttributableItem", "m_flFallbackWear" );

	//NETVAR( c_econ_item_view, item, "DT_BaseAttributableItem", "m_Item" );

	c_econ_item_view& item( ) {
		// Cheating. It should be this + m_Item netvar but then the netvars inside c_econ_item_view wont work properly.
		// A real fix for this requires a rewrite of the netvar manager
		return *( c_econ_item_view* )this;
	}
	void set_glove_model_index( int modelIndex );

	void set_odel_index(const int index)
	{
		return call_vfunction<void(__thiscall*)(c_base_entity*, int)>(this, 75)(this, index);
	}

};

class c_base_weapon_world_model : public c_base_entity
{
public:
	NETVAR( int32_t, model_index, "DT_BaseWeaponWorldModel", "m_nModelIndex" );
};

class c_base_combat_weapon : public c_base_attributable_item
{
public:
	NETVAR( float_t, next_primary_attack, "DT_BaseCombatWeapon", "m_flNextPrimaryAttack" );
	NETVAR( float_t, next_secondary_attack, "DT_BaseCombatWeapon", "m_flNextSecondaryAttack" );
	NETVAR( int32_t, clip1, "DT_BaseCombatWeapon", "m_iClip1" );
	NETVAR( int32_t, clip2, "DT_BaseCombatWeapon", "m_iClip2" );
	NETVAR(bool, started_arming, "DT_WeaponC4", "m_bStartedArming");
	NETVAR( float_t, recoil_index, "DT_WeaponCSBase", "m_flRecoilIndex" );
	NETVAR( int32_t, view_model_index, "DT_BaseCombatWeapon", "m_iViewModelIndex" );
	NETVAR( int32_t, world_model_index, "DT_BaseCombatWeapon", "m_iWorldModelIndex" );
	NETVAR( int32_t, world_dropped_model_index, "DT_BaseCombatWeapon", "m_iWorldDroppedModelIndex" );
	NETVAR( bool, pin_pulled, "DT_BaseCSGrenade", "m_bPinPulled" );
	NETVAR( float_t, throw_time, "DT_BaseCSGrenade", "m_fThrowTime" );
	NETVAR( float_t, postpone_fire_ready_time, "DT_BaseCombatWeapon", "m_flPostponeFireReadyTime" );
	NETVAR( c_handle<c_base_weapon_world_model>, weapon_world_model, "DT_BaseCombatWeapon", "m_hWeaponWorldModel" );

	NETVAR( int32_t, weapon_mode, "DT_WeaponCSBase", "m_weaponMode" );
	NETVAR( int32_t, zoom_level, "DT_WeaponCSBaseGun", "m_zoomLevel" );


	c_cs_weapon_info* get_cs_weapondata( );
	bool has_bullets( );
	bool can_fire( );
	bool is_grenade( );
	bool is_knife( );
	bool is_zeus( );
	bool is_reloading( );
	bool is_rifle( );
	bool is_pistol( );
	bool is_sniper( );
	bool is_gun( );

	float get_inaccuracy( );
	float get_spread( );
	void update_accuracy_penalty( );
	CUtlVector<i_ref_counted*>& custom_materials( );
	bool* custom_material_initialized( );
	int get_item_definition_index();

};

enum observer_mode_t {
	OBS_MODE_NONE = 0,
	OBS_MODE_DEATHCAM = 1,
	OBS_MODE_FREEZECAM = 2,
	OBS_MODE_FIXED = 3,
	OBS_MODE_IN_EYE = 4,
	OBS_MODE_CHASE = 5,
	OBS_MODE_ROAMING = 6
};

class c_base_player : public c_base_entity
{
public:
	static __forceinline c_base_player* get_player_by_user_id( int id ) {
		return static_cast< c_base_player* >( get_entity_by_index( g_engine_client->get_player_for_user_id( id ) ) );
	}
	static __forceinline c_base_player* get_player_by_index( int i ) {
		return static_cast< c_base_player* >(get_entity_by_index( i ) );
	}

	NETVAR( observer_mode_t, observer_mode, "DT_BasePlayer", "m_iObserverMode")
	NETVAR( bool, is_defusing, "DT_CSPlayer", "m_bIsDefusing" );
	NETVAR( bool, gun_game_immunity, "DT_CSPlayer", "m_bGunGameImmunity" );
	NETVAR( int32_t, shots_fired, "DT_CSPlayer", "m_iShotsFired" );
	NETVAR( QAngle, eye_angles, "DT_CSPlayer", "m_angEyeAngles[0]" );
	NETVAR( int, armor_value, "DT_CSPlayer", "m_ArmorValue" );
	NETVAR( bool, has_heavy_armor, "DT_CSPlayer", "m_bHasHeavyArmor" );
	NETVAR( bool, has_helmet, "DT_CSPlayer", "m_bHasHelmet" );
	NETVAR( int, ragdoll, "DT_CSPlayer", "m_hRagdoll");
	NETVAR( bool, is_scoped, "DT_CSPlayer", "m_bIsScoped" );;
	NETVAR( float, lower_body_yaw_target, "DT_CSPlayer", "m_flLowerBodyYawTarget" );
	NETVAR( int32_t, health, "DT_BasePlayer", "m_iHealth" );
	NETVAR( int32_t, life_state, "DT_BasePlayer", "m_lifeState" );
	NETVAR( int32_t, m_flags, "DT_BasePlayer", "m_fFlags" );
	NETVAR( int32_t, tick_base, "DT_BasePlayer", "m_nTickBase" );
	NETVAR( Vector, view_offset, "DT_BasePlayer", "m_vecViewOffset[0]" );
	NETVAR( QAngle, view_punch_angle, "DT_BasePlayer", "m_viewPunchAngle" );
	NETVAR( QAngle, aim_punch_angle, "DT_BasePlayer", "m_aimPunchAngle" );
	NETVAR( c_handle<c_base_view_model>, view_model, "DT_BasePlayer", "m_hViewModel[0]" );
	NETVAR( Vector, velocity, "DT_BasePlayer", "m_vecVelocity[0]" );
	NETVAR( float, max_speed, "DT_BasePlayer", "m_flMaxspeed" );
	NETVAR( c_handle<c_base_player>, observer_target, "DT_BasePlayer", "m_hObserverTarget" );
	NETVAR( float, flash_max_alpha, "DT_CSPlayer", "m_flFlashMaxAlpha" );
	NETVAR( float, next_attack, "DT_BaseCombatCharacter", "m_flNextAttack" );
	NETVAR( c_handle<c_base_combat_weapon>, active_weapon, "DT_BaseCombatCharacter", "m_hActiveWeapon" );
	NETVAR( int32_t, account, "DT_CSPlayer", "m_iAccount" );
	NETVAR( float, flash_duration, "DT_CSPlayer", "m_flFlashDuration" );
	NETVAR( float, simulation_time, "DT_BaseEntity", "m_flSimulationTime" );
	NETVAR( float, cycle, "DT_ServerAnimationData", "m_flCycle" );
	NETVAR( int, sequence, "DT_BaseViewModel", "m_nSequence" );
	PNETVAR( char, last_place_name, "DT_BasePlayer", "m_szLastPlaceName" );
	NETPROP( lower_body_yaw_target_prop, "DT_CSPlayer", "m_flLowerBodyYawTarget" );
	NETVAR( int32_t, fov, "DT_BasePlayer", "m_iFOV" );
	NETVAR( int32_t, default_fov, "DT_BasePlayer", "m_iDefaultFOV" );

	NETVAR(int, survival_team, "DT_CSPlayer", "m_nSurvivalTeam");

	int get_fov( ) {
		if( fov( ) != 0 )
			return fov( );
		return default_fov( );
	}

	//NETVAR(int, m_iAccount, "DT_CSPlayer", "m_iAccount");

	bool in_dangerzone()
	{
		static auto game_type = g_cvar->find_var("game_type");
		return game_type->get_int() == 6;
	}

	bool is_enemy()
	{
		if (in_dangerzone())
		{
			return this->survival_team() != g_local_player->survival_team() || g_local_player->survival_team() == -1;
		}
		else
		{
			return this->team_num() != g_local_player->team_num();
		}
	}

	void set_module_index(int index)
	{
		return call_vfunction<void(__thiscall*)(void*, int)>(this, 75)(this, index);
	}

	void set_abs_angles2(Vector angle)
	{
		using SetAbsAnglesFn = void(__thiscall*)(void*, const Vector & angle);
		static SetAbsAnglesFn set_abs_angles;

		if (!set_abs_angles)
			set_abs_angles = (SetAbsAnglesFn)(utils::pattern_scan(GetModuleHandleW(L"client_panorama.dll"), "55 8B EC 83 E4 F8 83 EC 64 53 56 57 8B F1 E8"));

		set_abs_angles(this, angle);
	}

	void set_abs_angles(const QAngle& angles)
	{
		using SetAbsAnglesFn = void(__thiscall*)(void*, const QAngle &angles);
		static SetAbsAnglesFn set_abs_angles = (SetAbsAnglesFn)utils::pattern_scan(GetModuleHandleW(L"client_panorama.dll"), "55 8B EC 83 E4 F8 83 EC 64 53 56 57 8B F1 E8");

		set_abs_angles(this, angles);
	}

	void set_abs_original(Vector origin)
	{
		using SetAbsOriginFn = void(__thiscall*)(void*, const Vector &origin);
		static SetAbsOriginFn SetAbsOrigin;
		if (!SetAbsOrigin)
		{
			SetAbsOrigin = (SetAbsOriginFn)((DWORD)utils::pattern_scan(GetModuleHandleW(L"client_panorama.dll"), "55 8B EC 83 E4 F8 51 53 56 57 8B F1 E8"));
		}
		SetAbsOrigin(this, origin);
	}

	NETVAR( QAngle, abs_angles, "DT_BaseEntity", "m_angAbsAngles" );
	NETVAR( Vector, abs_origin, "DT_BaseEntity", "m_angAbsOrigin" );
	NETVAR( float, duck_speed, "DT_BaseEntity", "m_flDuckSpeed" );
	NETVAR( float, duck_amount, "DT_BasePlayer", "m_flDuckAmount" );
	matrix3x4_t get_bone_matrix(int BoneID);
	std::array<float, 24> pose_parameter( ) const {
		static int _pose_parameter = netvar_sys::get_offset( "DT_BaseAnimating", "m_flPoseParameter" );
		return *( std::array<float, 24>* )( ( uintptr_t )this + _pose_parameter);
	}


	PNETVAR( c_handle<c_base_combat_weapon>, my_weapons, "DT_BaseCombatCharacter", "m_hMyWeapons" );
	PNETVAR( c_handle<c_base_attributable_item>, my_wearables, "DT_BaseCombatCharacter", "m_hMyWearables" );

	c_usercmd*& current_command( );

	void invalidate_bone_cache( );
	int get_num_anim_overlays( );
	Vector& get_abs_angles2();
	Vector& get_abs_origin();
	void set_angle2(Vector wantedang);
	animation_layer *get_anim_overlays( );
	animation_layer *get_anim_overlay( int i );
	int get_sequence_activity( int sequence );
	c_csgo_player_animstate *get_player_anim_state( );

	static void update_animation_state( c_csgo_player_animstate *state, QAngle angle );
	static void reset_animation_state( c_csgo_player_animstate *state );
	void create_animation_state( c_csgo_player_animstate *state );

	float_t &surface_friction( ) {
		static unsigned int _m_surfaceFriction = utils::find_in_data_map( get_pred_desc_map( ), "m_surfaceFriction" );
		return *( float_t* )( ( uintptr_t )this + _m_surfaceFriction );
	}
	Vector &base_velocity( ) {
		static unsigned int _m_vecBaseVelocity = utils::find_in_data_map(get_pred_desc_map( ), "m_vecBaseVelocity" );
		return *( Vector* )( ( uintptr_t )this + _m_vecBaseVelocity );
	}

	/*float_t &max_speed( ) {
		static unsigned int _m_flMaxspeed = utils::find_in_data_map(get_pred_desc_map( ), "m_flMaxspeed" );
		return *( float_t* )( ( uintptr_t )this + _m_flMaxspeed );
	}*/


	Vector        get_eye_pos( );
	player_info_t get_player_info( );
	bool          is_alive( );
	bool		  is_flashed(int min_alpha);
	bool          has_c4( );
	Vector        get_hitbox_pos( int hitbox_id );

	void		  precace_optimized_hitboxes();
	bool get_optimized_hitbox_pos(int hitbox, Vector& output);

	mstudiobbox_t * get_hitbox( int hitbox_id );
	bool          get_hitboxPos( int hitbox, Vector &output );
	Vector        get_bone_pos( int bone );
	bool          can_see_player( c_base_player* player, int hitbox );
	bool          can_see_player( c_base_player* player, const Vector& pos );
	float         can_see_player(const Vector& pos);
	void update_client_side_animation( );
	float get_max_desync_delta();

	int& move_type( );


	QAngle * get_vangles( );

	float_t spawn_time( );
	std::string get_name();
	bool is_not_target( );

	c_base_player* get_local_player()
	{
		return (c_base_player*)g_local_player;
	}

};

class c_player_resource
{
private:
	using int_65 = int[65];
public:
	NETPROP( iteam, "DT_CSPlayerResource", "m_iTeam");
	NETVAR(int_65, iping, "DT_CSPlayerResource", "m_iPing");
};

class c_cs_player_resource : public c_player_resource
{
private:
	using int_65 = int[65];
	using char6516 = char[65][16];
	using str_32 = char[32];
	using unsigned65 = unsigned[65];

public:
	NETVAR(int_65, ranking, "DT_CSPlayerResource", "m_iCompetitiveRanking");
	NETVAR(int_65, wins, "DT_CSPlayerResource", "m_iCompetitiveWins");
	NETVAR(int_65, team_color, "DT_CSPlayerResource", "m_iCompTeammateColor");
	NETVAR(char6516, clantag, "DT_CSPlayerResource", "m_szClan");
	NETVAR(unsigned65, coin, "DT_CSPlayerResource", "m_nActiveCoinRank");
	NETVAR(unsigned65, musik_kit, "DT_CSPlayerResource", "m_nMusicID");
};

class c_base_view_model : public c_base_entity
{
public:
	NETVAR( int32_t, modeli_ndex, "DT_BaseViewModel", "m_nModelIndex" );
	NETVAR( int32_t, view_model_index, "DT_BaseViewModel", "m_nViewModelIndex" );
	NETVAR( c_handle<c_base_combat_weapon>, weapon, "DT_BaseViewModel", "m_hWeapon" );
	NETVAR(c_handle<c_base_player>, owner, "DT_BaseViewModel", "m_hOwner" );
	NETPROP( sequence, "DT_BaseViewModel", "m_nSequence" );
	void send_view_model_matching_sequence( int sequence );
};

class animation_layer
{
public:
	char  pad_0000[20];
	// These should also be present in the padding, don't see the use for it though
	//float	m_flLayerAnimtime;
	//float	m_flLayerFadeOuttime;
	uint32_t order; //0x0014
	uint32_t sequence; //0x0018
	float_t prev_cycle; //0x001C
	float_t weight; //0x0020
	float_t weight_delta_rate; //0x0024
	float_t playback_rate; //0x0028
	float_t cycle; //0x002C
	void* owner; //0x0030 // player's thisptr
	char  pad_0038[4]; //0x0034
}; //Size: 0x0038

class c_csgo_player_animstate
{
public:
	/*void* pThis;
	char pad2[91];
	void* pBaseEntity; //0x60
	void* pActiveWeapon; //0x64
	void* pLastActiveWeapon; //0x68
	float m_flLastClientSideAnimationUpdateTime; //0x6C
	int m_iLastClientSideAnimationUpdateFramecount; //0x70
	float m_flEyePitch; //0x74
	float m_flEyeYaw; //0x78
	float m_flPitch; //0x7C
	float m_flGoalFeetYaw; //0x80
	float m_flSpeedFactor; //0x00FC
	float m_flDuckAmount; //0x00A4
	float m_flSpeedFraction; //0x00F8
	float m_flLandingRatio; //0x011C
	float m_flCurrentFeetYaw; //0x84
	float m_flCurrentTorsoYaw; //0x88
	float m_flUnknownVelocityLean; //0x8C //changes when moving/jumping/hitting ground
	float m_flLeanAmount; //0x90
	char pad4[4]; //NaN
	float m_flFeetCycle; //0x98 0 to 1
	float m_flFeetYawRate; //0x9C 0 to 1
	float m_fUnknown2;
	float m_fDuckAmount; //0xA4
	float m_fLandingDuckAdditiveSomething; //0xA8
	float m_fUnknown3; //0xAC
	Vector m_vOrigin; //0xB0, 0xB4, 0xB8
	Vector m_vLastOrigin; //0xBC, 0xC0, 0xC4
	float m_vVelocityX; //0xC8
	float m_vVelocityY; //0xCC
	char pad5[4];
	float m_flUnknownFloat1; //0xD4 Affected by movement and direction
	char pad6[8];
	float m_flUnknownFloat2; //0xE0 //from -1 to 1 when moving and affected by direction
	float m_flUnknownFloat3; //0xE4 //from -1 to 1 when moving and affected by direction
	float m_unknown; //0xE8
	float speed_2d; //0xEC
	float flUpVelocity; //0xF0
	float m_flSpeedNormalized; //0xF4 //from 0 to 1
	float m_flFeetSpeedForwardsOrSideWays; //0xF8 //from 0 to 2. something  is 1 when walking, 2.something when running, 0.653 when crouch walking
	float m_flFeetSpeedUnknownForwardOrSideways; //0xFC //from 0 to 3. something
	float m_flTimeSinceStartedMoving; //0x100
	float m_flTimeSinceStoppedMoving; //0x104
	unsigned char m_bOnGround; //0x108
	unsigned char m_bInHitGroundAnimation; //0x109
	char pad7[10];
	float m_flLastOriginZ; //0x114
	float m_flMinBodyYawDegrees; //0x0330
	float m_flMaxBodyYawDegrees; //0x0334
	float m_flHeadHeightOrOffsetFromHittingGroundAnimation; //0x118 from 0 to 1, is 1 when standing
	float m_flStopToFullRunningFraction; //0x11C from 0 to 1, doesnt change when walking or crouching, only running
	char pad8[4]; //NaN
	float m_flUnknownFraction; //0x124 affected while jumping and running, or when just jumping, 0 to 1
	char pad9[4]; //NaN
	float m_flUnknown3;
	char pad10[528];*/
	void* pThis;
	char pad2[91];
	void* pBaseEntity; //0x60
	void* pActiveWeapon; //0x64
	void* pLastActiveWeapon; //0x68
	float m_flLastClientSideAnimationUpdateTime; //0x6C
	int m_iLastClientSideAnimationUpdateFramecount; //0x70
	float m_flEyePitch; //0x74
	float m_flEyeYaw; //0x78
	float m_flPitch; //0x7C
	float m_flGoalFeetYaw; //0x80
	float m_flCurrentFeetYaw; //0x84
	float m_flCurrentTorsoYaw; //0x88
	float m_flUnknownVelocityLean; //0x8C //changes when moving/jumping/hitting ground
	float m_flLeanAmount; //0x90
	char pad4[4]; //NaN
	float m_flFeetCycle; //0x98 0 to 1
	float m_flFeetYawRate; //0x9C 0 to 1
	float m_fUnknown2;
	float m_fDuckAmount; //0xA4
	float m_fLandingDuckAdditiveSomething; //0xA8
	float m_fUnknown3; //0xAC
	Vector m_vOrigin; //0xB0, 0xB4, 0xB8
	Vector m_vLastOrigin; //0xBC, 0xC0, 0xC4
	float m_vVelocityX; //0xC8
	float m_vVelocityY; //0xCC
	char pad5[4];
	float m_flUnknownFloat1; //0xD4 Affected by movement and direction
	char pad6[8];
	float m_flUnknownFloat2; //0xE0 //from -1 to 1 when moving and affected by direction
	float m_flUnknownFloat3; //0xE4 //from -1 to 1 when moving and affected by direction
	float m_unknown; //0xE8
	float speed_2d; //0xEC
	float flUpVelocity; //0xF0
	float m_flSpeedNormalized; //0xF4 //from 0 to 1
	float m_flFeetSpeedForwardsOrSideWays; //0xF8 //from 0 to 2. something  is 1 when walking, 2.something when running, 0.653 when crouch walking
	float m_flFeetSpeedUnknownForwardOrSideways; //0xFC //from 0 to 3. something
	float m_flTimeSinceStartedMoving; //0x100
	float m_flTimeSinceStoppedMoving; //0x104
	unsigned char m_bOnGround; //0x108
	unsigned char m_bInHitGroundAnimation; //0x109
	char pad7[10];
	float m_flLastOriginZ; //0x114
	float m_flHeadHeightOrOffsetFromHittingGroundAnimation; //0x118 from 0 to 1, is 1 when standing
	float m_flStopToFullRunningFraction; //0x11C from 0 to 1, doesnt change when walking or crouching, only running
	char pad8[4]; //NaN
	float m_flUnknownFraction; //0x124 affected while jumping and running, or when just jumping, 0 to 1
	char pad9[4]; //NaN
	float m_flUnknown3;
	char pad10[528];
}; //Size=0x344


class c_game_rules {
public:
	NETVAR( int, match_stats_players_alive_t, "DT_CSGameRulesProxy", "m_iMatchStats_PlayersAlive_T" );
	NETVAR( int, match_stats_Players_alive_ct, "DT_CSGameRulesProxy", "m_iMatchStats_PlayersAlive_CT" );
	NETVAR( int, round_time, "DT_CSGameRulesProxy", "m_iRoundTime" );
	NETVAR( bool, freeze_period, "DT_CSGameRulesProxy", "m_bBombDropped" );
	NETVAR( bool, is_valve_ds, "DT_CSGameRulesProxy", "m_bIsValveDS" );
	NETVAR( bool, bomb_dropped, "DT_CSGameRulesProxy", "m_bBombDropped" );
	NETVAR( bool, bomb_planted, "DT_CSGameRulesProxy", "m_bBombPlanted" );
	NETVAR( float, survival_start_time, "DT_CSGameRulesProxy", "m_flSurvivalStartTime" );
}; extern c_game_rules* g_gamerules;
