#include "../menu.h"

static char* clantag_tabs[] =
{
	"standart",
	"players tags",
	"custom"
};

namespace menu
{
	void tabs::misc_tab()
	{
		auto& style = ImGui::GetStyle();

		ImGui::Columns(2, nullptr, false);
		ImGui::BeginChild("misc##misctab", ImVec2(0, 301), true, ImGuiWindowFlags_ChildWindowTitle);
		{
			ImGui::ColorEdit4("menu color", &settings::misc::menu_color);
			ImGui::Checkbox("bhop##bhop", &settings::misc::bhop::enable);
			ImGui::Checkbox("auto strafe##bhop", &settings::misc::bhop::auto_strafer);
			if (settings::misc::bhop::auto_strafer)
			{
				ImGui::Combo("auto strafe type", &settings::misc::bhop::strafer_type, std::vector<std::string>{ "standart", "WASD" });
				if (settings::misc::bhop::strafer_type == 0)
					ImGui::SliderFloat("retrack speed##bhop", &settings::misc::bhop::retrack_speed, 1.0f, 10.0f, "%.1f");
			}

			ImGui::Checkbox("in game radar", &settings::visuals::radar_ingame);

			ImGui::Checkbox("edge jump [?]", &settings::misc::edge_jump::enable);
			if (ImGui::ItemsToolTipBegin("##edge_jump_help"))
			{
				ImGui::Checkbox("auto duck", &settings::misc::edge_jump::auto_duck);
				ImGui::ItemsToolTipEnd();
			}
			ImGui::SameLine();
			ImGui::KeyBind("##edge_bind", &settings::misc::edge_jump::bind, ImGuiKeyBindFlags_OnItem);
			ImGui::Checkbox("block bot", &settings::misc::block_bot::enable);
			ImGui::SameLine();
			ImGui::KeyBind("##bock_bot_bind", &settings::misc::block_bot::bind, ImGuiKeyBindFlags_OnItem);
			ImGui::Checkbox("auto accept", &settings::misc::auto_accept);
			ImGui::Checkbox("money reveal", &settings::misc::reveal_money);
			ImGui::Checkbox("rank reveal", &settings::misc::reveal_rank);
			ImGui::Checkbox("moon walk", &settings::misc::moon_walk);
			ImGui::Checkbox("fast duck", &settings::misc::fast_duck);
			ImGui::Checkbox("prepare revolver", &settings::misc::prepare_revolver::enable);
			ImGui::SameLine();
			ImGui::KeyBind("##prepare_revolver_bind", &settings::misc::prepare_revolver::bind, ImGuiKeyBindFlags_OnItem);
			ImGui::Checkbox("slow gravity", &settings::misc::inverse_gravity::enable_slow);
			ImGui::Checkbox("inverse gravity", &settings::misc::inverse_gravity::enable);
			ImGui::SliderInt("inverse gravity value", &settings::misc::inverse_gravity::value, -10, 10);

			const char* disables_c[5] = { "smoke", "flash", "zoom", "zoom border" };
			bool* disables_b[5] = { &settings::misc::disable_smoke, &settings::misc::disable_flash, &settings::misc::disable_zoom, &settings::misc::disable_zoom_border };
			ImGui::MultiCombo("disable##misc", disables_c, disables_b, 4);
			if (settings::misc::disable_flash)
			{
				ImGui::SliderInt("max alpha##disable_flash", &settings::misc::flash_alpha, 0, 255);
			}

			ImGui::Checkbox("watermark", &settings::misc::watermark);
			ImGui::Checkbox("third Person", &settings::misc::third_person::enable);
			ImGui::SameLine();
			ImGui::KeyBind("##third_Person_bind", &settings::misc::third_person::bind, ImGuiKeyBindFlags_OnItem);
			if (settings::misc::third_person::enable)
			{
				ImGui::SliderFloat("dist##third_Person_", &settings::misc::third_person::dist, 25.f, 350.f, "%.1f");
			}

			ImGui::Checkbox("slowwalk##enable", &settings::misc::slowwalk::enable);
			ImGui::SameLine();
			ImGui::KeyBind("slowwalk##bind", &settings::misc::slowwalk::bind, ImGuiKeyBindFlags_OnItem);
			if (settings::misc::slowwalk::enable)
			{
				ImGui::SliderFloat("speed##slowwalk", &settings::misc::slowwalk::speed, 0.f, 100.f, "%.0f%%");
			}
		}
		ImGui::EndChild("misc##misctab");
		ImGui::BeginChild("clantag##misctab", ImVec2(0, 201), true, ImGuiWindowFlags_ChildWindowTitle);
		{

			horizontal_tabs(settings::misc::clantag::clantag_type, clantag_tabs);

			ImGui::Checkbox("enable##clantag", &settings::misc::clantag::enable);

			if (settings::misc::clantag::clantag_type == 0)
			{
				ImGui::Combo("clantag's##clanTag", &settings::misc::clantag::animation_type, std::vector<std::string>{"nullptr", "gaysence", "polakware.net", "c l o w n"});
			}
			if (settings::misc::clantag::clantag_type == 1)
			{
				if (g_player_resource)
				{

					ImVec2 becup = style.WindowPadding;
					float bec = style.FramePadding.x;

					if (ImGui::BeginCombo("player tags [?]##clantags.list", "tags"))
					{
						std::vector<std::string> tags;

						if (ImGui::Selectable("none"))
						{
							misc::clan_tag::user_index = 0;
						}

						for (int i = 1; i < g_engine_client->get_max_clients(); ++i)
						{
							auto* player = c_base_player::get_player_by_index(i);

							if (!player)
								continue;

							const auto info = player->get_player_info();
							if (info.fakeplayer)
								continue;

							auto usertag = std::string(g_player_resource->clantag()[player->get_index()]);
							auto user_name = std::string(info.szName);
							if (settings::misc::clantag::check_empty)
							{
								if (usertag.empty() || std::find(tags.begin(), tags.end(), usertag) != tags.end())
									continue;
							}

							tags.push_back(usertag);

							user_name += " : ";
							user_name += usertag;

							if (player->get_index() != g_local_player->get_index())
							{
								if (ImGui::Selectable(user_name.c_str()))
								{
									misc::clan_tag::user_index = i;
								}
							}
						}

						ImGui::EndCombo();
					}
					style.WindowPadding.x = becup.x;
					style.WindowPadding.y = becup.y;
					style.FramePadding.x = bec;
					if (ImGui::ItemsToolTipBegin("##player_tag_help"))
					{
						ImGui::Checkbox("checking on empty tag at player", &settings::misc::clantag::check_empty);
						ImGui::ItemsToolTipEnd();
					}
				}
				else
				{
					ImGui::TextS("connect to server");
				}
			}
			if (settings::misc::clantag::clantag_type == 2)
			{

				ImGui::InputText("clantag##misc_clantag", localtag, sizeof(localtag));
				if (ImGui::ButtonS("apply##clan", ImVec2(216, 0)))
				{
					settings::misc::clantag::clantag = localtag;
					settings::misc::clantag::clantag_visible = localtag;
				}

				ImGui::Combo("animation", &settings::misc::clantag::custom_type, "none\0type 1\0type 2");
				if (settings::misc::clantag::clantag_type != 0)
				{
					ImGui::SliderFloat("speed", &settings::misc::clantag::speed, 0.1f, 2.f, "%.1f");
				}
			}
		}
		ImGui::EndChild("clantag##misctab");

		ImGui::BeginChild("fake latency##misctab", ImVec2(0, 81), true, ImGuiWindowFlags_ChildWindowTitle);
		{
			ImGui::Checkbox("enable##fake latency", &settings::misc::fake_latency::enable);
			ImGui::SliderInt("amount##fake_latency", &settings::misc::fake_latency::amount, 0, 1000, "%i ms");
			ImGui::Tooltip("there may be problems with backtrack and connecting to the server.");
		}
		ImGui::EndChild("fake latency##misctab");

		ImGui::NextColumn();

		ImGui::BeginChild("viewmodel##misctab", ImVec2(0, 173), true, ImGuiWindowFlags_ChildWindowTitle);
		{
			ImGui::SliderFloat("override fov", &settings::misc::viewmodel::override, 60, 150, "%.1f");
			ImGui::SliderFloat("viewmodel fov", &settings::misc::viewmodel::viewmodel, 42, 120, "%.1f");
			ImGui::SliderFloat("viewmodel x", &settings::misc::viewmodel::fov_x, -30, 30, "%.1f");
			ImGui::SliderFloat("viewmodel y", &settings::misc::viewmodel::fov_z, -30, 30, "%.1f");
			ImGui::SliderFloat("viewmodel z", &settings::misc::viewmodel::fov_y, -30, 30, "%.1f");
		}
		ImGui::EndChild("viewmodel##misctab");

		ImGui::BeginChild("fake lag##misctab", ImVec2(0, 151), true, ImGuiWindowFlags_ChildWindowTitle);
		{
			const char* fake_lags_c[2] = { "when standing", "when moving" };
			bool* fake_lags_b[2] = { &settings::misc::fake_lag::lag_when_stand, &settings::misc::fake_lag::lag_when_move };

			ImGui::Checkbox("enable##fakelag", &settings::misc::fake_lag::enable);

			ImGui::MultiCombo("type's#fakelag", fake_lags_c, fake_lags_b, 2);
			ImGui::Combo("mode", &settings::misc::fake_lag::type, std::vector<std::string>{"switch", "static", "adaptive", "random", "legit peek"});
			ImGui::SliderInt("factor", &settings::misc::fake_lag::factor, 1, 15);

		}
		ImGui::EndChild("fake lag##misctab");
		ImGui::BeginChild("desync##misctab", ImVec2(0, 108), true, ImGuiWindowFlags_ChildWindowTitle);
		{
			ImGui::Combo("desync [BETA]", &settings::misc::desync::type, std::vector<std::string>{ "off", "static", "balance" });
			ImGui::KeyBind("flip bind", &settings::misc::desync::bind, ImGuiKeyBindFlags_NoSettings);

			ImGui::Checkbox("ad##autodiraa", &settings::misc::desync::auto_der);
		}
		ImGui::EndChild("desync##misctab");
		ImGui::BeginChild("windows##misctab", ImVec2(0, 93), true, ImGuiWindowFlags_ChildWindowTitle);
		{
			ImGui::Checkbox("bind window [?]", &settings::misc::bind_window::enable);
			if (ImGui::ItemsToolTipBegin("##bind_window"))
			{
				ImGui::SliderFloat("alpha##bind_window", &settings::windows::Bind_alpha, 0.f, 1.f, "%.1f");
				ImGui::ItemsToolTipEnd();
			}
			ImGui::Checkbox("spectator list [?]", &settings::visuals::spectator_list);
			if (ImGui::ItemsToolTipBegin("##spec_window"))
			{
				ImGui::SliderFloat("alpha##spec_window", &settings::windows::Spec_alpha, 0.f, 1.f, "%.1f");
				ImGui::ItemsToolTipEnd();
			}

			ImGui::Checkbox("info window [?]", &settings::visuals::spectator_list);
			if (ImGui::ItemsToolTipBegin("##spec_window"))
			{
				ImGui::SliderFloat("alpha##spec_window", &settings::windows::Spec_alpha, 0.f, 1.f, "%.1f");
				ImGui::ItemsToolTipEnd();
			}
		}
		ImGui::EndChild("windows##misctab");
	}
}