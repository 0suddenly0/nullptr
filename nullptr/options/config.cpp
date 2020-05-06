#include "config.h"
#include "options.h"
#include "../helpers/math.h"
#include "../helpers/utils.h"
#include "../menu/menu.h"
#include "../helpers/input.h"
#include "../imgui/imgui_internal.h"
#include "../features/notify/notify.h"]
#include "../features/changers/skin/item_definitions.h"

char line_splited = '/';

namespace config
{
	namespace c_utils
	{
		std::string split_line(std::string temp)
		{
			std::string new_t = "";

			for (int g = 0; g < temp.length(); g++)
			{
				if (temp[g] != line_splited)
				{
					new_t += temp[g];
				}
				else
				{
					return new_t;
				}
			}
		}

		void add_var(int& cur_line, int& cur_group, std::vector<std::string>& config, std::vector<window_v>& vars, std::string name, std::string wind, float& x, float& y)
		{
			add_space(cur_group, config);

			std::string full = name + " = x-" + std::to_string((int)x) + " y-" + std::to_string((int)y) + ";";

			vars.push_back(window_v{ full, name + " = ", wind, x, y, cur_line, cur_group });
			config.push_back(name + " = x-" + std::to_string((int)x) + " y-" + std::to_string((int)y) + ";" + "\n");

			cur_line++;
		}
		void add_var(int& cur_line, int& cur_group, std::vector<std::string>& config, std::vector<string_v>& vars, std::string name, std::string& var)
		{
			add_space(cur_group, config);

			std::string full = name + " = '" + var + "';";

			vars.push_back(string_v{ full, name + " = ", var, cur_line, cur_group });
			config.push_back(name + " = '" + var + "';" + "\n");

			cur_line++;
		}
		void add_var(int& cur_line, int& cur_group, std::vector<std::string>& config, std::vector<int_v>& vars, std::string name, int& var)
		{
			add_space(cur_group, config);

			std::string full = name + " = " + std::to_string(var) + ";";

			vars.push_back(int_v{ full, name + " = ", var, cur_line, cur_group });
			config.push_back(name + " = " + std::to_string(var) + ";" + "\n");

			cur_line++;
		}
		void add_var(int& cur_line, int& cur_group, std::vector<std::string>& config, std::vector<float_v>& vars, std::string name, float& var)
		{
			add_space(cur_group, config);

			std::string full = name + " = " + std::to_string(var) + ";";

			vars.push_back(float_v{ full, name + " = ", var, cur_line, cur_group });
			config.push_back(name + " = " + std::to_string(var) + ";" + "\n");

			cur_line++;
		}
		void add_var(int& cur_line, int& cur_group, std::vector<std::string>& config, std::vector<bind_v>& vars, std::string name, KeyBind_t& var)
		{
			add_space(cur_group, config);

			std::string full = name + " = " + std::to_string(var.key_id) + "-" + std::to_string(var.bind_type) + "-" + (var.enable ? "true" : "false") + ";";

			vars.push_back(bind_v{ full, name + " = ", var, cur_line, cur_group });
			config.push_back(name + " = " + std::to_string(var.key_id) + "-" + std::to_string(var.bind_type) + "-" + (var.enable ? "true" : "false") + ";" + "\n");

			cur_line++;
		}
		void add_var(int& cur_line, int& cur_group, std::vector<std::string>& config, std::vector<bool_v>& vars, std::string name, bool& var)
		{
			add_space(cur_group, config);

			std::string full = name + " = " + (var ? "true" : "false") + ";";

			vars.push_back(bool_v{ full, name + " = ", var, cur_line, cur_group });
			config.push_back(name + " = " + (var ? "true" : "false") + ";" + "\n");

			cur_line++;
		}
		void add_var(int& cur_line, int& cur_group, std::vector<std::string>& config, std::vector<color_v>& vars, std::string name, Color& var)
		{
			add_space(cur_group, config);

			std::string full = name + " = " + std::to_string(var.r()) + "-" + std::to_string(var.g()) + "-" + std::to_string(var.b()) + "-" + std::to_string(var.a()) + ";";

			vars.push_back(color_v{ full, name + " = ", var, cur_line, cur_group });
			config.push_back(name + " = " + std::to_string(var.r()) + "-" + std::to_string(var.g()) + "-" + std::to_string(var.b()) + "-" + std::to_string(var.a()) + ";" + "\n");

			cur_line++;
		}

		void add_space(int& cur_group, std::vector<std::string>& config)
		{
			for (int i = 0; i < cur_group; i++)
			{
				std::string text = "| ";

				if (i == cur_group - 1)
					text = "|- ";
				else
					text = "|  ";

				config.push_back(text);
			}
		}

		void add_group(int& cur_line, int& cur_group, std::vector<std::string>& config, std::string name)
		{
			add_space(cur_group, config);

			config.push_back("[ " + name + " ]" + "\n");

			cur_group++;
			cur_line++;
		}

		void end_group(int& cur_group)
		{
			cur_group--;
		}

		void save(std::string path, std::vector<std::string>& config, std::string name)
		{
			if (name.empty())
				return;
			CreateDirectoryA((LPCSTR)std::string("C:\\nullptr\\" + path + "\\").c_str(), NULL);

			std::ofstream out;
			out.open("C:\\nullptr\\" + path + "\\" + name + ".demon");
			if (out.is_open())
			{
				for (int i = 0; i < config.size(); i++)
				{
					out << config[i];
				}
			}
			out.close();
		}

