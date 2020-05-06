#include "csgostructs.h"
#include "../helpers/math.h"
#include "../helpers/utils.h"

c_game_rules* g_gamerules = nullptr;

bool c_base_entity::is_player( ) {
	//index: 152
	//ref: "effects/nightvision"
	//sig: 8B 92 ? ? ? ? FF D2 84 C0 0F 45 F7 85 F6
	return call_vfunction<bool( __thiscall* )( c_base_entity* )>( this, 157)( this );
}

bool c_base_entity::is_loot( )
{
	return ( get_client_class( )->class_id == ClassId_CPhysPropAmmoBox ||
		     get_client_class( )->class_id == ClassId_CPhysPropLootCrate ||
		     get_client_class( )->class_id == ClassId_CPhysPropRadarJammer ||
	         get_client_class( )->class_id == ClassId_CPhysPropWeaponUpgrade ||
		     get_client_class( )->class_id == ClassId_CDrone ||
		     get_client_class( )->class_id == ClassId_CDronegun ||
		     get_client_class( )->class_id == ClassId_CItem_Healthshot ||
		     get_client_class( )->class_id == ClassId_CItemCash );
}

bool c_base_entity::is_weapon( )
{
	//index: 160
	//ref: "CNewParticleEffect::DrawModel"
	//sig: 8B 80 ? ? ? ? FF D0 84 C0 74 6F 8B 4D A4
	return call_vfunction<bool( __thiscall* )( c_base_entity* )>( this, 165 )( this );
}


bool c_base_entity::is_planted_c4( )
{
	return get_client_class()->class_id == 128;
}

bool c_base_entity::is_defuse_kit( ) 
{
	return get_client_class( )->class_id == ClassId_CBaseAnimating;
}
int c_base_entity::get_health()
{
	return *reinterpret_cast<int*>((DWORD)this + 0x100);
}
bool c_base_entity::new_setup_bones(matrix3x4_t* matrix)
{
	if (this == g_local_player)
	{
		const auto Backup = *(int*)(uintptr_t(this) + ptrdiff_t(0x272));

		*(int*)(uintptr_t(this) + ptrdiff_t(0x272)) = -1;

		this->setup_bones(matrix, 126, 0x00000100 | 0x200, g_global_vars->curtime);

		*(int*)(uintptr_t(this) + ptrdiff_t(0x272)) = Backup;

		return true;
	}
	else
	{
		*reinterpret_cast<int*>(uintptr_t(this) + 0xA30) = g_global_vars->framecount;
		*reinterpret_cast<int*>(uintptr_t(this) + 0xA28) = 0;

		const auto Backup = *(int*)(uintptr_t(this) + ptrdiff_t(0x272));

		*(int*)(uintptr_t(this) + ptrdiff_t(0x272)) = -1;

		this->setup_bones(matrix, 126, 0x00000100 | 0x200, g_global_vars->curtime);

		*(int*)(uintptr_t(this) + ptrdiff_t(0x272)) = Backup;

		return true;
	}
}
matrix3x4_t c_base_entity::get_bone_matrix(int BoneID)
{
	matrix3x4_t matrix;

	uintptr_t m_dwBoneMatrix = 0x26A8;
	auto offset = *reinterpret_cast<uintptr_t*>(uintptr_t(this) + m_dwBoneMatrix);
	if (offset)
		matrix = *reinterpret_cast<matrix3x4_t*>(offset + 0x30 * BoneID);

	return matrix;
}


c_cs_weapon_info* c_base_combat_weapon::get_cs_weapondata( )
{
	/*static auto fnGetWpnData
		= reinterpret_cast<c_cs_weapon_info*(__thiscall*)(void*)>(
			utils::pattern_scan(GetModuleHandleW(L"client_panorama.dll"), "55 8B EC 81 EC ? ? ? ? 53 8B D9 56 57 8D 8B ? ? ? ? 85 C9 75 04 33 FF EB 2F")
			);*/

	return g_weapon_system->get_wpn_data(this->item().item_definition_index());

	//return call_vfunc<460, c_cs_weapon_info*>(this);
}

