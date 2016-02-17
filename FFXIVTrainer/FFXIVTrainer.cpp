// FFXIVTrainer

#include "settings.h"

#include <iostream>
#include <string>

#include "stdafx.h"
#include "windows.h"
#include "psapi.h"


using namespace std;


/*const DWORD camera_pointer = 0x14E11B0;
const DWORD offset_current = 0x108;
const DWORD offset_max = 0x110;
const string version = "2015.11.21.0000.0000";*/

const float camera_default_max = 20;
const float camera_default_new_max = 120;

const char* window_name = "FINAL FANTASY XIV";
const string process_name = "ffxiv_dx11.exe";


DWORD_PTR GetModuleBase(HANDLE hProc, string sModuleName)
{
	HMODULE *hModules = NULL;
	char szBuf[50];
	DWORD cModules;
	DWORD_PTR dwBase = -1;

	EnumProcessModules(hProc, hModules, 0, &cModules);
	hModules = new HMODULE[cModules / sizeof(HMODULE)];

	if (EnumProcessModules(hProc, hModules, cModules / sizeof(HMODULE), &cModules)) {
		for (int i = 0; i < cModules / sizeof(HMODULE); ++i) {
			if (GetModuleBaseNameA(hProc, hModules[i], szBuf, sizeof(szBuf))) {
				if (sModuleName.compare(szBuf) == 0) {
					dwBase = (DWORD_PTR)hModules[i];
					break;
				}
			}
		}
	}

	delete[] hModules;

	return dwBase;
}

int main(int argc, char *argv[])
{
	DWORD pid;
	DWORD address;
	DWORD *exitCode;
	float zoom_current;
	float zoom_max;
	float camera_new_max;
	int result;
	Settings settings;


	cout << "FFXIV camera zoom hack - extends max range from "
		 << camera_default_max << " to " << camera_default_new_max << endl;

	cout << "For any errors, try running the process as an administrator"
		 << endl << endl;

	if (!loadSettings(settings)) {
		cin.ignore();
		return 1;
	}

	cout << "Current memory addresses are updated to work with version "
		 << settings.version << endl;


	HWND hwnd = FindWindowA(0, window_name);

	if (hwnd == 0) {
		cout << "Error: cannot find window" << endl;
		cin.ignore();
		return 1;
	}

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

	cout << "Process base address: 0x";
	cout << hex << base << endl;

	cout << "Using static pointer address: 0x";
	cout << settings.camera_pointer << endl;

	cout << "Searching pointer from address: 0x";
	cout << hex << (base + settings.camera_pointer) << endl;

	if (!ReadProcessMemory(hProc, (void*)(base + settings.camera_pointer), &address, 4, 0)) {
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
	}

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

	cout << endl;
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
	CloseHandle(hProc);

	cin.ignore();

    return 0;
}