		void load(std::string path, std::string name, std::vector<int_v>& ints, std::vector<bool_v>& bools, std::vector<float_v>& floats, std::vector<color_v>& colors, std::vector<bind_v>& binds, std::vector<string_v>& strings, std::vector<window_v>& windows)
		{
			if (name.empty())
				return;

			CreateDirectoryA((LPCSTR)std::string("C:\\nullptr\\" + path + "\\").c_str(), NULL);

			std::string line;

			std::string config = "";

			std::ifstream in("C:\\nullptr\\" + path + "\\" + name + ".demon");

			if (in.is_open())
			{
				while (getline(in, line))
				{
					config += line;
					config += line_splited;
				}
			}
			in.close();


			for (int i = 0; i < bools.size(); i++)
			{
				if (strstr(config.c_str(), bools[i].text.c_str()))
				{
					std::string temp = config;
					temp.erase(0, temp.find(bools[i].text));

					std::string new_t = c_utils::split_line(temp);
					new_t.erase(0, bools[i].text.length());
					new_t.erase(new_t.length() - 1, 1);

					bools[i].v = (new_t == "true" ? true : false);
				}
			}

			for (int i = 0; i < ints.size(); i++)
			{
				if (strstr(config.c_str(), ints[i].text.c_str()))
				{
					std::string temp = config;
					temp.erase(0, temp.find(ints[i].text));

					std::string new_t = c_utils::split_line(temp);
					new_t.erase(0, ints[i].text.length());
					new_t.erase(new_t.length() - 1, 1);

					ints[i].v = std::stoi(new_t);

				}
			}

			for (int i = 0; i < floats.size(); i++)
			{
				if (strstr(config.c_str(), floats[i].text.c_str()))
				{
					std::string temp = config;
					temp.erase(0, temp.find(floats[i].text));

					std::string new_t = c_utils::split_line(temp);
					new_t.erase(0, floats[i].text.length());
					new_t.erase(new_t.length() - 1, 1);

					floats[i].v = std::stof(new_t);
				}
			}

			for (int i = 0; i < strings.size(); i++)
			{
				if (strstr(config.c_str(), strings[i].text.c_str()))
				{
					std::string temp = config;
					temp.erase(0, temp.find(strings[i].text));

					std::string new_t = c_utils::split_line(temp);
					new_t.erase(0, strings[i].text.length() + 1);
					new_t.erase(new_t.length() - 2, 2);

					strings[i].v = new_t;
				}
			}

			for (int i = 0; i < colors.size(); i++)
			{
				if (strstr(config.c_str(), colors[i].text.c_str()))
				{
					std::string temp = config;
					temp.erase(0, temp.find(colors[i].text));

					std::string new_t = c_utils::split_line(temp);
					new_t.erase(0, colors[i].text.length());

					std::string color = "";

					char split = '-';
					char endl = ';';

					float col[4] = { 0, 0, 0, 0 };

					int h = 0;

					for (int g = 0; g < new_t.length(); g++)
					{

						if (new_t[g] != split && new_t[g] != endl)
						{
							color += new_t[g];
						}
						else
						{
							col[h] = std::stof(color);
							color = "";
							h++;
						}
					}



					colors[i].v._CColor[0] = col[0];
					colors[i].v._CColor[1] = col[1];
					colors[i].v._CColor[2] = col[2];
					colors[i].v._CColor[3] = col[3];

					colors[i].v.save_color[0] = col[0] / 255.f;
					colors[i].v.save_color[1] = col[1] / 255.f;
					colors[i].v.save_color[2] = col[2] / 255.f;
					colors[i].v.save_color[3] = col[3] / 255.f;
				}
			}

			for (int i = 0; i < binds.size(); i++)
			{
				if (strstr(config.c_str(), binds[i].text.c_str()))
				{
					std::string temp = config;
					temp.erase(0, temp.find(binds[i].text));

					std::string new_t = c_utils::split_line(temp);
					new_t.erase(0, binds[i].text.length());

					std::string value = "";

					int h = 0;

					char split = '-';
					char endl = ';';

					for (int g = 0; g < new_t.length(); g++)
					{
						if (new_t[g] != split && new_t[g] != endl)
						{
							value += new_t[g];
						}
						else
						{
							switch (h)
							{
							case 0: binds[i].v.key_id = std::stoi(value);
							case 1: binds[i].v.bind_type = std::stoi(value);
							case 2: binds[i].v.enable = (value == "true" ? true : false);
							}

							value = "";
							h++;
						}
					}
				}
			}

			for (int i = 0; i < windows.size(); i++)
			{

				if (strstr(config.c_str(), windows[i].text.c_str()))
				{
					std::string temp = config;
					temp.erase(0, temp.find(windows[i].text));

					std::string new_t = c_utils::split_line(temp);
					new_t.erase(0, windows[i].text.length() + 2);

					ImVec2 local_pos = ImVec2(0, 0);

					char split = ' ';
					char endl = ';';

					std::string value = "";

					for (int g = 0; g < new_t.length(); g++)
					{
						if (new_t[g] != split && new_t[g] != endl)
						{
							value += new_t[g];
						}
						else if (new_t[g] == split)
						{
							local_pos.x = std::stof(value);
							g += 2;

							value = "";
						}
						else if (new_t[g] == endl)
						{
							local_pos.y = std::stof(value);

							value = "";
						}
					}

					ImGui::SetWindowPos(windows[i].window_name.c_str(), local_pos);
				}

			}
		}
	}

	namespace standart
	{
		void clear()
		{
			config.clear();

			ints.clear();
			bools.clear();
			binds.clear();
			floats.clear();
			colors.clear();
			strings.clear();
			windows.clear();

			cur_line = 0;
			cur_group = 0;
		}