bool c_base_combat_weapon::has_bullets( ) 
{
	return !is_reloading( ) && clip1( ) > 0;
}

bool c_base_combat_weapon::can_fire( )
{
	static decltype( this ) stored_weapon = nullptr;
	static auto stored_tick = 0;
	if( stored_weapon != this || stored_tick >= g_local_player->tick_base( ) ) {
		stored_weapon = this;
		stored_tick = g_local_player->tick_base( );
		return false; //cannot shoot first tick after switch
	}

	if(is_reloading( ) || clip1( ) <= 0 || !g_local_player )
		return false;

	auto flServerTime = g_local_player->tick_base( ) * g_global_vars->interval_per_tick;

	return next_primary_attack( ) <= flServerTime;
}

bool c_base_combat_weapon::is_grenade()
{
	return get_cs_weapondata()->weapon_type == WEAPONTYPE_GRENADE;
}


bool c_base_combat_weapon::is_gun()
{
	switch(get_cs_weapondata()->weapon_type)
	{
	case WEAPONTYPE_C4:
		return false;
	case WEAPONTYPE_GRENADE:
		return false;
	case WEAPONTYPE_KNIFE:
		return false;
	case WEAPONTYPE_UNKNOWN:
		return false;
	default:
		return true;
	}
}

bool c_base_combat_weapon::is_knife( ) {
	if( this->item( ).item_definition_index( ) == WEAPON_TASER ) return false;
	return get_cs_weapondata( )->weapon_type == WEAPONTYPE_KNIFE;
}

bool c_base_combat_weapon::is_zeus ( ) {
	if (this->item().item_definition_index() == WEAPON_TASER)
		return true;
	return false;
}

bool c_base_combat_weapon::is_rifle( ) {
	switch( get_cs_weapondata( )->weapon_type ) {
	case WEAPONTYPE_RIFLE:
		return true;
	case WEAPONTYPE_SUBMACHINEGUN:
		return true;
	case WEAPONTYPE_SHOTGUN:
		return true;
	case WEAPONTYPE_MACHINEGUN:
		return true;
	default:
		return false;
	}
}

bool c_base_combat_weapon::is_pistol( ) {
	switch( get_cs_weapondata( )->weapon_type ) {
	case WEAPONTYPE_PISTOL:
		return true;
	default:
		return false;
	}
}

int c_base_combat_weapon::get_item_definition_index()
{
	if (!this)
		return 0;

	return this->item().item_definition_index();
}

bool c_base_combat_weapon::is_sniper( ) {
	switch( get_cs_weapondata( )->weapon_type ) {
	case WEAPONTYPE_SNIPER_RIFLE:
		return true;
	default:
		return false;
	}
}

bool c_base_combat_weapon::is_reloading( )
{
	static auto inReload = *( uint32_t* )( utils::pattern_scan( GetModuleHandleW( L"client_panorama.dll" ), "C6 87 ? ? ? ? ? 8B 06 8B CE FF 90" ) + 2 );
	return *( bool* )( ( uintptr_t )this + inReload );
}

float c_base_combat_weapon::get_inaccuracy( ) {
	return call_vfunction<float( __thiscall* )( void* )>( this, 482)( this );
}

float c_base_combat_weapon::get_spread( ) {
	return call_vfunction<float( __thiscall* )( void* )>( this, 452)( this );
}

void c_base_combat_weapon::update_accuracy_penalty( ) {
	call_vfunction<void( __thiscall* )( void* )>( this, 483)( this );
}

CUtlVector<i_ref_counted*>& c_base_combat_weapon::custom_materials( ) {
	static auto inReload = *( uint32_t* )(utils::pattern_scan( GetModuleHandleW( L"client_panorama.dll" ), "83 BE ? ? ? ? ? 7F 67" ) + 2 ) - 12;
	return *( CUtlVector<i_ref_counted*>* )( ( uintptr_t )this + inReload );
}

