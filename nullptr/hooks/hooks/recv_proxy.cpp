#include "../hooks.h"

static auto random_sequence(const int low, const int high) -> int {
	return rand() % (high - low + 1) + low;
}

static auto fix_animation(const char* model, const int sequence) -> int {
	enum ESequence
	{
		SEQUENCE_DEFAULT_DRAW = 0,
		SEQUENCE_DEFAULT_IDLE1 = 1,
		SEQUENCE_DEFAULT_IDLE2 = 2,
		SEQUENCE_DEFAULT_LIGHT_MISS1 = 3,
		SEQUENCE_DEFAULT_LIGHT_MISS2 = 4,
		SEQUENCE_DEFAULT_HEAVY_MISS1 = 9,
		SEQUENCE_DEFAULT_HEAVY_HIT1 = 10,
		SEQUENCE_DEFAULT_HEAVY_BACKSTAB = 11,
		SEQUENCE_DEFAULT_LOOKAT01 = 12,
		SEQUENCE_BUTTERFLY_DRAW = 0,
		SEQUENCE_BUTTERFLY_DRAW2 = 1,
		SEQUENCE_BUTTERFLY_LOOKAT01 = 13,
		SEQUENCE_BUTTERFLY_LOOKAT03 = 15,
		SEQUENCE_FALCHION_IDLE1 = 1,
		SEQUENCE_FALCHION_HEAVY_MISS1 = 8,
		SEQUENCE_FALCHION_HEAVY_MISS1_NOFLIP = 9,
		SEQUENCE_FALCHION_LOOKAT01 = 12,
		SEQUENCE_FALCHION_LOOKAT02 = 13,
		SEQUENCE_DAGGERS_IDLE1 = 1,
		SEQUENCE_DAGGERS_LIGHT_MISS1 = 2,
		SEQUENCE_DAGGERS_LIGHT_MISS5 = 6,
		SEQUENCE_DAGGERS_HEAVY_MISS2 = 11,
		SEQUENCE_DAGGERS_HEAVY_MISS1 = 12,
		SEQUENCE_BOWIE_IDLE1 = 1,
	};
	if (strstr(model, "models/weapons/v_knife_butterfly.mdl")) {
		switch (sequence) {
		case SEQUENCE_DEFAULT_DRAW:
			return random_sequence(SEQUENCE_BUTTERFLY_DRAW, SEQUENCE_BUTTERFLY_DRAW2);
		case SEQUENCE_DEFAULT_LOOKAT01:
			return random_sequence(SEQUENCE_BUTTERFLY_LOOKAT01, SEQUENCE_BUTTERFLY_LOOKAT03);
		default:
			return sequence + 1;
		}
	}
	else if (strstr(model, "models/weapons/v_knife_falchion_advanced.mdl")) {
		switch (sequence) {
		case SEQUENCE_DEFAULT_IDLE2:
			return SEQUENCE_FALCHION_IDLE1;
		case SEQUENCE_DEFAULT_HEAVY_MISS1:
			return random_sequence(SEQUENCE_FALCHION_HEAVY_MISS1, SEQUENCE_FALCHION_HEAVY_MISS1_NOFLIP);
		case SEQUENCE_DEFAULT_LOOKAT01:
			return random_sequence(SEQUENCE_FALCHION_LOOKAT01, SEQUENCE_FALCHION_LOOKAT02);
		case SEQUENCE_DEFAULT_DRAW:
		case SEQUENCE_DEFAULT_IDLE1:
			return sequence;
		default:
			return sequence - 1;
		}
	}
	else if (strstr(model, "models/weapons/v_knife_push.mdl")) {
		switch (sequence) {
		case SEQUENCE_DEFAULT_IDLE2:
			return SEQUENCE_DAGGERS_IDLE1;
		case SEQUENCE_DEFAULT_LIGHT_MISS1:
		case SEQUENCE_DEFAULT_LIGHT_MISS2:
			return random_sequence(SEQUENCE_DAGGERS_LIGHT_MISS1, SEQUENCE_DAGGERS_LIGHT_MISS5);
		case SEQUENCE_DEFAULT_HEAVY_MISS1:
			return random_sequence(SEQUENCE_DAGGERS_HEAVY_MISS2, SEQUENCE_DAGGERS_HEAVY_MISS1);
		case SEQUENCE_DEFAULT_HEAVY_HIT1:
		case SEQUENCE_DEFAULT_HEAVY_BACKSTAB:
		case SEQUENCE_DEFAULT_LOOKAT01:
			return sequence + 3;
		case SEQUENCE_DEFAULT_DRAW:
		case SEQUENCE_DEFAULT_IDLE1:
			return sequence;
		default:
			return sequence + 2;
		}
	}
	else if (strstr(model, "models/weapons/v_knife_survival_bowie.mdl")) {
		switch (sequence) {
		case SEQUENCE_DEFAULT_DRAW:
		case SEQUENCE_DEFAULT_IDLE1:
			return sequence;
		case SEQUENCE_DEFAULT_IDLE2:
			return SEQUENCE_BOWIE_IDLE1;
		default:
			return sequence - 1;
		}
	}
	else if (strstr(model, "models/weapons/v_knife_ursus.mdl"))
	{
		switch (sequence)
		{
		case SEQUENCE_DEFAULT_DRAW:
			return random_sequence(SEQUENCE_BUTTERFLY_DRAW, SEQUENCE_BUTTERFLY_DRAW2);
		case SEQUENCE_DEFAULT_LOOKAT01:
			return random_sequence(SEQUENCE_BUTTERFLY_LOOKAT01, 14);
		default:
			return sequence + 1;
		}
	}
	else if (strstr(model, "models/weapons/v_knife_skeleton.mdl"))
	{
		switch (sequence)
		{
		case SEQUENCE_DEFAULT_DRAW:
			return random_sequence(SEQUENCE_BUTTERFLY_DRAW, SEQUENCE_BUTTERFLY_DRAW2);
		case SEQUENCE_DEFAULT_LOOKAT01:
			return random_sequence(SEQUENCE_BUTTERFLY_LOOKAT01, 14);
		default:
			return sequence + 1;
		}
	}
	else if (strstr(model, "models/weapons/v_knife_outdoor.mdl"))
	{
		switch (sequence)
		{
		case SEQUENCE_DEFAULT_DRAW:
			return random_sequence(SEQUENCE_BUTTERFLY_DRAW, SEQUENCE_BUTTERFLY_DRAW2);
		case SEQUENCE_DEFAULT_LOOKAT01:
			return random_sequence(SEQUENCE_BUTTERFLY_LOOKAT01, 14);
		default:
			return sequence + 1;
		}
	}
	else if (strstr(model, "models/weapons/v_knife_canis.mdl"))
	{
		switch (sequence)
		{
		case SEQUENCE_DEFAULT_DRAW:
			return random_sequence(SEQUENCE_BUTTERFLY_DRAW, SEQUENCE_BUTTERFLY_DRAW2);
		case SEQUENCE_DEFAULT_LOOKAT01:
			return random_sequence(SEQUENCE_BUTTERFLY_LOOKAT01, 14);
		default:
			return sequence + 1;
		}
	}
	else if (strstr(model, "models/weapons/v_knife_cord.mdl"))
	{
		switch (sequence)
		{
		case SEQUENCE_DEFAULT_DRAW:
			return random_sequence(SEQUENCE_BUTTERFLY_DRAW, SEQUENCE_BUTTERFLY_DRAW2);
		case SEQUENCE_DEFAULT_LOOKAT01:
			return random_sequence(SEQUENCE_BUTTERFLY_LOOKAT01, 14);
		default:
			return sequence + 1;
		}
	}
	else if (strstr(model, "models/weapons/v_knife_stiletto.mdl"))
	{
		switch (sequence)
		{
		case SEQUENCE_DEFAULT_LOOKAT01:
			return random_sequence(12, 13);
		}
	}
	else if (strstr(model, "models/weapons/v_knife_widowmaker.mdl"))
	{
		switch (sequence)
		{
		case SEQUENCE_DEFAULT_LOOKAT01:
			return random_sequence(14, 15);
		case SEQUENCE_DEFAULT_HEAVY_BACKSTAB:
			return sequence - 1;
		}
	}
	else {
		return sequence;
	}
}

