#define NOMINMAX
#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
#include <Windows.h>
#include "hooks/hooks.h"
#include "render/render.h"
#include "helpers/input.h"
#include "helpers/utils.h"
#include "valve_sdk/sdk.h"
#include "options/config.h"
#include <filesystem>
#include "options/options.h"
#include "features/changers/skin/kit_parser.h"

#include "lua sdk/lua_sdk.h"

DWORD WINAPI attach(LPVOID base)
{
	CreateDirectoryA((LPCSTR)"C:\\nullptr", NULL);

	if (utils::wait_for_modules(10000, { L"client_panorama.dll", L"engine.dll", L"shaderapidx9.dll" }) == WAIT_TIMEOUT)
	{
		return FALSE;
	}

#ifdef _DEBUG
	utils::attach_console();
#endif

	try
	{
		lua::init_state();

		lua::init_command();
		config::standart::setup_set();
		config::skins::setup_set();

		utils::console_print("initializing...\n");

		interfaces::initialize();
		interfaces::dump();

		netvar_sys::initialize();
		input_sys::initialize();
		render::initialize();
		menu::initialize();

		chams::init();

		hooks::initialize();
		
#ifdef _DEBUG
		input_sys::register_hotkey(VK_END, [base]() {
			g_unload = true;
			});
#endif

		input_sys::register_hotkey(VK_INSERT, [base]() {
			menu::toggle();
			});


		utils::console_print("load interfaces and hooking func finished.\n");

		std::string text = utils::snprintf("version - beta | %.2f", NULLPTR_VERSION);

		notify::add("nullptr", "injected", log_type::console);
		notify::add("nullptr", text.c_str(), log_type::console);

		notify::add("injected", 7500);
		notify::add(text.c_str(), 7500);

		utils::console_print("parsing skins.\n");
		
		if (k_skins.size() == 0)
		{
			initialize_kits();
		}

		notify::add("kits initialized", 7500);
		notify::add("nullptr", "kits initialized", log_type::console);
		utils::console_print("all finished.\n");

		while (!g_unload)
			Sleep(1000);

		FreeLibraryAndExitThread(static_cast<HMODULE>(base), 1);

	}
	catch (const std::exception & ex)
	{
		utils::console_print("An error occured during initialization:\n");
		utils::console_print("%s\n", ex.what());
		utils::console_print("Press any key to exit.\n");
		utils::console_read_key();
		utils::detach_console();

		FreeLibraryAndExitThread(static_cast<HMODULE>(base), 1);
	}
}

BOOL WINAPI detach()
{
	lua::unload();
#ifdef _DEBUG
	utils::detach_console();
#endif

	input_sys::remove();

	hooks::shutdown();

	menu::shutdown();

	chams::shutdown();
	glow::shutdown();


	return TRUE;
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        DisableThreadLibraryCalls(hModule);
		CreateThread( 0, 0, (LPTHREAD_START_ROUTINE)attach, hModule, 0, 0 );
		return TRUE;
    case DLL_PROCESS_DETACH:
		if (lpReserved == nullptr)
			return detach();
        break;
    }
    return TRUE;
}

