#include "../menu.h"

static int selected_tab_skins = 0;
char* skinchanger_tabs[] = { "general", "paint kit" };

struct hud_weapons_t {
	std::int32_t* get_weapon_count() {
		return reinterpret_cast<std::int32_t*>(std::uintptr_t(this) + 0x80);
	}
};
template<class T>
static T* FindHudElement(const char* name)
{
	static auto pThis = *reinterpret_cast<DWORD**>(utils::pattern_scan(GetModuleHandleA("client_panorama.dll"), "B9 ? ? ? ? E8 ? ? ? ? 8B 5D 08") + 1);

	static auto find_hud_element = reinterpret_cast<DWORD(__thiscall*)(void*, const char*)>(utils::pattern_scan(GetModuleHandleA("client_panorama.dll"), "55 8B EC 53 8B 5D 08 56 57 8B F9 33 F6 39 77 28"));
	return (T*)find_hud_element(pThis, name);
}

bool this_knife(int idx)
{
	return idx == WEAPON_KNIFE_T || idx == WEAPON_KNIFE;
}
bool this_glove(int idx)
{
	return idx == GLOVE_CT_SIDE || idx == GLOVE_T_SIDE;
}
bool this_ct(int idx)
{
	return idx == GLOVE_CT_SIDE || idx == WEAPON_KNIFE;
}
bool this_t(int idx)
{
	return idx == GLOVE_T_SIDE || idx == WEAPON_KNIFE_T;
}