		void setup_set()
		{
			clear();

			add_group("legit bot");
			{
				add_group("general");
				{
					add_var("[lb general] bind check", settings::legit_bot::bind_check);
					add_var("[lb general] bind", settings::legit_bot::bind);
					add_var("[lb general] auto-current", settings::legit_bot::auto_current);
					add_var("[lb general] settings weapon", settings::legit_bot::settings_weapon);
					add_var("[lb general] settings category", settings::legit_bot::settings_category);
					add_var("[lb general] settings group", settings::legit_bot::weapon_group);
				}
				end_group();

				add_group("category");
				{
					add_group("weapons");
					{
						for (auto& [id, val] : weapon_names)
						{
							auto& settings_cur = settings::legit_bot::legitbot_items[id];

							add_group(val);
							{
								add_var("[lb weap]-[" + val + "] enable", settings_cur.enabled);

								add_group("fov's");
								{
									add_var("[lb weap]-[fov]-[" + val + "] fov", settings_cur.fov);
									add_var("[lb weap]-[fov]-[" + val + "] silent fov", settings_cur.silent_fov);
								}
								end_group();

								add_var("[lb weap]-[" + val + "] deathmatch", settings_cur.deathmatch);
								add_var("[lb weap]-[" + val + "] autopistol", settings_cur.autopistol);

								add_group("check's");
								{
									add_var("[lb weap]-[check]-[" + val + "] smoke check", settings_cur.smoke_check);
									add_var("[lb weap]-[check]-[" + val + "] flash check", settings_cur.flash_check);
									add_var("[lb weap]-[check]-[" + val + "] jump check", settings_cur.jump_check);
									add_var("[lb weap]-[check]-[" + val + "] only in zoom", settings_cur.only_in_zoom);
								}
								end_group();

								add_var("[lb weap]-[" + val + "] autowall", settings_cur.autowall);
								add_var("[lb weap]-[" + val + "] min damage", settings_cur.min_damage);

								add_group("backtrack");
								{
									add_var("[lb weap]-[bt]-[" + val + "] aim at backtrack", settings_cur.aim_at_backtrack);
									add_var("[lb weap]-[bt]-[" + val + "] time", settings_cur.backtrack_time);
								}
								end_group();

								add_var("[lb weap]-[" + val + "] smooth type", settings_cur.smooth_type);

								add_group("hitboxes");
								{
									for (auto& [idx, hit_name] : hitbox_names)
									{
										auto& hit_cur = settings_cur.hitboxes[idx];

										add_var("[lb weap]-[hits]-[" + val + "] " + hit_name, hit_cur);
									}
								}
								end_group();

								add_group("hitbox override");
								{
									add_var("[lb weap]-[hit over]-[" + val + "] enable", settings_cur.enable_hitbox_override);
									add_var("[lb weap]-[hit over]-[" + val + "] bind", settings_cur.hitbox_override_bind);
									add_group("hitboxes");
									{
										for (auto& [idx, hit_name] : hitbox_names)
										{
											auto& hit_cur = settings_cur.override_hitboxes[idx];

											add_var("[lb weap]-[hit over]-[hits]-[" + val + "] " + hit_name, hit_cur);
										}
									}
									end_group();
								}
								end_group();

								add_var("[lb weap]-[" + val + "] smooth standart", settings_cur.smooth);
								add_var("[lb weap]-[" + val + "] switch target delay", settings_cur.kill_delay);

								add_group("rcs");
								{
									add_var("[lb weap]-[rcs]-[" + val + "] enable", settings_cur.rcs);
									add_var("[lb weap]-[rcs]-[" + val + "] type", settings_cur.rcs_type);
									add_var("[lb weap]-[rcs]-[" + val + "] enable custom fov", settings_cur.rcs_fov_enabled);
									add_var("[lb weap]-[rcs]-[" + val + "] enable custom smooth", settings_cur.rcs_smooth_enabled);
									add_var("[lb weap]-[rcs]-[" + val + "] custom fov", settings_cur.rcs_fov);
									add_var("[lb weap]-[rcs]-[" + val + "] custom smooth", settings_cur.rcs_smooth);
									add_var("[lb weap]-[rcs]-[" + val + "] rcs x", settings_cur.rcs_x);
									add_var("[lb weap]-[rcs]-[" + val + "] rcs y", settings_cur.rcs_y);
									add_var("[lb weap]-[rcs]-[" + val + "] bullet start", settings_cur.rcs_start);
								}
								end_group();
							}
							end_group();
						}
					}
					end_group();

					add_group("all weapon");
					{
						auto& settings_cur = settings::legit_bot::legitbot_items_all;
						
						add_var("[lb all weap] enable", settings_cur.enabled);

						add_group("fov's");
						{
							add_var("[lb all weap]-[fov] fov", settings_cur.fov);
							add_var("[lb all weap]-[fov] silent fov", settings_cur.silent_fov);
						}
						end_group();

						add_var("[lb all weap] deathmatch", settings_cur.deathmatch);
						add_var("[lb all weap] autopistol", settings_cur.autopistol);

						add_group("check's");
						{
							add_var("[lb all weap]-[check] smoke check", settings_cur.smoke_check);
							add_var("[lb all weap]-[check] flash check", settings_cur.flash_check);
							add_var("[lb all weap]-[check] jump check", settings_cur.jump_check);
							add_var("[lb all weap]-[check] only in zoom", settings_cur.only_in_zoom);
						}
						end_group();

						add_var("[lb all weap] autowall", settings_cur.autowall);
						add_var("[lb all weap] min damage", settings_cur.min_damage);

						add_group("backtrack");
						{
							add_var("[lb all weap]-[bt] aim at backtrack", settings_cur.aim_at_backtrack);
							add_var("[lb all weap]-[bt] time", settings_cur.backtrack_time);
						}
						end_group();

						add_var("[lb all weap] smooth type", settings_cur.smooth_type);

						add_group("hitboxes");
						{
							for (auto& [idx, hit_name] : hitbox_names)
							{
								auto& hit_cur = settings_cur.hitboxes[idx];

								add_var("[lb all weap]-[hits] " + hit_name, hit_cur);
							}
						}
						end_group();

						add_group("hitbox override");
						{
							add_var("[lb gp]-[hit over] enable", settings_cur.enable_hitbox_override);
							add_var("[lb gp]-[hit over] bind", settings_cur.hitbox_override_bind);
							add_group("hitboxes");
							{
								for (auto& [idx, hit_name] : hitbox_names)
								{
									auto& hit_cur = settings_cur.override_hitboxes[idx];

									add_var("[lb all weap]-[hit over]-[hits] " + hit_name, hit_cur);
								}
							}
							end_group();
						}
						end_group();

						add_var("[lb all weap] smooth standart", settings_cur.smooth);
						add_var("[lb all weap] switch target delay", settings_cur.kill_delay);

						add_group("rcs");
						{
							add_var("[lb all weap]-[rcs] enable", settings_cur.rcs);
							add_var("[lb all weap]-[rcs] type", settings_cur.rcs_type);
							add_var("[lb all weap]-[rcs] enable custom fov", settings_cur.rcs_fov_enabled);
							add_var("[lb all weap]-[rcs] enable custom smooth", settings_cur.rcs_smooth_enabled);
							add_var("[lb all weap]-[rcs] custom fov", settings_cur.rcs_fov);
							add_var("[lb all weap]-[rcs] custom smooth", settings_cur.rcs_smooth);
							add_var("[lb all weap]-[rcs] rcs x", settings_cur.rcs_x);
							add_var("[lb all weap]-[rcs] rcs y", settings_cur.rcs_y);
							add_var("[lb all weap]-[rcs] bullet start", settings_cur.rcs_start);
						}
						end_group();
					}
					end_group();

					add_group("groups");
					{
						for (auto& [id, val] : weapon_groups)
						{
							auto& settings_cur = settings::legit_bot::legitbot_items_groups[id];

							add_group(val);
							{
								add_var("[lb gp]-[" + val + "] enable", settings_cur.enabled);

								add_group("fov's");
								{
									add_var("[lb gp]-[fov]-[" + val + "] fov", settings_cur.fov);
									add_var("[lb gp]-[fov]-[" + val + "] silent fov", settings_cur.silent_fov);
								}
								end_group();

								add_var("[lb gp]-[" + val + "] deathmatch", settings_cur.deathmatch);
								add_var("[lb gp]-[" + val + "] autopistol", settings_cur.autopistol);

								add_group("check's");
								{
									add_var("[lb gp]-[check]-[" + val + "] smoke check", settings_cur.smoke_check);
									add_var("[lb gp]-[check]-[" + val + "] flash check", settings_cur.flash_check);
									add_var("[lb gp]-[check]-[" + val + "] jump check", settings_cur.jump_check);
									add_var("[lb gp]-[check]-[" + val + "] only in zoom", settings_cur.only_in_zoom);
								}
								end_group();

								add_var("[lb gp]-[" + val + "] autowall", settings_cur.autowall);
								add_var("[lb gp]-[" + val + "] min damage", settings_cur.min_damage);

								add_group("backtrack");
								{
									add_var("[lb gp]-[bt]-[" + val + "] aim at backtrack", settings_cur.aim_at_backtrack);
									add_var("[lb gp]-[bt]-[" + val + "] time", settings_cur.backtrack_time);
								}
								end_group();

								add_var("[lb gp]-[" + val + "] smooth type", settings_cur.smooth_type);

								add_group("hitboxes");
								{
									for (auto& [idx, hit_name] : hitbox_names)
									{
										auto& hit_cur = settings_cur.hitboxes[idx];

										add_var("[lb gp]-[hits]-[" + val + "] " + hit_name, hit_cur);
									}
								}
								end_group();

								add_group("hitbox override");
								{
									add_var("[lb gp]-[hit over]-[" + val + "] enable", settings_cur.enable_hitbox_override);
									add_var("[lb gp]-[hit over]-[" + val + "] bind", settings_cur.hitbox_override_bind);
									add_group("hitboxes");
									{
										for (auto& [idx, hit_name] : hitbox_names)
										{
											auto& hit_cur = settings_cur.override_hitboxes[idx];

											add_var("[lb gp]-[hit over]-[hits]-[" + val + "] " + hit_name, hit_cur);
										}
									}
									end_group();
								}
								end_group();

								add_var("[lb gp]-[" + val + "] smooth standart", settings_cur.smooth);
								add_var("[lb gp]-[" + val + "] switch target delay", settings_cur.kill_delay);

								add_group("rcs");
								{
									add_var("[lb gp]-[rcs]-[" + val + "] enable", settings_cur.rcs);
									add_var("[lb gp]-[rcs]-[" + val + "] type", settings_cur.rcs_type);
									add_var("[lb gp]-[rcs]-[" + val + "] enable custom fov", settings_cur.rcs_fov_enabled);
									add_var("[lb gp]-[rcs]-[" + val + "] enable custom smooth", settings_cur.rcs_smooth_enabled);
									add_var("[lb gp]-[rcs]-[" + val + "] custom fov", settings_cur.rcs_fov);
									add_var("[lb gp]-[rcs]-[" + val + "] custom smooth", settings_cur.rcs_smooth);
									add_var("[lb gp]-[rcs]-[" + val + "] rcs x", settings_cur.rcs_x);
									add_var("[lb gp]-[rcs]-[" + val + "] rcs y", settings_cur.rcs_y);
									add_var("[lb gp]-[rcs]-[" + val + "] bullet start", settings_cur.rcs_start);
								}
								end_group();
							}
							end_group();
						}
					}
					end_group();
				}
			}
			end_group();

			add_group("visuals");
			{
				add_group("general");
				{
					add_group("esp");
					{
						for (auto& [id, val] : entity_names)
						{
							auto& settings_cur = settings::visuals::esp::esp_items[id];

							add_group(val);
							{
								add_var("[esp]-[" + val + "] enable", settings_cur.enable);
								add_var("[esp]-[" + val + "] visible only", settings_cur.only_visible);
								add_var("[esp]-[" + val + "] box", settings_cur.box);
								add_var("[esp]-[" + val + "] color visible", settings_cur.box_visible);
								add_var("[esp]-[" + val + "] color invisible", settings_cur.box_invisible);
								add_var("[esp]-[" + val + "] dormant color", settings_cur.dormant);
								add_var("[esp]-[" + val + "] name", settings_cur.name);

								add_group("skeleton");
								{
									add_var("[esp]-[skeleton]-[" + val + "] enable", settings_cur.skeleton);
									add_var("[esp]-[skeleton]-[" + val + "] color visible", settings_cur.skeleton_visible);
									add_var("[esp]-[skeleton]-[" + val + "] color invisible", settings_cur.skeleton_invisible);
								}
								end_group();

								add_group("health");
								{
									add_var("[esp]-[health]-[" + val + "] enable", settings_cur.health_bar);
									add_var("[esp]-[health]-[" + val + "] health in bar", settings_cur.health_in_bar);
									add_var("[esp]-[health]-[" + val + "] color based of health", settings_cur.health_based);
									add_var("[esp]-[health]-[" + val + "] bar pos", settings_cur.health_position);
									add_var("[esp]-[health]-[" + val + "] bar color", settings_cur.health_bar_main);
									add_var("[esp]-[health]-[" + val + "] bar outline color", settings_cur.health_bar_outline);
									add_var("[esp]-[health]-[" + val + "] bar backgroud color", settings_cur.health_bar_background);
								}
								end_group();

								add_group("armor");
								{
									add_var("[esp]-[armor]-[" + val + "] enable", settings_cur.armor_bar);
									add_var("[esp]-[armor]-[" + val + "] armor in bar", settings_cur.armor_in_bar);
									add_var("[esp]-[armor]-[" + val + "] bar pos", settings_cur.armor_position);
									add_var("[esp]-[armor]-[" + val + "] bar color", settings_cur.armor_bar_main);
									add_var("[esp]-[armor]-[" + val + "] bar outline color", settings_cur.armor_bar_outline);
									add_var("[esp]-[armor]-[" + val + "] bar backgroud color", settings_cur.armor_bar_background);
								}
								end_group();

								add_var("[esp]-[weapon]-[" + val + "] weapon", settings_cur.weapon);

								add_group("ammo");
								{
									add_var("[esp]-[weapon]-[" + val + "] enable", settings_cur.weapon_ammo);
									add_var("[esp]-[weapon]-[" + val + "] bar color", settings_cur.ammo_bar_main);
									add_var("[esp]-[weapon]-[" + val + "] bar outline color", settings_cur.ammo_bar_outline);
									add_var("[esp]-[weapon]-[" + val + "] bar backgroud color", settings_cur.ammo_bar_background);
								}
								end_group();

								add_group("flags");
								{
									add_var("[esp]-[flags]-[" + val + "] flashed", settings_cur.flags_flashed);
									add_var("[esp]-[flags]-[" + val + "] armor", settings_cur.flags_armor);
									add_var("[esp]-[flags]-[" + val + "] have bomb", settings_cur.flags_bomb_carrier);
									add_var("[esp]-[flags]-[" + val + "] defusing", settings_cur.flags_defusing);
									add_var("[esp]-[flags]-[" + val + "] helment", settings_cur.flags_helmet);
									add_var("[esp]-[flags]-[" + val + "] planting", settings_cur.flags_planting);
									add_var("[esp]-[flags]-[" + val + "] scoped", settings_cur.flags_scoped);
								}
								end_group();
							}
							end_group();
						}
					}
					end_group();

					add_group("glow");
					{
						for (auto& [id, val] : entity_names)
						{
							auto& settings_cur = settings::visuals::glow::glow_items[id];

							add_group(val);
							{
								add_var("[glow]-[" + val + "] enable", settings_cur.enable);
								add_var("[glow]-[" + val + "] visible only", settings_cur.visible_only);
								add_var("[glow]-[" + val + "] visible", settings_cur.visible);
								add_var("[glow]-[" + val + "] invisible", settings_cur.in_visible);
							}
							end_group();
						}
					}
					end_group();

					add_group("chams");
					{
						for (auto& [id, val] : entity_names)
						{
							auto& settings_cur = settings::visuals::chams::chams_items[id];

							add_group(val);
							{
								add_var("[chams]-[" + val + "] enable", settings_cur.enable);
								add_var("[chams]-[" + val + "] type", settings_cur.chams_type);
								add_var("[chams]-[" + val + "] visible only", settings_cur.only_visible);
								add_var("[chams]-[" + val + "] visible", settings_cur.visible);
								add_var("[chams]-[" + val + "] invisible", settings_cur.invisible);
								add_var("[chams]-[" + val + "] glow visible", settings_cur.glow_visible);
								add_var("[chams]-[" + val + "] glow invisible", settings_cur.glow_invisible);
							}
							end_group();
						}
						add_group("oher chams");
						{
							add_group("ragdoll");
							{
								for (auto& [id, val] : entity_names_ragdoll)
								{
									auto& settings_cur = settings::visuals::chams::chams_items_ragdoll[id];

									add_group(val);
									{
										add_var("[ragdoll]-[" + val + "] enable", settings_cur.enable);
										add_var("[ragdoll]-[" + val + "] type", settings_cur.chams_type);
										add_var("[ragdoll]-[" + val + "] visible only", settings_cur.only_visible);
										add_var("[ragdoll]-[" + val + "] visible", settings_cur.visible);
										add_var("[ragdoll]-[" + val + "] invisible", settings_cur.invisible);
										add_var("[ragdoll]-[" + val + "] glow visible", settings_cur.glow_visible);
										add_var("[ragdoll]-[" + val + "] glow invisible", settings_cur.glow_invisible);
									}
									end_group();
								}
							}
							end_group();

							add_group("localplayer");
							{
								add_group("real model");
								{
									add_var("[real model] enable", settings::visuals::chams::local_model::real);
									add_var("[real model] type", settings::visuals::chams::local_model::real_type);
									add_var("[real model] color", settings::visuals::chams::local_model::real_color);
								}
								end_group();

								add_group("weapon");
								{
									add_var("[weapon] enable", settings::visuals::chams::local_model::weapon);
									add_var("[weapon] type", settings::visuals::chams::local_model::weapon_type);
									add_var("[weapon] color", settings::visuals::chams::local_model::weapon_color);
								}
								end_group();

								add_group("hands");
								{
									add_var("[hands] enable", settings::visuals::chams::local_model::hands);
									add_var("[hands] type", settings::visuals::chams::local_model::hands_type);
									add_var("[hands] color", settings::visuals::chams::local_model::hands_color);
								}
								end_group();
							}
							end_group();

							add_group("backtrack");
							{
								add_var("[backtrack] enable", settings::visuals::chams::backtrack::enable);
								add_var("[backtrack] draw type", settings::visuals::chams::backtrack::type_draw);
								add_var("[backtrack] type", settings::visuals::chams::backtrack::type);
								add_var("[backtrack] visible only", settings::visuals::chams::backtrack::only_visible);
								add_var("[backtrack] visible", settings::visuals::chams::backtrack::color_visible);
								add_var("[backtrack] invisible", settings::visuals::chams::backtrack::color_invisible);
							}
							end_group();
						}
						end_group();

					}
					end_group();

					add_group("misc");
					{
						add_var("[sniper crosshair] enable", settings::misc::sniper_crosshair);

						add_group("hitbox");
						{
							add_var("[hitbox] enable", settings::visuals::hitbox::enable);
							add_var("[hitbox] color", settings::visuals::hitbox::color);
							add_var("[hitbox] show time", settings::visuals::hitbox::show_time);
							add_var("[hitbox] show type", settings::visuals::hitbox::show_type);
						}
						end_group();

						add_group("ofc");
						{
							add_var("[ofc] enable", settings::visuals::ofc::enable);
							add_var("[ofc] visible only", settings::visuals::ofc::visible_check);
							add_var("[ofc] color", settings::visuals::ofc::color);
							add_var("[ofc] range", settings::visuals::ofc::range);
							add_var("[ofc] hight", settings::visuals::ofc::hight);
							add_var("[ofc] wight", settings::visuals::ofc::wight);
						}
						end_group();

						add_group("bullet tracer");
						{
							add_var("[bullet tracer] enable", settings::visuals::bullet_impact::line::enable);
							add_var("[bullet tracer] enemies", settings::visuals::bullet_impact::line::enemies);
							add_var("[bullet tracer] show time", settings::visuals::bullet_impact::line::time);
							add_var("[bullet tracer] color enemies", settings::visuals::bullet_impact::line::color_enemies);
							add_var("[bullet tracer] teammates", settings::visuals::bullet_impact::line::teammates);
							add_var("[bullet tracer] color teammates", settings::visuals::bullet_impact::line::color_teammates);
							add_var("[bullet tracer] localplayer", settings::visuals::bullet_impact::line::local_player);
							add_var("[bullet tracer] color localplayer", settings::visuals::bullet_impact::line::color_local);
						}
						end_group();

						add_group("hits");
						{
							add_var("[hits] hitmarker", settings::misc::hitmarker);
						    add_var("[hits] hitsound", settings::misc::hitsound::enable);
						    add_var("[hits] hitsound volume", settings::misc::hitsound::volume);
						}
						end_group();

						add_group("bullet impact");
						{
							add_group("client");
							{
								add_var("[bullet impact]-[client] enable", settings::visuals::bullet_impact::box::enable_c);
								add_var("[bullet impact]-[client] size", settings::visuals::bullet_impact::box::size_c);
								add_var("[bullet impact]-[client] show time", settings::visuals::bullet_impact::box::time_c);
								add_var("[bullet impact]-[client] color", settings::visuals::bullet_impact::box::color_c);
							}
							end_group();

							add_group("server");
							{
								add_var("[bullet impact]-[server] enable", settings::visuals::bullet_impact::box::enable_s);
								add_var("[bullet impact]-[server] size", settings::visuals::bullet_impact::box::size_s);
								add_var("[bullet impact]-[server] show time", settings::visuals::bullet_impact::box::time_s);
								add_var("[bullet impact]-[server] color", settings::visuals::bullet_impact::box::color_s);
							}
							end_group();
						}
						end_group();

						add_group("grenades");
						{
							add_var("[grenades] enable", settings::visuals::grenades::enable);

							add_var("[grenades] molotov radius", settings::visuals::grenades::molotov_radius);
							add_var("[grenades] molotov bar", settings::visuals::grenades::molotov_bar);
							add_var("[grenades] molotov timer", settings::visuals::grenades::molotov_timer);
							add_var("[grenades] molotov color", settings::visuals::grenades::color_molotov);
							add_var("[grenades] molotov bar color", settings::visuals::grenades::color_bar_molotov_main);
							add_var("[grenades] molotov bar color back", settings::visuals::grenades::color_bar_molotov_back);
							add_var("[grenades] molotov color radius", settings::visuals::grenades::color_molotov_radius);


							add_var("[grenades] smoke radius", settings::visuals::grenades::smoke_radius);
							add_var("[grenades] smoke bar", settings::visuals::grenades::smoke_bar);
							add_var("[grenades] smoke timer", settings::visuals::grenades::smoke_timer);
							add_var("[grenades] smoke color", settings::visuals::grenades::color_smoke);
							add_var("[grenades] smoke bar color", settings::visuals::grenades::color_bar_smoke_main);
							add_var("[grenades] smoke bar color back", settings::visuals::grenades::color_bar_smoke_back);
							add_var("[grenades] smoke color radius", settings::visuals::grenades::color_smoke_radius);

							add_var("[grenades] he color", settings::visuals::grenades::color_frag);
							add_var("[grenades] flash color", settings::visuals::grenades::color_flash);
							add_var("[grenades] decoy color", settings::visuals::grenades::color_decoy);
						}
						end_group();

						add_group("spreed circle");
						{
							add_var("[spreed circle] enable", settings::misc::spreed_circle::enable);
							add_var("[spreed circle] color", settings::misc::spreed_circle::color);
						}
						end_group();

						add_group("grenade prediction");
						{
							add_var("[grenade prediction] enable", settings::visuals::grenade_prediction::enable);
							add_var("[grenade prediction] radius", settings::visuals::grenade_prediction::radius);
							add_var("[grenade prediction] radius color", settings::visuals::grenade_prediction::radius_color);

							add_var("[grenade prediction] line thickness", settings::visuals::grenade_prediction::line_thickness);
							add_var("[grenade prediction] colision box size", settings::visuals::grenade_prediction::colision_box_size);
							add_var("[grenade prediction] main colision box size", settings::visuals::grenade_prediction::main_colision_box_size);

							add_var("[grenade prediction] line color", settings::visuals::grenade_prediction::main);
							add_var("[grenade prediction] colision box color", settings::visuals::grenade_prediction::main_box);
							add_var("[grenade prediction] end colision box color", settings::visuals::grenade_prediction::end_box);
						}
						end_group();


						add_group("fov's");
						{
							add_var("[fov] draw standart fov", settings::visuals::fov::fov_basic);
							add_var("[fov] draw standart fov background", settings::visuals::fov::fov_basic_background);
							add_var("[fov] standart color", settings::visuals::fov::color_fov_basic);
							add_var("[fov] standart color background", settings::visuals::fov::color_fov_basic_background);

							add_var("[fov] draw silent fov", settings::visuals::fov::fov_silent);
							add_var("[fov] draw silent fov background", settings::visuals::fov::fov_silent_background);
							add_var("[fov] silent color", settings::visuals::fov::color_fov_silent);
							add_var("[fov] silent color background", settings::visuals::fov::color_fov_silent_background);
						}
						end_group();

						add_group("damage indicator");
						{
							add_var("[damage indicator] enable", settings::visuals::damage_indicator::enable);

							add_var("[damage indicator] speed", settings::visuals::damage_indicator::speed);
							add_var("[damage indicator] show time", settings::visuals::damage_indicator::show_time);

							add_var("[damage indicator] offset at hit", settings::visuals::damage_indicator::offset_hit);
							add_var("[damage indicator] offset at kill", settings::visuals::damage_indicator::offset_if_kill);

							add_var("[damage indicator] max position y", settings::visuals::damage_indicator::max_pos_y);
							add_var("[damage indicator] range random x", settings::visuals::damage_indicator::range_offset_x);
						}
						end_group();

						add_group("dropped weapon");
						{
							add_var("[dropped weapon] enable", settings::visuals::dropped_weapon::enable);

							add_var("[dropped weapon] box", settings::visuals::dropped_weapon::box);
							add_var("[dropped weapon] box color", settings::visuals::dropped_weapon::box_color);

							add_var("[dropped weapon] chams", settings::visuals::dropped_weapon::chams);
							add_var("[dropped weapon] chams type", settings::visuals::dropped_weapon::chams_type);
							add_var("[dropped weapon] chams color", settings::visuals::dropped_weapon::chams_color);

							add_var("[dropped weapon] ammo bar", settings::visuals::dropped_weapon::ammo_bar);
							add_var("[dropped weapon] ammo color outline", settings::visuals::dropped_weapon::bar_outline);
							add_var("[dropped weapon] ammo color back", settings::visuals::dropped_weapon::bar_background);
							add_var("[dropped weapon] ammo color main", settings::visuals::dropped_weapon::bar_main);
						}
						end_group();

						add_group("events");
						{
							add_group("screen");
							{
								add_var("[events]-[screen] hit", settings::visuals::events::hurt.enable_screen);
								add_var("[events]-[screen] buy", settings::visuals::events::player_buy.enable_screen);
								add_var("[events]-[screen] planting", settings::visuals::events::planting.enable_screen);
								add_var("[events]-[screen] defuse", settings::visuals::events::defusing.enable_screen);
								add_var("[events]-[screen] config", settings::visuals::events::config.enable_screen);
								add_var("[events]-[screen] lua", settings::visuals::events::lua.enable_screen);
							}
							end_group();

							add_group("console");
							{
								add_var("[events]-[console] hit", settings::visuals::events::hurt.enable_console);
								add_var("[events]-[console] buy", settings::visuals::events::player_buy.enable_console);
								add_var("[events]-[console] planting", settings::visuals::events::planting.enable_console);
								add_var("[events]-[console] defuse", settings::visuals::events::defusing.enable_console);
								add_var("[events]-[console] config", settings::visuals::events::config.enable_console);
								add_var("[events]-[console] lua", settings::visuals::events::lua.enable_console);
							}
							end_group();

							add_group("colors");
							{
								add_var("[events]-[custom colors] hit", settings::visuals::events::hurt.custom_color);
								add_var("[events]-[custom colors] buy", settings::visuals::events::player_buy.custom_color);
								add_var("[events]-[custom colors] planting", settings::visuals::events::planting.custom_color);
								add_var("[events]-[custom colors] defuse", settings::visuals::events::defusing.custom_color);
								add_var("[events]-[custom colors] config", settings::visuals::events::config.custom_color);
								add_var("[events]-[custom colors] lua", settings::visuals::events::lua.custom_color);

								add_var("[events]-[custom colors] hit color", settings::visuals::events::hurt.color);
								add_var("[events]-[custom colors] buy color", settings::visuals::events::player_buy.color);
								add_var("[events]-[custom colors] planting color", settings::visuals::events::planting.color);
								add_var("[events]-[custom colors] defuse color", settings::visuals::events::defusing.color);
								add_var("[events]-[custom colors] config color", settings::visuals::events::config.color);
								add_var("[events]-[custom colors] lua color", settings::visuals::events::lua.color);
							}
							end_group();
						}
						end_group();
					}
					end_group();
				}
				end_group();
			}
			end_group();

			add_group("windows");
			{
				add_var("[windows] spectator list pos", "spectator list", settings::windows::Spec_pos.x, settings::windows::Spec_pos.y);
				add_var("[windows] speclist alpha", settings::windows::Spec_alpha);

				add_var("[windows] bind window pos", "bind's", settings::windows::Bind_pos.x, settings::windows::Bind_pos.y);
				add_var("[windows] bind window alpha", settings::windows::Bind_alpha);
			}
			end_group();

			add_group("misc");
			{
				add_group("gravity");
				{
					add_var("[misc]-[gravity] slow gravity", settings::misc::inverse_gravity::enable_slow);
					add_var("[misc]-[gravity] inverse gravity", settings::misc::inverse_gravity::enable);
					add_var("[misc]-[gravity] inverse gravity value", settings::misc::inverse_gravity::value);
				}
				end_group();

				add_var("[misc] menu color", settings::misc::menu_color);
				add_var("[misc] radar ingame", settings::visuals::radar_ingame);
				add_var("[misc] bhop", settings::misc::bhop::enable);
				add_var("[misc] auto strafer", settings::misc::bhop::auto_strafer);
				add_var("[misc] auto strafer type", settings::misc::bhop::strafer_type);
				add_var("[misc] retrack speed", settings::misc::bhop::retrack_speed);
				add_var("[misc] bind window", settings::misc::bind_window::enable);
				add_var("[misc] speclist window", settings::visuals::spectator_list);
				add_var("[misc] block bot", settings::misc::block_bot::enable);
				add_var("[misc] block bot bind", settings::misc::block_bot::bind);
				add_var("[misc] clantag enable", settings::misc::clantag::enable);
				add_var("[misc] clantag type", settings::misc::clantag::clantag_type);
				add_var("[misc] clantag animation type", settings::misc::clantag::animation_type);
				add_var("[misc] clantag custom type", settings::misc::clantag::custom_type);
				add_var("[misc] clantag speed", settings::misc::clantag::speed);
				add_var("[misc] clantag check on empty", settings::misc::clantag::check_empty);
				add_var("[misc] custom clantag clantag", settings::misc::clantag::clantag);
				add_var("[misc] desync type", settings::misc::desync::type);
				add_var("[misc] desync side", settings::misc::desync::side);
				add_var("[misc] desync bind", settings::misc::desync::bind);
				add_group("disable");
				{
					add_var("[disable] flash", settings::misc::disable_flash);
					add_var("[disable] flash alpha", settings::misc::flash_alpha);
					add_var("[disable] smoke", settings::misc::disable_smoke);
					add_var("[disable] zoom", settings::misc::disable_zoom);
					add_var("[disable] zoom border", settings::misc::disable_zoom_border);
				}
				end_group();

				add_var("[misc] edge jump", settings::misc::edge_jump::enable);
				add_var("[misc] auto duck", settings::misc::edge_jump::auto_duck);
				add_var("[misc] edge jump key", settings::misc::edge_jump::bind);
				add_var("[misc] fast duck", settings::misc::fast_duck);
				add_var("[misc] moon walk", settings::misc::moon_walk);
				add_var("[misc] night mode", settings::misc::nightmode);
				add_var("[misc] reveal money", settings::misc::reveal_money);
				add_var("[misc] reveal rank", settings::misc::reveal_rank);
				add_var("[misc] slowwalk", settings::misc::slowwalk::enable);
				add_var("[misc] slowwalk speed", settings::misc::slowwalk::speed);
				add_var("[misc] slowwalk key", settings::misc::slowwalk::bind);
				add_var("[misc] third person", settings::misc::third_person::enable);
				add_var("[misc] third person dist", settings::misc::third_person::dist);
				add_var("[misc] third person key", settings::misc::third_person::bind);

				add_group("fov");
				{
					add_var("[fov] override", settings::misc::viewmodel::override);
					add_var("[fov] viewmodel", settings::misc::viewmodel::viewmodel);
					add_var("[fov] x", settings::misc::viewmodel::fov_x);
					add_var("[fov] y", settings::misc::viewmodel::fov_y);
					add_var("[fov] z", settings::misc::viewmodel::fov_z);
				}
				end_group();

				add_var("[misc] watermark", settings::misc::watermark);
				add_var("[misc] bomb timer", settings::visuals::bomb_timer);
			}
			end_group();

			add_group("profile changer");
			{
				for (auto& [id, val] : mode_names)
				{
					add_group(val);
					{
						add_var("[profile changer]-[" + val + "] rank", settings::changers::profile::profile_items[id].rank_id);
						add_var("[profile changer]-[" + val + "] wins", settings::changers::profile::profile_items[id].wins);
					}
					end_group();
				}
				add_var("[profile changer] private rank", settings::changers::profile::private_id);
				add_var("[profile changer] private xp", settings::changers::profile::private_xp);

				add_group("comments");
				{
					add_var("[profile changer]-[comments] teacher", settings::changers::profile::teacher);
					add_var("[profile changer]-[comments] friendly", settings::changers::profile::friendly);
					add_var("[profile changer]-[comments] leader", settings::changers::profile::leader);
				}
				end_group();

				add_group("ban");
				{
					add_var("[profile changer]-[ban] type", settings::changers::profile::ban_type);
					add_var("[profile changer]-[ban] time", settings::changers::profile::ban_time);
					add_var("[profile changer]-[ban] time type", settings::changers::profile::ban_time_type);
				}
				end_group();

			}
			end_group();

		}
		void add_var(std::string name, std::string wind, float& x, float& y)
		{
			c_utils::add_var(cur_line, cur_group, config, windows, name, wind, x, y);
		}
		void add_var(std::string name, std::string& var)
		{
			c_utils::add_var(cur_line, cur_group, config, strings, name, var);
		}
		void add_var(std::string name, int& var)
		{
			c_utils::add_var(cur_line, cur_group, config, ints, name, var);
		}
		void add_var(std::string name, float& var)
		{
			c_utils::add_var(cur_line, cur_group, config, floats, name, var);
		}
		void add_var(std::string name, KeyBind_t& var)
		{
			c_utils::add_var(cur_line, cur_group, config, binds, name, var);
		}
		void add_var(std::string name, bool& var)
		{
			c_utils::add_var(cur_line, cur_group, config, bools, name, var);
		}

