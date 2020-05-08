#include "../menu.h"

namespace menu
{
	void tabs::profile_tab()
	{
		const char* ranks[] = {
		"off",
		"silver 1",
		"silver 2",
		"silver 3",
		"silver 4",
		"silver elite",
		"silver elite master",
		"gold nova 1",
		"gold nova 2",
		"gold nova 3",
		"gold nova master",
		"master guardian 1",
		"master guardian 2",
		"master guardian elite",
		"distinguished master guardian",
		"legendary eagle",
		"legendary eagle master",
		"supreme master first class",
		"the global elite"
		};
		const char* ranks_dz[] = {
			"off",
			"lab rat 1",
			"lab rat 2",
			"sprinting hare 1",
			"sprinting hare 2",
			"wild scout 1",
			"wild scout 2",
			"wild scout elite",
			"hunter fox 1",
			"hunter fox 2",
			"hunter fox 3",
			"hunter fox elite",
			"timber wolf",
			"ember wolf",
			"wildfire wolf",
			"the howling alpha"
		};
		const char* private_ranks[] = {
	"Private rank 1                | 1",
	"Private rank 2                | 2",
	"Private rank 3                | 3",
	"Private rank 4                | 4",
	"Corporal 1                      | 5",
	"Corporal 2                      | 6",
	"Corporal 3                      | 7",
	"Corporal 4                      | 8",
	"Sergeant 1                      | 9",
	"Sergeant 2                      | 10",
	"Sergeant 3                      | 11",
	"Sergeant 4                      | 12",
	"Master Sergeant 1         | 13",
	"Master Sergeant 2         | 14",
	"Master Sergeant 3         | 15",
	"Master Sergeant 4         | 16",
	"Sergeant Major 1           | 17",
	"Sergeant Major 2           | 18",
	"Sergeant Major 3           | 19",
	"Sergeant Major 4           | 20",
	"Lieutenant 1                   | 21",
	"Lieutenant 2                   | 22",
	"Lieutenant 3                   | 23",
	"Lieutenant 4                   | 24",
	"Captain 1                        | 25",
	"Captain 2                        | 26",
	"Captain 3                        | 27",
	"Captain 4                        | 28",
	"Major 1                           | 29",
	"Major 2                           | 30",
	"Major 3                           | 31",
	"Major 4                           | 32",
	"Colonel 1                        | 33",
	"Colonel 2                        | 34",
	"Colonel 3                        | 35",
	"Brigadier General          | 36",
	"Major General                | 37",
	"Lieutenant General        | 38",
	"Ggeneral                         | 39",
	"Global General               | 40"
		};
		const char* bans[] = {
			"none",
			"you were kicked from the last match (competitive cooldown)",
			"you killed too many teammates (competitive cooldown)",
			"you killed a teammate at round start (competitive cooldown)",
			"you failed to reconnect to the last match (competitive cooldown)",
			"you abandoned the last match (competitive cooldown)",
			"you did too much damage to your teammates (competitive cooldown)",
			"you did too much damage to your teammates at round start (competitive cooldown)",
			"this account is permanently untrusted (global cooldown)",
			"you were kicked from too many recent matches (competitive cooldown)",
			"convicted by overwatch - majorly disruptive (global cooldown)",
			"convicted by overwatch - minorly disruptive (global cooldown)",
			"resolving matchmaking state for your account (temporary cooldown)",
			"resolving matchmaking state after the last match (temporary cooldown)",
			"this account is permanently untrusted (global cooldown)",
			"(global cooldown)",
			"you failed to connect by match start. (competitive cooldown)",
			"you have kicked too many teammates in recent matches (competitive cooldown)",
			"congratulations on your recent competitive wins! before you play competitive matches further please wait for matchmaking servers to calibrate your skill group placement based on your lastest performance. (temporary cooldown)",
			"a server using your game server login token has been banned. your account is now permanently banned from operating game servers, and you have a cooldown from connecting to game servers. (global cooldown)"
		};

		static char* modes[] =
		{
			"matchmaking",
			"wingman",
			"danger zone"
		};

		ImGui::Columns(2, nullptr, false);
		ImGui::BeginChild("general##profile_changer", ImVec2(0, 196), true, ImGuiWindowFlags_ChildWindowTitle);
		{
			horizontal_tabs(settings::changers::profile::mode_id, modes);

			if (settings::changers::profile::mode_id == 2)
				ImGui::Combo("rank", &settings::changers::profile::profile_items[settings::changers::profile::mode_id].rank_id, ranks_dz, IM_ARRAYSIZE(ranks_dz));
			else
				ImGui::Combo("rank", &settings::changers::profile::profile_items[settings::changers::profile::mode_id].rank_id, ranks, IM_ARRAYSIZE(ranks));

			ImGui::InputInt("wins", &settings::changers::profile::profile_items[settings::changers::profile::mode_id].wins, 1, 20);

			ImGui::Combo("private rank", &settings::changers::profile::private_id, private_ranks, IM_ARRAYSIZE(private_ranks));
			ImGui::SliderInt("xp", &settings::changers::profile::private_xp, 0, 5000);

		}
		ImGui::EndChild("general##profile_changer");


		if (ImGui::Button("update", ImVec2(285, 0)))
		{
			utils::message_send();
		}

		ImGui::NextColumn();

		ImGui::BeginChild("comment's##profile", ImVec2(0, 156), true, ImGuiWindowFlags_ChildWindowTitle);
		{
			ImGui::InputInt("friendly", &settings::changers::profile::friendly, 1, 10);
			ImGui::InputInt("teacher", &settings::changers::profile::teacher, 1, 10);
			ImGui::InputInt("leader", &settings::changers::profile::leader, 1, 10);
		}
		ImGui::EndChild("comment's##profile");
		ImGui::BeginChild("ban##profile", ImVec2(0, 144), true, ImGuiWindowFlags_ChildWindowTitle);
		{
			ImGui::Combo("ban type", &settings::changers::profile::ban_type, bans, IM_ARRAYSIZE(bans));
			ImGui::Combo("time type", &settings::changers::profile::ban_time_type, "sec\0min\0hour\0days");
			ImGui::InputInt("time", &settings::changers::profile::ban_time, 1, 100);
		}
		ImGui::EndChild("ban##profile");

	}
}