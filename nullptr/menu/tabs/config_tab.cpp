#include "../menu.h"
static bool is_configs_loaded = false;
static bool is_configs_loaded_skin = false;
static std::string current_config;
static std::string current_config_skin;
static bool load_request_skin = false;
static bool load_request = false;
static bool save_request_skin = false;
static bool save_request = false;
static bool del_request_skin = false;
static bool del_request = false;
Color color = settings::visuals::events::config.custom_color ? settings::visuals::events::config.color : settings::misc::menu_color;

int w = 0;
int h = 0;

bool check_config_load(bool skin)
{
	g_engine_client->get_screen_size(w, h);

	ImGui::SetNextWindowPos(ImVec2((w / 2) - (164 / 2), ((h / 2) - (54 / 2))));
	ImGui::SetNextWindowSize(ImVec2(164, 54));
	ImGui::SetNextWindowFocus();

	ImGui::Begin("are u sure?", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_RainbowTitleBar | ImGuiWindowFlags_NoCollapse);
	{
		if (ImGui::Button("yes", ImVec2(70, 0)))
		{
			if (!skin)
			{
				if (!current_config.empty())
				{
					config::standart::load(current_config);
					strcpy(menu::localtag, settings::misc::clantag::clantag.c_str());
					settings::misc::clantag::clantag_visible = menu::localtag;
				}

				load_request = false;
			}
			else
			{
				if (!current_config_skin.empty())
				{
					config::skins::load(current_config_skin);
				}

				load_request_skin = false;
			}

			ImGui::End();
			return true;
		}

		ImGui::SameLine();

		if (ImGui::Button("no", ImVec2(70, 0)))
		{
			if (!skin)
				load_request = false;
			else
				load_request_skin = false;

			ImGui::End();
			return false;
		}
	
		ImGui::End();
	}
}

bool check_config_save(bool skin)
{
	g_engine_client->get_screen_size(w, h);

	ImGui::SetNextWindowPos(ImVec2((w / 2) - (164 / 2), ((h / 2) - (54 / 2))));
	ImGui::SetNextWindowSize(ImVec2(164, 54));
	ImGui::SetNextWindowFocus();

	ImGui::Begin("are u sure?", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_RainbowTitleBar | ImGuiWindowFlags_NoCollapse);
	{
		if (ImGui::Button("yes", ImVec2(70, 0)))
		{
			if (!skin)
			{
				if (!current_config.empty())
				{
					config::standart::save(current_config);
				}

				save_request = false;
			}
			else
			{
				if (!current_config_skin.empty())
				{
					config::skins::save(current_config_skin);
				}

				save_request_skin = false;
			}

			ImGui::End();
			return true;
		}

		ImGui::SameLine();

		if (ImGui::Button("no", ImVec2(70, 0)))
		{
			if (!skin)
				save_request = false;
			else
				save_request_skin = false;

			ImGui::End();
			return false;
		}

		ImGui::End();
	}
}

bool check_config_del(bool skin)
{
	g_engine_client->get_screen_size(w, h);

	ImGui::SetNextWindowPos(ImVec2((w / 2) - (164 / 2), ((h / 2) - (54 / 2))));
	ImGui::SetNextWindowSize(ImVec2(164, 54));
	ImGui::SetNextWindowFocus();

	ImGui::Begin("are u sure?", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_RainbowTitleBar | ImGuiWindowFlags_NoCollapse);
	{
		if (ImGui::Button("yes", ImVec2(70, 0)))
		{
			if (!skin)
			{
				if (!current_config.empty())
				{
					current_config.clear();
					is_configs_loaded = false;
					if (settings::visuals::events::enable)
					{
						if (settings::visuals::events::config.enable_screen)
							notify::add("config - " + current_config + " deleted", 7500);
						if (settings::visuals::events::config.enable_console)
							notify::add("config", "config - " + current_config + " deleted", color, log_type::console);
					}
				}

				del_request = false;
			}
			else
			{
				if (!current_config_skin.empty())
				{
					current_config_skin.clear();
					is_configs_loaded_skin = false;

					if (settings::visuals::events::enable)
					{
						if (settings::visuals::events::config.enable_screen)
							notify::add("config - " + current_config_skin + " deleted", 7500);
						if (settings::visuals::events::config.enable_console)
							notify::add("config", "config - " + current_config_skin + " deleted", color, log_type::console);
					}
				}

				del_request_skin = false;
			}

			ImGui::End();
			return true;
		}

		ImGui::SameLine();

		if (ImGui::Button("no", ImVec2(70, 0)))
		{
			if (!skin)
				del_request = false;
			else
				del_request_skin = false;

			ImGui::End();
			return false;
		}

		ImGui::End();
	}
}