		void add_var(std::string name, Color& var)
		{
			c_utils::add_var(cur_line, cur_group, config, colors, name, var);
		}

		void add_space()
		{
			c_utils::add_space(cur_group, config);
		}

		void add_group(std::string name)
		{
			c_utils::add_group(cur_line, cur_group, config, name);
		}

		void end_group()
		{
			c_utils::end_group(cur_group);
		}

		void save(std::string name)
		{
			if (name.empty())
				return;

			setup_set();

			c_utils::save("standart", config, name);
			
			Color color = settings::visuals::events::config.custom_color ? settings::visuals::events::config.color : settings::misc::menu_color;

			if (settings::visuals::events::enable)
			{
				if (settings::visuals::events::config.enable_screen)
					notify::add("saved - " + name, 7500);
				if (settings::visuals::events::config.enable_console)
					notify::add("config", "saved - " + name, color, log_type::console);
			}
		}

		void load(std::string name)
		{
			c_utils::load("standart", name, ints, bools, floats, colors, binds, strings, windows);

			Color color = settings::visuals::events::config.custom_color ? settings::visuals::events::config.color : settings::misc::menu_color;

			if (settings::visuals::events::enable)
			{
				if (settings::visuals::events::config.enable_screen)
					notify::add("loaded - " + name, 7500);
				if (settings::visuals::events::config.enable_console)
					notify::add("config", "loaded - " + name, color, log_type::console);
			}
		}
	}