bool* c_base_combat_weapon::custom_material_initialized( ) {
	static auto currentCommand = *( uint32_t* )(utils::pattern_scan( GetModuleHandleW( L"client_panorama.dll" ), "C6 86 ? ? ? ? ? FF 50 04" ) + 2 );
	return ( bool* )( ( uintptr_t )this + currentCommand );
}

c_usercmd*& c_base_player::current_command( ) {
	static auto currentCommand = *( uint32_t* )(utils::pattern_scan( GetModuleHandleW( L"client_panorama.dll" ), "89 BE ? ? ? ? E8 ? ? ? ? 85 FF" ) + 2 );
	return *( c_usercmd** )( ( uintptr_t )this + currentCommand );
}

int c_base_player::get_num_anim_overlays( ) 
{
	return *( int* )( ( DWORD )this + 0x298C );
}

Vector& c_base_player::get_abs_angles2()
{
	return call_vfunction<Vector& (__thiscall*)(void*)>(this, 11)(this);
}
Vector& c_base_player::get_abs_origin()
{
	return call_vfunction<Vector& (__thiscall*)(void*)>(this, 10)(this);
}


void c_base_player::set_angle2(Vector wantedang)
{
	typedef void(__thiscall * SetAngleFn)(void*, const Vector&);
	static SetAngleFn SetAngle = (SetAngleFn)((DWORD)utils::pattern_scan(GetModuleHandleW(L"client_panorama.dll"), "55 8B EC 83 E4 F8 83 EC 64 53 56 57 8B F1"));
	SetAngle(this, wantedang);
}

animation_layer *c_base_player::get_anim_overlays( ) {
	return *( animation_layer** )( ( DWORD )this + 0x2980 );
}

animation_layer *c_base_player::get_anim_overlay( int i ) {
	if( i < 15 )
		return &get_anim_overlays( )[i];
	return nullptr;
}

int c_base_player::get_sequence_activity( int sequence )
{
	auto hdr = g_mdl_info->get_studiomodel( this->get_model( ) );

	if( !hdr )
		return -1;

	// sig for stuidohdr_t version: 53 56 8B F1 8B DA 85 F6 74 55
	// sig for C_BaseAnimating version: 55 8B EC 83 7D 08 FF 56 8B F1 74 3D
	// c_csplayer vfunc 242, follow calls to find the function.

	static auto get_sequence_activity = reinterpret_cast< int( __fastcall* )( void*, studiohdr_t*, int ) >(utils::pattern_scan( GetModuleHandleA( "client_panorama.dll" ), "55 8B EC 53 8B 5D 08 56 8B F1 83" ) );

	return get_sequence_activity( this, hdr, sequence );
}

c_csgo_player_animstate *c_base_player::get_player_anim_state( ) {
	//static auto m_bIsScoped = NetvarSys::Get( ).GetOffset( "DT_CSPlayer", "m_bIsScoped" );
	//return *( c_csgo_player_animstate** )( ( DWORD )this + ( m_bIsScoped - 0xA ) );
	return *( c_csgo_player_animstate** )( ( DWORD )this + 0x3900 );
}

void c_base_player::update_animation_state( c_csgo_player_animstate *state, QAngle angle ) {
	static auto UpdateAnimState = utils::pattern_scan(GetModuleHandleA( "client_panorama.dll" ), "55 8B EC 83 E4 F8 83 EC 18 56 57 8B F9 F3 0F 11 54 24" );

	if( !UpdateAnimState )
		return;

	__asm {
		push 0
	}

	__asm
	{
		mov ecx, state

		movss xmm1, dword ptr[angle + 4]
		movss xmm2, dword ptr[angle]

		call UpdateAnimState
	}
}

