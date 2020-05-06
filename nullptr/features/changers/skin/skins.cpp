#include "skins.h"

ImVec4 skins::get_color_ratiry(int rar)
{
	switch (rar)
	{
	case 1:
		return ImColor(150, 150, 150, 255);
		break;
	case 2:
		return ImColor(100, 100, 255, 255);
		break;
	case 3:
		return ImColor(50, 50, 255, 255);
		break;
	case 4:
		return ImColor(255, 64, 242, 255);
		break;
	case 5:
		return ImColor(255, 50, 50, 255);
		break;
	case 6:
		return ImColor(255, 50, 50, 255);
		break;
	case 7:
		return ImColor(255, 196, 46, 255);
		break;
	default:
		return ImColor(150, 150, 150, 255);
		break;
	}
}

static auto erase_override_if_exists_by_index(const int definition_index) -> void
{
	if (k_weapon_info.count(definition_index))
	{
		auto& icon_override_map = settings::changers::skin::m_icon_overrides;
		const auto& original_item = k_weapon_info.at(definition_index);
		if (original_item.icon && icon_override_map.count(original_item.icon))
			icon_override_map.erase(icon_override_map.at(original_item.icon)); // Remove the leftover override
	}
}

bool get_skins_cur(int weaponidx)
{
	return true;
}

const char* skins::get_icon_override(const std::string original)
{
	return settings::changers::skin::m_icon_overrides.count(original) ? settings::changers::skin::m_icon_overrides.at(original).data() : nullptr;
}

static auto apply_config_on_attributable_item(c_base_attributable_item* item, const item_setting* config,
	const unsigned xuid_low) -> void
{
	/*if (config->paint_kit_index < 0)
	{
		return;
	}*/

	item->item().item_id_high() = -1;
	item->item().account_id() = xuid_low;
	if (config->custom_name[0])
		strcpy_s(item->item().custom_name(), config->custom_name);
	if (config->paint_kit_index)
		item->fallback_paint_kit() = config->paint_kit_index;
	if (config->seed)
		item->fallback_seed() = config->seed;
	if (config->stat_trak) {
		item->fallback_stat_trak() = settings::changers::skin::statrack_items[config->definition_index].statrack_new.counter;
		item->item().entity_quality() = 9;
	}
	else {
		item->item().entity_quality() = is_knife(config->definition_index) ? 3 : 0;
	}
	item->fallback_wear() = config->wear;
	auto& definition_index = item->item().item_definition_index();
	auto& icon_override_map = settings::changers::skin::m_icon_overrides;
	if (config->definition_override_index && config->definition_override_index != definition_index && k_weapon_info.count(config->definition_override_index))
	{
		const auto old_definition_index = definition_index;
		definition_index = config->definition_override_index;
		const auto& replacement_item = k_weapon_info.at(config->definition_override_index);
		item->model_index() = g_mdl_info->get_model_index(replacement_item.model);
		item->set_module_index(g_mdl_info->get_model_index(replacement_item.model));
		item->get_client_networkable()->pre_data_update(0);
		if (old_definition_index && k_weapon_info.count(old_definition_index))
		{
			const auto& original_item = k_weapon_info.at(old_definition_index);
			if (original_item.icon && replacement_item.icon) 
			{
				icon_override_map[original_item.icon] = replacement_item.icon;
			}
		}
	}
	else
	{
		erase_override_if_exists_by_index(definition_index);
	}
}
static auto get_wearable_create_fn() -> Createclient_classFn
{
	auto clazz = g_chl_client->get_all_classes();
	// Please, if you gonna paste it into a cheat use classids here. I use names because they
	// won't change in the foreseeable future and i dont need high speed, but chances are
	// you already have classids, so use them instead, they are faster.
	while (strcmp(clazz->network_name, "CEconWearable"))
		clazz = clazz->next;
	return clazz->create_fn;
}