	namespace skins
	{
		void clear()
		{
			config.clear();

			ints.clear();
			bools.clear();
			binds.clear();
			floats.clear();
			colors.clear();
			strings.clear();
			windows.clear();

			cur_line = 0;
			cur_group = 0;
		}

		void setup_set()
		{
			clear();
		
			add_group("general");
			{
				add_var("[general] show skins for selected weapon", settings::changers::skin::show_cur);
				add_var("[general] skin preview", settings::changers::skin::skin_preview);
			}
			end_group();

			add_group("skinchanger");
			{
				for (auto& val : k_weapon_names)
				{
					add_group(val.name);
					{
						auto& settings_cur = settings::changers::skin::m_items[val.definition_index];

						add_var("[" + val.name +"] paint kit", settings_cur.paint_kit_index);
						add_var("[" + val.name + "] paint kit index", settings_cur.paint_kit_vector_index);
						add_var("[" + val.name + "] selected model idx", settings_cur.definition_override_index);
						add_var("[" + val.name + "] selected model", settings_cur.definition_override_vector_index);
					}
					end_group();
				}
			}
			end_group();

			add_group("skin preview");
			{
				for (int i = 0; i < k_weapon_names.size(); i++)
				{
					if (i > 3)
					{
						add_group(k_weapon_names[i].name);
						{
							add_var("[skin preview] [" + k_weapon_names[i].name + "] name", k_weapon_names[i]._weapon_name);
							add_var("[skin preview] [" + k_weapon_names[i].name + "] name skin", k_weapon_names[i].skin_name);
						}
						end_group();
					}
				}

				add_group("ct side");
				{
					for (int i = 0; i < k_ct_knife_names.size(); i++)
					{
						if (i > 2)
						{
							add_group(k_ct_knife_names[i].name);
							{
								add_var("[ct skin preview]-[ct side]-[" + k_ct_knife_names[i].name + "] name", k_ct_knife_names[i]._weapon_name);
								add_var("[ct skin preview]-[ct side]-[" + k_ct_knife_names[i].name + "] name skin", k_ct_knife_names[i].skin_name);
							}
							end_group();
						}
					}

					for (int i = 0; i < k_ct_glove_names.size(); i++)
					{
						if (i > 2)
						{
							add_group(k_ct_glove_names[i].name);
							{
								add_var("[skin preview]-[ct side]-[" + k_ct_glove_names[i].name + "] name", k_ct_glove_names[i]._weapon_name);
								add_var("[skin preview]-[ct side]-[" + k_ct_glove_names[i].name + "] name skin", k_ct_glove_names[i].skin_name);
							}
							end_group();
						}
					}
				}
				end_group();			

				add_group("t side");
				{
					for (int i = 0; i < k_t_knife_names.size(); i++)
					{
						if (i > 2)
						{
							add_group(k_t_knife_names[i].name);
							{
								add_var("[skin preview]-[t side]-[" + k_t_knife_names[i].name + "] name", k_t_knife_names[i]._weapon_name);
								add_var("[skin preview]-[t side]-[" + k_t_knife_names[i].name + "] name skin", k_t_knife_names[i].skin_name);
							}
							end_group();
						}
					}

					for (int i = 0; i < k_t_glove_names.size(); i++)
					{
						if (i > 2)
						{
							add_group(k_t_glove_names[i].name);
							{
								add_var("[skin preview]-[t side]-[" + k_t_glove_names[i].name + "] name", k_t_glove_names[i]._weapon_name);
								add_var("[skin preview]-[t side]-[" + k_t_glove_names[i].name + "] name skin", k_t_glove_names[i].skin_name);
							}
							end_group();
						}
					}
				}
				end_group();
			}
			end_group();

		}
		void add_var(std::string name, std::string wind, float& x, float& y)
		{
			c_utils::add_var(cur_line, cur_group, config, windows, name, wind, x, y);
		}
		void add_var(std::string name, std::string& var)
		{
			c_utils::add_var(cur_line, cur_group, config, strings, name, var);
		}
		void add_var(std::string name, int& var)
		{
			c_utils::add_var(cur_line, cur_group, config, ints, name, var);
		}
		void add_var(std::string name, float& var)
		{
			c_utils::add_var(cur_line, cur_group, config, floats, name, var);
		}
		void add_var(std::string name, KeyBind_t& var)
		{
			c_utils::add_var(cur_line, cur_group, config, binds, name, var);
		}
		void add_var(std::string name, bool& var)
		{
			c_utils::add_var(cur_line, cur_group, config, bools, name, var);
		}