void c_base_player::reset_animation_state( c_csgo_player_animstate *state ) {
	using ResetAnimState_t = void( __thiscall* )( c_csgo_player_animstate* );
	static auto reset_anim_state = ( ResetAnimState_t )utils::pattern_scan( GetModuleHandleA( "client_panorama.dll" ), "56 6A 01 68 ? ? ? ? 8B F1" );
	if( !reset_anim_state)
		return;

	reset_anim_state( state );
}

void c_base_player::create_animation_state( c_csgo_player_animstate *state ) {
	using CreateAnimState_t = void( __thiscall* )( c_csgo_player_animstate*, c_base_player* );
	static auto CreateAnimState = ( CreateAnimState_t )utils::pattern_scan( GetModuleHandleA( "client_panorama.dll" ), "55 8B EC 56 8B F1 B9 ? ? ? ? C7 46" );
	if( !CreateAnimState )
		return;

	CreateAnimState( state, this );
}

Vector c_base_player::get_eye_pos( ) {
	auto duckAmount = this->duck_amount( );
	return vec_origin( ) + Vector( 0.0f, 0.0f, ( ( 1.0f - duckAmount ) * 18.0f ) + 46.0f );
}

player_info_t c_base_player::get_player_info( ) {
	player_info_t info;
	g_engine_client->get_player_info( ent_index( ), &info );
	return info;
}

bool c_base_player::is_alive( ) {
	return life_state( ) == LIFE_ALIVE;
}

bool c_base_player::is_flashed(int min_alpha)
{
	/*static auto m_flFlashMaxAlpha = NetvarSys::Get( ).GetOffset( "DT_CSPlayer", "m_flFlashMaxAlpha" );
	return *( float* )( ( uintptr_t )this + m_flFlashMaxAlpha - 0x8 ) > (float)min_alpha;*/

	return flash_duration() > (float)min_alpha;
}

bool c_base_player::has_c4( ) {
	static auto fnHasC4
		= reinterpret_cast< bool( __thiscall* )( void* ) >(utils::pattern_scan( GetModuleHandleW( L"client_panorama.dll" ), "56 8B F1 85 F6 74 31" ));

	return fnHasC4( this );
}

Vector c_base_player::get_hitbox_pos( int hitbox_id ) {
	matrix3x4_t boneMatrix[MAXSTUDIOBONES];

	if( setup_bones( boneMatrix, MAXSTUDIOBONES, BONE_USED_BY_HITBOX, 0.0f ) ) {
		auto studio_model = g_mdl_info->get_studiomodel( get_model( ) );
		if( studio_model ) {
			auto hitbox = studio_model->get_hitbox_set( 0 )->get_hitbox( hitbox_id );
			if( hitbox ) {
				auto
					min = Vector{},
					max = Vector{};

				math::vector_transform( hitbox->bbmin, boneMatrix[hitbox->bone], min );
				math::vector_transform( hitbox->bbmax, boneMatrix[hitbox->bone], max );

				return ( min + max ) / 2.0f;
			}
		}
	}
	return Vector{};
}

mstudiobbox_t* c_base_player::get_hitbox( int hitbox_id ) {
	matrix3x4_t boneMatrix[MAXSTUDIOBONES];

	if(setup_bones( boneMatrix, MAXSTUDIOBONES, BONE_USED_BY_HITBOX, 0.0f ) ) {
		auto studio_model = g_mdl_info->get_studiomodel(get_model( ) );
		if( studio_model ) {
			auto hitbox = studio_model->get_hitbox_set( 0 )->get_hitbox( hitbox_id );
			if( hitbox ) {
				return hitbox;
			}
		}
	}
	return nullptr;
}

