#include "../hooks.h"

namespace hooks
{
	int __fastcall send_datagram::hook(void* net_channel, void*, void* datagram)
	{
		static auto original_fn = netchannel_vhook.get_original<fn>(index::send_datagram);

		if (!settings::misc::fake_latency::enable || !g_engine_client->is_connected() || !g_local_player)
			return original_fn(net_channel, datagram);

		auto* channel = reinterpret_cast<i_net_channel*>(net_channel);

		int instate = channel->in_reliable_state;
		int insequencenr = channel->in_sequence_nr;


		misc::fake_latency::add_latency(channel);


		int ret = original_fn(channel, datagram);

		channel->in_reliable_state = instate;
		channel->in_sequence_nr = insequencenr;

		return ret;
	}
}