namespace hooks
{
    void recv_proxy::hook(const c_recv_proxy_data* pData, void* entity, void* output)
    {
		static auto original_fn = sequence_vhook->get_original_function();
		const auto local = static_cast<c_base_player*>(g_entity_list->get_client_entity(g_engine_client->get_local_player()));

		if (local && local->is_alive()) 
		{
			const auto proxy_data = const_cast<c_recv_proxy_data*>(pData);
			const auto view_model = static_cast<c_base_view_model*>(entity);
			if (view_model && view_model->owner() && view_model->owner().is_valid()) {
				const auto owner = static_cast<c_base_player*>(g_entity_list->get_client_entity_from_handle(view_model->owner()));
				if (owner == g_entity_list->get_client_entity(g_engine_client->get_local_player())) {
					const auto view_model_weapon_handle = view_model->weapon();
					if (view_model_weapon_handle.is_valid()) {
						const auto view_model_weapon = static_cast<c_base_attributable_item*>(g_entity_list->get_client_entity_from_handle(view_model_weapon_handle));
						if (view_model_weapon)
						{
							if (k_weapon_info.count(view_model_weapon->item().item_definition_index()))
							{
								auto original_sequence = proxy_data->value.m_int;
								const auto override_model = k_weapon_info.at(view_model_weapon->item().item_definition_index()).model;
								proxy_data->value.m_int = fix_animation(override_model, proxy_data->value.m_int);
							}
						}
					}
				}
			}
		}
		original_fn(pData, entity, output);
    }
}