namespace menu
{
	void tabs::skins_tab()
	{
		auto& entries = settings::changers::skin::m_items;
		ImGui::Columns(2, nullptr, false);
		ImGui::BeginChild("weapon select##skin window", ImVec2(0.f, 0.f), true, ImGuiWindowFlags_ChildWindowTitle);
		{
			ImGui::ListBoxHeader("weapons##sjinstab", ImVec2(0, 306));
			{
				for (size_t w = 0; w < k_weapon_names.size(); w++)
				{
					switch (w)
					{
					case 0:
						ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.f), "knfe");
						break;
					case 2:
						ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.f), "glove");
						break;
					case 4:
						ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.f), "pistols");
						break;
					case 14:
						ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.f), "semi-rifle");
						break;
					case 21:
						ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.f), "rifle");
						break;
					case 28:
						ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.f), "sniper-rifle");
						break;
					case 32:
						ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.f), "machingun");
						break;
					case 34:
						ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.f), "shotgun");
						break;
					}

					if (ImGui::Selectable(k_weapon_names[w].name.c_str(), definition_vector_index == w))
					{
						definition_vector_index = w;
					}
				}
			}
			ImGui::ListBoxFooter();

			static float next_enb_time = 0;

			float time_to_next_up = g_global_vars->curtime;

			if (next_enb_time - g_global_vars->curtime < 2.f)
				time_to_next_up = std::clamp(next_enb_time - g_global_vars->curtime, 0.f, 1.f);
			else
			{
				next_enb_time = g_global_vars->curtime;
			}

			std::string name = "update (";
			name += std::to_string(time_to_next_up);
			name.erase(12, 16);
			name += ")";

			if (ImGui::ButtonS(name.c_str(), ImVec2(221, 0)))
			{
				if (next_enb_time < g_global_vars->curtime)
				{
					static auto clear_hud_weapon_icon_fn =
						reinterpret_cast<std::int32_t(__thiscall*)(void*, std::int32_t)>(
							utils::pattern_scan(GetModuleHandleA("client_panorama.dll"), "55 8B EC 51 53 56 8B 75 08 8B D9 57 6B FE 2C 89 5D FC"));

					auto element = FindHudElement<std::uintptr_t*>("CCSGO_HudWeaponSelection");

					if (element)
					{
						auto hud_weapons = reinterpret_cast<hud_weapons_t*>(std::uintptr_t(element) - 0xa0);
						if (hud_weapons != nullptr)
						{

							if (*hud_weapons->get_weapon_count())
							{
								for (std::int32_t i = 0; i < *hud_weapons->get_weapon_count(); i++)
									i = clear_hud_weapon_icon_fn(hud_weapons, i);

								typedef void(*ForceUpdate) (void);
								static ForceUpdate FullUpdate = (ForceUpdate)utils::pattern_scan(GetModuleHandleA("engine.dll"), "A1 ? ? ? ? B9 ? ? ? ? 56 FF 50 14 8B 34 85");
								FullUpdate();

								g_client_state->force_full_update();
							}
						}
					}

					next_enb_time = g_global_vars->curtime + 1.f;
				}
			}
		}
		ImGui::EndChild("weapon select");

		ImGui::NextColumn();

		ImGui::BeginChild("skin select##skin tab", ImVec2(0, 0), true, ImGuiWindowFlags_ChildWindowTitle);
		{
			auto& selected_entry = entries[k_weapon_names[definition_vector_index].definition_index];
			auto& satatt = settings::changers::skin::statrack_items[k_weapon_names[definition_vector_index].definition_index];
			selected_entry.definition_index = k_weapon_names[definition_vector_index].definition_index;
			selected_entry.definition_vector_index = definition_vector_index;

			horizontal_tabs(selected_tab_skins, skinchanger_tabs);

			std::vector<weapon_name>& cur_knifes = this_ct(selected_entry.definition_index) ? k_ct_knife_names : k_t_knife_names;
			std::vector<weapon_name>& cur_gloves = this_ct(selected_entry.definition_index) ? k_ct_glove_names : k_t_glove_names;

			if (!this_knife(selected_entry.definition_index) && !this_glove(selected_entry.definition_index)) k_weapon_names.at(definition_vector_index)._weapon_name = k_weapon_names_preview[definition_vector_index].name;
			else if (this_glove(selected_entry.definition_index)) cur_gloves.at(selected_entry.definition_override_vector_index)._weapon_name = k_glove_names_preview.at(selected_entry.definition_override_vector_index).name;
			else if (this_knife(selected_entry.definition_index)) cur_knifes.at(selected_entry.definition_override_vector_index)._weapon_name = k_knife_names_preview.at(selected_entry.definition_override_vector_index).name;
			
			if (selected_tab_skins == 0)
			{
				ImGui::Checkbox("skin preview", &settings::changers::skin::skin_preview);
				ImGui::Checkbox("stattrak##2", &selected_entry.stat_trak);
				ImGui::InputInt("seed", &selected_entry.seed);
				ImGui::InputInt("stattrak", &satatt.statrack_new.counter);
				ImGui::SliderFloat("wear", &selected_entry.wear, FLT_MIN, 1.f, "%.10f", 5);

				if (this_knife(selected_entry.definition_index))
				{
					ImGui::ListBoxHeader("knife##list_kinfes", ImVec2(0, 152));
					{
						for (int i = 0; i < cur_knifes.size(); i++)
						{
							if (ImGui::Selectable(cur_knifes[i].name.c_str(), selected_entry.definition_override_vector_index == i))
							{
								selected_entry.definition_override_vector_index = i;
							}
						}

						selected_entry.definition_override_index = cur_knifes.at(selected_entry.definition_override_vector_index).definition_index;
					}
					ImGui::ListBoxFooter();
				}
				else if (selected_entry.definition_index == GLOVE_T_SIDE || selected_entry.definition_index == GLOVE_CT_SIDE)
				{
					ImGui::ListBoxHeader("glove##list_gloves", ImVec2(0, 152));
					{
						for (int i = 0; i < cur_gloves.size(); i++)
						{
							if (ImGui::Selectable(cur_gloves[i].name.c_str(), selected_entry.definition_override_vector_index == i))
							{
								selected_entry.definition_override_vector_index = i;
							}
						}

						selected_entry.definition_override_index = cur_gloves.at(selected_entry.definition_override_vector_index).definition_index;
					}
					ImGui::ListBoxFooter();
				}
				else {
					static auto unused_value = 0;
					selected_entry.definition_override_vector_index = 0;
				}
			}

			else if (selected_tab_skins == 1)
			{
				if (skins_parsed)
				{
					static char filter_name[32];
					std::string filter = filter_name;

					int cur_weapidx = 0;
					if (!this_glove(selected_entry.definition_index) && !this_knife(selected_entry.definition_index)) cur_weapidx = k_weapon_names[definition_vector_index].definition_index;
					else if (this_glove(selected_entry.definition_index)) cur_weapidx = cur_gloves.at(selected_entry.definition_override_vector_index).definition_index;
					if (this_knife(selected_entry.definition_index)) cur_weapidx = cur_knifes.at(selected_entry.definition_override_vector_index).definition_index;
					

					ImGui::InputText("name filter [?]", filter_name, sizeof(filter_name));
					if (ImGui::ItemsToolTipBegin("##skinfilter"))
					{
						ImGui::Checkbox("show skins for selected weapon", &settings::changers::skin::show_cur);
						ImGui::ItemsToolTipEnd();
					}

					auto weaponName = weaponnames(cur_weapidx);

					ImGui::ListBoxHeader("skins##sdsdadsdadas", ImVec2(0, 261));
					{
						if (selected_entry.definition_index != GLOVE_T_SIDE && selected_entry.definition_index != GLOVE_CT_SIDE)
						{
							if (ImGui::Selectable(" - ", selected_entry.paint_kit_index == -1))
							{
								selected_entry.paint_kit_vector_index = -1;
								selected_entry.paint_kit_index = -1;

								if(!this_knife(selected_entry.definition_index)) k_weapon_names.at(definition_vector_index).skin_name = "empty";
								else cur_knifes.at(selected_entry.definition_override_vector_index).skin_name = "empty";
							}

							int lastID = ImGui::GetItemID();
							for (size_t w = 0; w < k_skins.size(); w++)
							{
								for (auto names : k_skins[w].weaponName)
								{
									std::string name = k_skins[w].name;

									if (settings::changers::skin::show_cur)
									{
										if (names != weaponName)
											continue;
									}

									if (name.find(filter) != name.npos)
									{
										ImGui::PushID(lastID++);

										ImGui::PushStyleColor(ImGuiCol_Text, skins::get_color_ratiry(this_knife(selected_entry.definition_index) && settings::changers::skin::show_cur ? 6 : k_skins[w].rarity));
										{
											if (ImGui::Selectable(name.c_str(), selected_entry.paint_kit_vector_index == w))
											{
												selected_entry.paint_kit_vector_index = w;
												selected_entry.paint_kit_index = k_skins[selected_entry.paint_kit_vector_index].id;

												if (!this_knife(selected_entry.definition_index)) k_weapon_names.at(definition_vector_index).skin_name = k_skins[w].name_short;
												else cur_knifes.at(selected_entry.definition_override_vector_index).skin_name = k_skins[w].name_short;
											}
										}
										ImGui::PopStyleColor();

										ImGui::PopID();
									}
								}
							}
						}
						else
						{
							int lastID = ImGui::GetItemID();

							if (ImGui::Selectable(" - ", selected_entry.paint_kit_index == -1))
							{
								selected_entry.paint_kit_vector_index = -1;
								selected_entry.paint_kit_index = -1;

								cur_gloves.at(definition_vector_index).skin_name = "empty";
							}

							for (size_t w = 0; w < k_gloves.size(); w++)
							{
								for (auto names : k_gloves[w].weaponName)
								{
									std::string name = k_gloves[w].name;

									if (settings::changers::skin::show_cur)
									{

										if (names != weaponName)
											continue;
									}

									if (name.find(filter) != name.npos)
									{
										ImGui::PushID(lastID++);

										ImGui::PushStyleColor(ImGuiCol_Text, skins::get_color_ratiry(6));
										{
											if (ImGui::Selectable(name.c_str(), selected_entry.paint_kit_vector_index == w))
											{
												selected_entry.paint_kit_vector_index = w;
												selected_entry.paint_kit_index = k_gloves[selected_entry.paint_kit_vector_index].id;

												cur_gloves.at(selected_entry.definition_override_vector_index).skin_name = k_gloves[selected_entry.paint_kit_vector_index].name_short;
											}
										}
										ImGui::PopStyleColor();

										ImGui::PopID();
									}
								}
							}
						}
					}
					ImGui::ListBoxFooter();
				}
				else
				{
					ImGui::TextS("skins parsing, wait...");
				}
			}
		}
		ImGui::EndChild("skin select");
	}
}