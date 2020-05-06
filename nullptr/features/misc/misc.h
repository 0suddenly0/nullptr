#pragma once
#include <deque>
#include "../../valve_sdk/sdk.h"

namespace misc
{
	void inverse_gravity();
	void prepare_revolver(c_usercmd* cmd);
	void moon_walk(c_usercmd* cmd);
	void fast_duck(c_usercmd* cmd);
	void block_bot(c_usercmd* cmd);
	void edge_jump(c_usercmd* cmd, int old_flags);
	void disable_flash_alpha();
	void no_smoke();
	void fog();

	namespace clan_tag
	{
		void init();

		extern int user_index;
	}
	namespace fake_latency
	{
		struct incoming_sequence_t {
			incoming_sequence_t::incoming_sequence_t(int instate, int outstate, int seqnr, float time) {
				m_in_reliable_state = instate;
				m_out_reliable_state = outstate;
				m_sequence_nr = seqnr;
				m_current_time = time;
			}

			int m_in_reliable_state;
			int m_out_reliable_state;
			int m_sequence_nr;
			float m_current_time;
		};


		std::deque< incoming_sequence_t > sequences;
		int m_last_incoming_sequence_number;

		void update_sequence();
		void clear_sequence();
		void add_latency(i_net_channel* net_channel);

	}
	namespace bhop
	{
		void on_create_move(c_usercmd* cmd);
		void auto_strafe(c_usercmd* cmd, QAngle va);
	}
	namespace desync
	{
		float side = -1.f;
		bool m_bBreakLowerBody = false;
		float m_flLastLby = 0.0f;
		float m_flNextBodyUpdate = 0.f;
		bool m_bBreakBalance = false;

		float desync_amount;
		bool flip_packet;
		float next_lby_update_time = 0.f;

		void next_lby_update(c_usercmd* cmd, bool* sendpacket);

		bool is_firing(c_usercmd* cmd);
		bool is_enabled(c_usercmd* cmd);
		void handle(c_usercmd* cmd, QAngle currentViewAngles, bool* send_packet);
	}
	namespace fake_lags
	{
		void handle(c_usercmd* cmd, bool* send_packet);
	}
}