bool c_base_player::get_hitboxPos( int hitbox, Vector &output ) {
	if( hitbox >= HITBOX_MAX )
		return false;

	const model_t *model = this->get_model( );
	if( !model )
		return false;

	studiohdr_t *studioHdr = g_mdl_info->get_studiomodel( model );
	if( !studioHdr )
		return false;

	matrix3x4_t matrix[MAXSTUDIOBONES];
	if( !this->setup_bones( matrix, MAXSTUDIOBONES, 0x100, 0 ) )
		return false;

	mstudiobbox_t *studioBox = studioHdr->get_hitbox_set( 0 )->get_hitbox( hitbox );
	if( !studioBox )
		return false;

	Vector min, max;

	math::vector_transform( studioBox->bbmin, matrix[studioBox->bone], min );
	math::vector_transform( studioBox->bbmax, matrix[studioBox->bone], max );

	output = ( min + max ) * 0.5f;

	return true;
}

bool can_use_precached = false;
matrix3x4_t precached_matrix[MAXSTUDIOBONES];
mstudiohitboxset_t* precached_studio_box_set;
void c_base_player::precace_optimized_hitboxes()
{
	can_use_precached = false;
	const model_t* model = this->get_model();
	if (!model)
	{
		return;
	}

	studiohdr_t* studioHdr = g_mdl_info->get_studiomodel(model);
	if (!studioHdr)
	{
		return;
	}

	if (!this->setup_bones(precached_matrix, MAXSTUDIOBONES, 0x100, 0))
	{
		return;
	}

	precached_studio_box_set = studioHdr->get_hitbox_set(0);
	if (!precached_studio_box_set)
	{
		return;
	}
	can_use_precached = true;
}

bool c_base_player::get_optimized_hitbox_pos(int hitbox, Vector& output)
{
	if (!can_use_precached)
	{
		return false;
	}
	if (hitbox >= HITBOX_MAX)
	{
		return false;
	}

	Vector min, max;

	mstudiobbox_t* studioBox = precached_studio_box_set->get_hitbox(hitbox);
	if (!studioBox)
	{
		return false;
	}

	math::vector_transform(studioBox->bbmin, precached_matrix[studioBox->bone], min);
	math::vector_transform(studioBox->bbmax, precached_matrix[studioBox->bone], max);

	output = (min + max) * 0.5f;

	return true;
}

Vector c_base_player::get_bone_pos( int bone ) {
	matrix3x4_t boneMatrix[MAXSTUDIOBONES];
	if( setup_bones( boneMatrix, MAXSTUDIOBONES, BONE_USED_BY_ANYTHING, 0.0f ) ) {
		return boneMatrix[bone].at( 3 );
	}
	return Vector{};
}

bool c_base_player::can_see_player( c_base_player* player, int hitbox ) {
	c_game_trace tr;
	ray_t ray;
	c_tracefilter filter;
	filter.pSkip = this;

	auto endpos = player->get_hitbox_pos( hitbox );

	ray.init( get_eye_pos( ), endpos );
	g_engine_trace->trace_ray( ray, MASK_SHOT | CONTENTS_GRATE, &filter, &tr );

	return tr.hit_entity == player || tr.fraction > 0.97f;
}

bool c_base_player::can_see_player( c_base_player* player, const Vector& pos ) {
	c_game_trace tr;
	ray_t ray;
	c_tracefilter filter;
	filter.pSkip = this;

	ray.init(get_eye_pos( ), pos );
	g_engine_trace->trace_ray( ray, MASK_SHOT | CONTENTS_GRATE, &filter, &tr );

	return tr.hit_entity == player || tr.fraction > 0.9f;
}

float c_base_player::can_see_player(const Vector& pos)
{
	c_game_trace tr;
	ray_t ray;
	c_tracefilter filter;
	filter.pSkip = this;

	ray.init(get_eye_pos(), pos);
	g_engine_trace->trace_ray(ray, MASK_SHOT, &filter, &tr);

	return tr.fraction /*> 0.9f*/;
}

void c_base_player::update_client_side_animation( ) {
	return call_vfunction< void( __thiscall* )( void* ) >( this, 223)( this );
}

