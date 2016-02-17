// FFXIVTrainer

#include "ffxiv.h"
#include "settings.h"

#include <iostream>
#include "stdafx.h"


int main(int argc, char *argv[])
{
	Settings settings;
	//DWORD *exitCode;


	std::cout << "FFXIV camera zoom hack - extends max range from "
		 << zoom_max_default_ << " to " << zoom_max_custom_ << std::endl;

	std::cout << "For any errors, try running the process as an administrator"
		 << std::endl << std::endl;


	if (!loadSettings(settings)) {
		std::cin.ignore();
		return 1;
	}

	FFXIV ffxiv(settings);

	while (true) {
		Sleep(1000);

		if (!ffxiv.game_found()) {
			if (!ffxiv.findGame())
				continue;
		} else {
			ffxiv.checkValues();
		}
	}



	

	/*cout << "Process base address: 0x";
	cout << hex << base << endl;

	cout << "Using static pointer address: 0x";
	cout << settings.camera_pointer << endl;

	cout << "Searching pointer from address: 0x";
	cout << hex << (base + settings.camera_pointer) << endl;*/

	/*if (!ReadProcessMemory(hProc, (void*)(base + settings.camera_pointer), &address, 4, 0)) {
		cout << "Error: cannot read memory from address 0x";
		cout << hex << (base + settings.camera_pointer) << endl;
		CloseHandle(hProc);
		cin.ignore();
		return 1;
	}
	else
	{
		cout << "Camera address found: 0x";
		cout << address << endl << endl;

		ReadProcessMemory(hProc, (void*)(address + settings.offset_current), &zoom_current, 4, 0);
		cout << "Current zoom value: " << zoom_current << endl;

		ReadProcessMemory(hProc, (void*)(address + settings.offset_max), &zoom_max, 4, 0);
		cout << "Current max zoom value: " << zoom_max << endl;

		if (zoom_max < camera_default_max) {
			cout << "Error: max zoom value was under the default " << camera_default_max << endl;
			cout << "Memory addresses are probably outdated. Wait for an update" << endl;
			CloseHandle(hProc);
			cin.ignore();
			return 1;
		}

		camera_new_max = camera_default_new_max;
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

		if (zoom_max == camera_new_max) {
			cout << "The camera has already been adjusted" << endl;
			zoom_max = camera_new_max;
		}
		else
		{
			result = WriteProcessMemory(hProc, (void*)(address + settings.offset_max), &camera_new_max, (DWORD)sizeof(camera_new_max), NULL);

			if (result == 1) {
				cout << "Successfully set max zoom value to " << camera_new_max << endl;
				zoom_max = camera_new_max;
			}
			else
			{
				cout << "Error: couldn't write to process memory" << endl;
				CloseHandle(hProc);
				cin.ignore();
				return 1;
			}
		}
	}*/

	/*while (true) {
		Sleep(2000);

		GetExitCodeProcess(hProc, exitCode);
		if (*exitCode == STILL_ACTIVE) {
			cout << "process still active" << endl;

			ReadProcessMemory(hProc, (void*)(address + offset_max), &zoom_max, 4, 0);
			//cout << "Current max zoom value: " << zoom_max << endl;

			if (zoom_max == camera_new_max) {
				//cout << "The camera has already been adjusted" << endl;
				zoom_max = camera_new_max;
			}
			else
			{
				result = WriteProcessMemory(hProc, (void*)(address + offset_max), &camera_new_max, (DWORD)sizeof(camera_new_max), NULL);

				if (result == 1) {
					cout << "Successfully set max zoom value to " << camera_new_max << endl;
					zoom_max = camera_new_max;
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
		else
		{
			cout << "process not running" << endl;

			HWND hwnd = FindWindowA(0, window_name);

			if (hwnd == 0) {
				cout << "Error: cannot find window" << endl;
				continue;
			}

			CloseHandle(hProc);

			GetWindowThreadProcessId(hwnd, &pid);

			HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);

			if (hProc == NULL) {
				cout << "Error: cannot open process" << endl;
				cin.ignore();
				return 1;
			}

			DWORD_PTR base = GetModuleBase(hProc, "ffxiv_dx11.exe");

			if (base == -1) {
				cout << "Error: unable to find process base address" << endl;
				cin.ignore();
				return 1;
			}
		}
	}*/

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