		void add_var(std::string name, Color& var)
		{
			c_utils::add_var(cur_line, cur_group, config, colors, name, var);
		}

		void add_space()
		{
			c_utils::add_space(cur_group, config);
		}

		void add_group(std::string name)
		{
			c_utils::add_group(cur_line, cur_group, config, name);
		}

		void end_group()
		{
			c_utils::end_group(cur_group);
		}

		void save(std::string name)
		{
			if (name.empty())
				return;

			setup_set();

			c_utils::save("skins", config, name);

			Color color = settings::visuals::events::config.custom_color ? settings::visuals::events::config.color : settings::misc::menu_color;

			if (settings::visuals::events::enable)
			{
				if (settings::visuals::events::config.enable_screen)
					notify::add("saved - " + name, 7500);
				if (settings::visuals::events::config.enable_console)
					notify::add("config", "saved - " + name, color, log_type::console);
			}
		}

		void load(std::string name)
		{
			c_utils::load("skins", name, ints, bools, floats, colors, binds, strings, windows);

			Color color = settings::visuals::events::config.custom_color ? settings::visuals::events::config.color : settings::misc::menu_color;

			if (settings::visuals::events::enable)
			{
				if (settings::visuals::events::config.enable_screen)
					notify::add("loaded - " + name, 7500);
				if (settings::visuals::events::config.enable_console)
					notify::add("config", "loaded - " + name, color, log_type::console);
			}
		}
	}
}