float c_base_player::get_max_desync_delta()
{
	auto animstate = uintptr_t(this->get_player_anim_state());

	float duckammount = *(float*)(animstate + 0xA4);
	float speedfraction = std::fmax(0, std::fmin(*reinterpret_cast<float*>(animstate + 0xF8), 1));

	float speedfactor = std::fmax(0, std::fmin(1, *reinterpret_cast<float*> (animstate + 0xFC)));

	float unk1 = ((*reinterpret_cast<float*> (animstate + 0x11C) * -0.30000001) - 0.19999999)* speedfraction;
	float unk2 = unk1 + 1.f;
	float unk3;

	if (duckammount > 0) {

		unk2 += ((duckammount * speedfactor) * (0.5f - unk2));

	}

	unk3 = *(float*)(animstate + 0x334) * unk2;

	return unk3;
}

void c_base_player::invalidate_bone_cache( ) {
	static DWORD addr = ( DWORD )utils::pattern_scan( GetModuleHandleA( "client_panorama.dll" ), "80 3D ? ? ? ? ? 74 16 A1 ? ? ? ? 48 C7 81" );

	*( int* )( ( uintptr_t )this + 0xA30 ) = g_global_vars->framecount; //we'll skip occlusion checks now
	*( int* )( ( uintptr_t )this + 0xA28 ) = 0;//clear occlusion flags

	unsigned long g_iModelBoneCounter = **( unsigned long** )( addr + 10 );
	*( unsigned int* )( ( DWORD )this + 0x2924 ) = 0xFF7FFFFF; // m_flLastBoneSetupTime = -FLT_MAX;
	*( unsigned int* )( ( DWORD )this + 0x2690 ) = ( g_iModelBoneCounter - 1 ); // m_iMostRecentModelBoneCounter = g_iModelBoneCounter - 1;
}

int& c_base_player::move_type( ) {
	return *( int* )( ( uintptr_t )this + 0x25c );
}

QAngle* c_base_player::get_vangles( ) {
	static auto deadflag = netvar_sys::get_offset( "DT_BasePlayer", "deadflag" );
	return (QAngle*)((uintptr_t)this + deadflag + 0x4);
}

void c_base_attributable_item::set_glove_model_index( int modelIndex ) {
	return call_vfunction<void( __thiscall* )( void*, int )>( this, 75 )( this, modelIndex );
}

void c_base_view_model::send_view_model_matching_sequence( int sequence ) {
	return call_vfunction<void( __thiscall* )( void*, int )>( this, 241 )( this, sequence );
}

float_t c_base_player::spawn_time( ) {
	// 0xA360
	//static auto m_iAddonBits = NetvarSys::Get( ).GetOffset( "DT_CSPlayer", "m_iAddonBits" );
	//return *( float_t* )( ( uintptr_t )this + m_iAddonBits - 0x4 );
	return *( float_t* )( ( uintptr_t )this + 0xA360 );
}

std::string c_base_player::get_name()
{
	bool console_safe;
	// Cleans player's name so we don't get new line memes. Use this everywhere you get the players name.
	// Also, if you're going to use the console for its command and use the players name, set console_safe.
	player_info_t pinfo = this->get_player_info();

	char* pl_name = pinfo.szName;
	char buf[128];
	int c = 0;

	for (int i = 0; pl_name[i]; ++i)
	{
		if (c >= sizeof(buf) - 1)
			break;

		switch (pl_name[i])
		{
		case '"': if (console_safe) break;
		case '\\':
		case ';': if (console_safe) break;
		case '\n':
			break;
		default:
			buf[c++] = pl_name[i];
		}
	}

	buf[c] = '\0';
	return std::string(buf);
}

bool c_base_player::is_not_target( ) {
	if( !this || this == g_local_player )
		return true;

	if( health( ) <= 0 )
		return true;

	if( gun_game_immunity( ) )
		return true;

	if(m_flags( ) & FL_FROZEN )
		return true;

	int _ent_index = ent_index();
	return _ent_index > g_global_vars->maxClients;
}