void skins::on_frame_stage_notify(bool frame_end)
{
	const auto local_index = g_engine_client->get_local_player();
	const auto local = static_cast<c_base_player*>(g_entity_list->get_client_entity(local_index));
	if (!local)
		return;
	player_info_t player_info;
	if (!g_engine_client->get_player_info(local_index, &player_info))
		return;


	if(frame_end)
	{
		const auto wearables = local->my_wearables();
		const auto glove_config = &settings::changers::skin::m_items[local->team_num() == 3 ? GLOVE_CT_SIDE : GLOVE_T_SIDE];
		static auto glove_handle = c_base_handle(0);
		auto glove = reinterpret_cast<c_base_attributable_item*>(g_entity_list->get_client_entity_from_handle(wearables[0]));
		if (!glove)
		{
			const auto our_glove = reinterpret_cast<c_base_attributable_item*>(g_entity_list->get_client_entity_from_handle(glove_handle));
			if (our_glove) // Our glove still exists
			{
				wearables[0] = glove_handle;
				glove = our_glove;
			}
		}
		if (!local->is_alive())
		{
			if (glove)
			{
				glove->get_client_networkable()->set_destroyed_on_recreate_entities();
				glove->get_client_networkable()->release();
			}
			return;
		}
		if (glove_config && glove_config->definition_override_index)
		{
			if (!glove)
			{
				static auto create_wearable_fn = get_wearable_create_fn();
				const auto entry = g_entity_list->get_highest_entity_index() + 1;
				const auto serial = rand() % 0x1000;
				//glove = static_cast<C_BaseAttributableItem*>(create_wearable_fn(entry, serial));
				create_wearable_fn(entry, serial);
				glove = reinterpret_cast<c_base_attributable_item*>(g_entity_list->get_client_entity(entry));
				assert(glove);
				{
					static auto set_abs_origin_addr = utils::pattern_scan(GetModuleHandleA("client_panorama.dll"), "55 8B EC 83 E4 F8 51 53 56 57 8B F1 E8");
					const auto set_abs_origin_fn = reinterpret_cast<void(__thiscall*)(void*, const std::array<float, 3>&)>(set_abs_origin_addr);
					static constexpr std::array<float, 3> new_pos = { 10000.f, 10000.f, 10000.f };
					set_abs_origin_fn(glove, new_pos);
				}
				const auto wearable_handle = reinterpret_cast<c_base_handle*>(&wearables[0]);
				*wearable_handle = entry | serial << 16;
				glove_handle = wearables[0];
			}
			// Thanks, Beakers
			glove->set_glove_model_index(-1);
			apply_config_on_attributable_item(glove, glove_config, player_info.xuid_low);
		}
	}
	else
	{
		auto weapons = local->my_weapons();
		for (int i = 0; weapons[i].is_valid(); i++)
		{
			c_base_attributable_item* weapon = (c_base_attributable_item*)g_entity_list->get_client_entity_from_handle(weapons[i]);
			if (!weapon)
				continue;
			auto& definition_index = weapon->item().item_definition_index();
			const auto active_conf = &settings::changers::skin::m_items[is_knife(definition_index) ? (local->team_num() == 3 ? WEAPON_KNIFE : WEAPON_KNIFE_T) : definition_index];
			apply_config_on_attributable_item(weapon, active_conf, player_info.xuid_low);
		}
	}
	const auto view_model_handle = local->view_model();
	if (!view_model_handle.is_valid())
		return;
	const auto view_model = static_cast<c_base_view_model*>(g_entity_list->get_client_entity_from_handle(view_model_handle));
	if (!view_model)
		return;
	const auto view_model_weapon_handle = view_model->weapon();
	if (!view_model_weapon_handle.is_valid())
		return;
	const auto view_model_weapon = static_cast<c_base_combat_weapon*>(g_entity_list->get_client_entity_from_handle(view_model_weapon_handle));
	if (!view_model_weapon)
		return;
	if (k_weapon_info.count(view_model_weapon->item().item_definition_index()))
	{
		const auto override_model = k_weapon_info.at(view_model_weapon->item().item_definition_index()).model;
		auto override_model_index = g_mdl_info->get_model_index(override_model);

		view_model->model_index() = override_model_index;
		auto world_model_handle = view_model_weapon->weapon_world_model();
		if (!world_model_handle.is_valid())
			return;
		const auto world_model = static_cast<c_base_weapon_world_model*>(g_entity_list->get_client_entity_from_handle(world_model_handle));
		if (!world_model)
			return;
		world_model->model_index() = override_model_index + 1;

	}
}