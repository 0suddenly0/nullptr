#pragma once
#include <map>
#include <string>
#include <vector>
#include "../../../valve_sdk/csgostructs.h"

static auto is_knife(const int i) -> bool
{
	return (i >= WEAPON_KNIFE_BAYONET && i < GLOVE_STUDDED_BLOODHOUND) || i == WEAPON_KNIFE_T || i == WEAPON_KNIFE;
}

struct weapon_info
{
	constexpr weapon_info(const char* model, const char* icon = nullptr) :
		model(model),
		icon(icon)
	{}

	const char* model;
	const char* icon;
};

class weapon_name
{
public:
	weapon_name(int _definition_index, const char* _name)
	{
		definition_index = _definition_index;
		name = _name;
	}

	int definition_index;
	std::string name;

	std::string _weapon_name;
	std::string skin_name;
};
extern const std::map<size_t, weapon_info> k_weapon_info;
extern std::vector<weapon_name> k_t_knife_names;
extern std::vector<weapon_name> k_t_glove_names;
extern std::vector<weapon_name> k_ct_knife_names;
extern std::vector<weapon_name> k_ct_glove_names;
extern std::vector<weapon_name> k_weapon_names;

extern const std::vector<weapon_name> k_knife_names_preview;
extern const std::vector<weapon_name> k_glove_names_preview;
extern const std::vector<weapon_name> k_weapon_names_preview;