namespace menu
{
	void tabs::config_tab()
	{
		bool can_open = !load_request && !load_request_skin && !save_request && !save_request_skin && !del_request && !del_request_skin;

		ImGui::Columns(2, nullptr, false);
		ImGui::BeginChild("configs##configs", ImVec2(0, 0), true, ImGuiWindowFlags_ChildWindowTitle);
		{
			static std::vector<std::string> configs;

			static auto load_configs = []() {
				std::vector<std::string> items = {};

				std::string path("C:\\nullptr\\standart\\");
				if (!fs::is_directory(path))
					fs::create_directories(path);

				for (auto& p : fs::directory_iterator(path))
				{
					if (strstr(p.path().string().substr(path.length()).c_str(), ".demon"))
					{
						std::string s = p.path().string().substr(path.length());

						items.push_back(s.substr(0, s.length() - 6));
					}
				}

				return items;
			};

			if (!is_configs_loaded) {
				is_configs_loaded = true;
				configs = load_configs();
			}

			static char config_name[32];

			ImGui::InputText("config name", config_name, sizeof(config_name));
			if (ImGui::ButtonS("create", ImVec2(221, 0)))
			{
				current_config = std::string(config_name);

				config::standart::save(current_config);
				is_configs_loaded = false;
				memset(config_name, 0, 32);
			}


			ImGui::ListBoxHeader("configs", ImVec2(0, 225));
			{
				for (auto& config : configs)
				{
					if (ImGui::Selectable(config.c_str(), config == current_config)) {
						current_config = config;
					}
					std::string cur_name_tooltip = "##" + config + "configs_ren";
					std::string cur_name_text = "new name##" + config + "configs_ren";
					std::string cur_name_button = "rename##" + config + "configs_ren";
					if (ImGui::ItemsToolTipBegin(cur_name_tooltip.c_str()))
					{
						static char config_rename[32];
						ImGui::InputText(cur_name_text.c_str(), config_rename, sizeof(config_rename));

						if (ImGui::ButtonS(cur_name_button.c_str(), ImVec2(187, 0)))
						{
							if (!config.empty())
							{
								std::string old_name = "C:\\nullptr\\standart\\" + config + ".demon";
								std::string new_name = "C:\\nullptr\\standart\\" + std::string(config_rename) + ".demon";

								rename(old_name.c_str(), new_name.c_str());

								if (settings::visuals::events::enable)
								{
									if (settings::visuals::events::config.enable_screen)
										notify::add("config - '' " + config + " '' renamed to - '' " + config_rename + " ''", 7500);
									if (settings::visuals::events::config.enable_console)
										notify::add("config", "config - '' " + config + " '' renamed to - '' " + config_rename + " ''", color, log_type::console);
								}

								config = std::string(config_rename);

								is_configs_loaded = false;
							}
						}
						ImGui::ItemsToolTipEnd();
					}
				}
			}
			ImGui::ListBoxFooter();

			if (ImGui::ButtonS("refresh", ImVec2(221, 0)))
				is_configs_loaded = false;

			if (ImGui::ButtonS("load", ImVec2(71, 0)))
			{
				if(can_open)
					load_request = true;
			}

			if (load_request)
				check_config_load(false);

			ImGui::SameLine();

			if (ImGui::ButtonS("save", ImVec2(71, 0)))
			{
				if(can_open)
					save_request = true;
			}

			if (save_request)
				check_config_save(false);

			ImGui::SameLine();

			if (ImGui::ButtonS("delete", ImVec2(71, 0)) && fs::remove("C:\\nullptr\\standart\\" + current_config + ".demon"))
			{
				if (can_open)
					del_request = true;
			}

			if (del_request)
				check_config_del(false);
		}
		ImGui::EndChild("configs##configs");

		ImGui::NextColumn();

		ImGui::BeginChild("config skins##configs", ImVec2(0, 0), true, ImGuiWindowFlags_ChildWindowTitle);
		{
			static std::vector<std::string> configs;

			static auto load_configs = []() {
				std::vector<std::string> items = {};

				std::string path("C:\\nullptr\\skins\\");
				if (!fs::is_directory(path))
					fs::create_directories(path);

				for (auto& p : fs::directory_iterator(path))
				{
					if (strstr(p.path().string().substr(path.length()).c_str(), ".demon"))
					{
						std::string s = p.path().string().substr(path.length());

						items.push_back(s.substr(0, s.length() - 6));
					}
				}

				return items;
			};

			if (!is_configs_loaded_skin)
			{
				is_configs_loaded_skin = true;
				configs = load_configs();
			}

			static char config_name[32];

			ImGui::InputText("config name##skins", config_name, sizeof(config_name));
			if (ImGui::ButtonS("create##skins", ImVec2(221, 0)))
			{
				current_config_skin = std::string(config_name);

				config::skins::save(current_config_skin);
				is_configs_loaded_skin = false;
				memset(config_name, 0, 32);
			}

			ImGui::ListBoxHeader("configs##skins", ImVec2(0, 225));
			{
				for (auto& config : configs)
				{
					if (ImGui::Selectable(config.c_str(), config == current_config_skin)) {
						current_config_skin = config;
					}
					std::string cur_name_tooltip = "##" + config + "configs_ren";
					std::string cur_name_text = "new name##" + config + "configs_ren";
					std::string cur_name_button = "rename##" + config + "configs_ren skins";
					if (ImGui::ItemsToolTipBegin(cur_name_tooltip.c_str()))
					{
						static char config_rename[32];
						ImGui::InputText(cur_name_text.c_str(), config_rename, sizeof(config_rename));

						if (ImGui::ButtonS(cur_name_button.c_str(), ImVec2(187, 0)))
						{
							if (!config.empty())
							{
								std::string old_name = "C:\\nullptr\\skins\\" + config + ".demon";
								std::string new_name = "C:\\nullptr\\skins\\" + std::string(config_rename) + ".demon";

								rename(old_name.c_str(), new_name.c_str());
								
								if (settings::visuals::events::enable)
								{
									if (settings::visuals::events::config.enable_screen)
										notify::add("config - '' " + config + " '' renamed to - '' " + config_rename + " ''", 7500);
									if (settings::visuals::events::config.enable_console)
										notify::add("config", "config - '' " + config + " '' renamed to - '' " + config_rename + " ''", color, log_type::console);
								}

								config = std::string(config_rename);

								is_configs_loaded_skin = false;
							}
						}
						ImGui::ItemsToolTipEnd();
					}
				}
			}
			ImGui::ListBoxFooter();

			if (ImGui::ButtonS("refresh##skins", ImVec2(221, 0)))
				is_configs_loaded_skin = false;

			if (ImGui::ButtonS("load##skins", ImVec2(71, 0)))
			{
				if (can_open)
					load_request_skin = true;
			}

			if (load_request_skin)
				check_config_load(true);

			ImGui::SameLine();

			if (ImGui::ButtonS("save##skins", ImVec2(71, 0)))
			{
				if (can_open)
					save_request_skin = true;
			}

			if (save_request_skin)
				check_config_save(true);

			ImGui::SameLine();

			if (ImGui::ButtonS("delete##skins", ImVec2(71, 0)) && fs::remove("C:\\nullptr\\skins\\" + current_config_skin + ".demon"))
			{
				del_request_skin = true;
			}

			if (del_request_skin)
				check_config_del(true);
		}
		ImGui::EndChild("config skins##configs");
	}
}