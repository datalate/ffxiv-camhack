// FFXIVTrainer

#include "ffxiv.h"
#include "settings.h"
#include "config.h"

#include <iostream>

int main(int argc, char *argv[])
{
	Settings settings;
	Config config;

	loadConfig(config);

	//registry tricks here

	std::cout << "FFXIV camera zoom hack - extends max range from "
		 << zoom_max_default << " to " << config.max_zoom << std::endl;

	std::cout << "For any errors, try running the process as an administrator"
		 << std::endl << std::endl;


	if (!loadSettings(settings)) {
		std::cin.ignore();
		return 1;
	}

	FFXIV ffxiv(settings, (float)config.max_zoom);

	while (true) {
		Sleep(1000);

		if (!ffxiv.game_found()) {
			if (!ffxiv.findGame())
				continue;
		} else {
			ffxiv.checkValues();

			ffxiv.toggleZoom();
		}
	}



	/*cout << "Process base address: 0x";
	cout << hex << base << endl;

	cout << "Using static pointer address: 0x";
	cout << settings.camera_pointer << endl;

	cout << "Searching pointer from address: 0x";
	cout << hex << (base + settings.camera_pointer) << endl;*/


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


	/*
	if (RegisterHotKey(NULL, 1, MOD_ALT | MOD_NOREPEAT, 0x42))  //0x42 is 'b'
	{
		cout << "Registered a hotkey for toggling max zoom (ALT+B)" << endl;
	}
	else
	{
		cout << "Error: couldn't register a system hotkey" << endl;
	}

	MSG msg = {0};
	while (GetMessage(&msg, NULL, 0, 0) != 0)
	{
		if (msg.message == WM_HOTKEY)
		{
			if (zoom_max != camera_default_max) {
				zoom_max = camera_default_max;
			}
			else
			{
				zoom_max = camera_new_max;
			}

			result = WriteProcessMemory(hProc, (void*)(address + offset_max), &zoom_max, (DWORD)sizeof(zoom_max), NULL);

			if (result == 1) {
				cout << "Successfully set max zoom value to " << zoom_max << endl;
			}
			else
			{
				cout << "Error: couldn't write to process memory" << endl;
				CloseHandle(hProc);
				cin.ignore();
				return 1;
			}

		}
	}
	*/

	std::cin.ignore();
    return 0;
}
