// FFXIVTrainer

#include "ffxiv.h"
#include "settings.h"
#include "config.h"
#include "registry.h"

#include <iostream>

int main(int argc, char *argv[])
{
	Settings settings;
	Config config;
	MSG msg;

	loadConfig(config);

	if (config.hide_window) {
		FreeConsole();
	}

	if (registryGetAutostart() != config.autostart) {
		registrySetAutostart(config.autostart, false);
	}

	std::cout << "FFXIV camera zoom hack - extends max range from "
		 << zoom_max_default << " to " << config.max_zoom << std::endl;

	std::cout << "For any errors, try running the process as an administrator"
		 << std::endl << std::endl;


	if (!loadSettings(settings)) {
		std::cin.ignore();
		return 1;
	}

	FFXIV ffxiv(settings, (float)config.max_zoom);

	if (config.toggle) {
		if (RegisterHotKey(NULL, 1, config.toggle_modifier | MOD_NOREPEAT, config.toggle_key)) {
			std::cout << "Registered a hotkey for toggling max zoom" << std::endl;
			msg = { 0 };
		} else {
			std::cout << "Error: couldn't register a toggle hotkey" << std::endl;
			config.toggle = false;
		}
	}

	while (true) {
		Sleep(1000);

		if (config.toggle) {
			if ((PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
				&& (msg.message == WM_HOTKEY)) {

				ffxiv.toggleZoom();
			}
		}

		if (!ffxiv.game_found())
			if (!ffxiv.findGame())
				continue;

		ffxiv.checkValues();
	}

	/*
		if (zoom_max < camera_default_max) {
			cout << "Error: max zoom value was under the default " << camera_default_max << endl;
			cout << "Memory addresses are probably outdated. Wait for an update" << endl;
			CloseHandle(hProc);
			cin.ignore();
			return 1;
		}
	*/

	/*
		if (argc > 1) {
			int temp = atoi(argv[1]);
			if ((temp < 20) || (temp > 1000000)) {
				cout << "Error: camera value needs to be in between 20 and 1000000" << endl;
			}
			else {
				cout << "Got camera value " << dec << temp << " from command-line" << endl;
				camera_new_max = temp;
			}
		}
	*/

	std::cin.ignore();
    return 0;
}
