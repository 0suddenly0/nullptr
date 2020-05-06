#include "../hooks.h"

namespace hooks
{
	EGCResults __stdcall retrieve_message::hook(uint32_t* punMsgType, void* pubDest, uint32_t cubDest, uint32_t* pcubMsgSize)
	{

		auto oRetrieveMessage = retrieve_message_vhook.get_original< fn >(index::retrieve_message);

		const auto result = oRetrieveMessage(g_game_coordinator, punMsgType, pubDest, cubDest, pcubMsgSize);
		if (result != EGCResults::k_EGCResultOK)
			return result;

		uint32_t messageType = *punMsgType & 0x7FFFFFFF;

#ifdef _DEBUG
		utils::console_print("[->] message received from GC [%d]\n", messageType);
#endif


		void* thisPtr = nullptr;
		__asm mov thisPtr, ebx;
		auto oldEBP = *reinterpret_cast<void**>((uint32_t)_AddressOfReturnAddress() - 4);

		if (globals::profile::update_request)
		{
			write.receive_message(thisPtr, oldEBP, messageType, pubDest, cubDest, pcubMsgSize);
		}

		return result;
	}
}