#include "ffxiv.h"

#include <string>
#include <iostream>
#include "settings.h"
#include "psapi.h"


namespace {
	DWORD_PTR GetModuleBase(HANDLE hProc, std::string sModuleName) {
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
}

FFXIV::FFXIV(Settings& settings): settings_(&settings) {
	game_found_ = false;
	firstrun_ = true;

	proc_handle_ = NULL;
	base_address_ = -1;

	DWORD address_zoom_current_ = 0;
	DWORD address_zoom_max_ = 0;

	float zoom_current_ = 0;
	float zoom_max_ = 0;
}

FFXIV::~FFXIV() {
	if (proc_handle_ != NULL) {
		CloseHandle(proc_handle_);
		proc_handle_ = NULL;
	}
}

bool FFXIV::game_found() {
	return game_found_;
}

bool FFXIV::findGame() {
	if (game_found_)
		return false;

	std::cout << "Trying to find game..." << std::endl;

	DWORD pid;

	if (proc_handle_ != NULL) {
		CloseHandle(proc_handle_);
		proc_handle_ = NULL;
	}

	HWND hwnd = FindWindowA(0, window_name);

	if (hwnd == 0) {
		std::cout << "Error: cannot find window" << std::endl;
		return false;
	}

	GetWindowThreadProcessId(hwnd, &pid);

	proc_handle_ = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);

	if (proc_handle_ == NULL) {
		std::cout << "Error: cannot open process" << std::endl;
		return false;
	}

	base_address_ = GetModuleBase(proc_handle_, process_name);

	if (base_address_ == -1) {
		std::cout << "Error: unable to find process base address" << std::endl;
		return false;
	}

	game_found_ = true;

	std::cout << "Found game process and base address" << std::endl;

	return calculateAddresses();
}

bool FFXIV::calculateAddresses() {
	if (!game_found_)
		return false;

	DWORD camera_address;

	if (!ReadProcessMemory(proc_handle_, (void*)(base_address_ + settings_->camera_pointer),
						   &camera_address, 4, 0)) {

		std::cout << "Error: cannot read memory from address 0x"
				  << std::hex << (base_address_ + settings_->camera_pointer) << std::endl;

		game_found_ = false;
		return false;
	}

	//std::cout << "Camera address found: 0x";
	//std::cout << camera_address << std::endl << std::endl;

	address_zoom_current_ = camera_address + settings_->offset_current;
	address_zoom_max_ = camera_address + settings_->offset_max;

	std::cout << "Camera addresses calculated successfully" << std::endl;

	return true;
}

void FFXIV::checkValues() {
	if (!game_found_)
		return;

	ReadProcessMemory(proc_handle_, (void*)address_zoom_current_, &zoom_current_, 4, 0);
	//std::cout << "Current zoom value: " << zoom_current_ << std::endl;

	ReadProcessMemory(proc_handle_, (void*)address_zoom_max_, &zoom_max_, 4, 0);
	//std::cout << "Current max zoom value: " << zoom_max_ << std::endl;

	if (zoom_max_ == zoom_max_custom_) {
		if (firstrun_) {
			std::cout << "The camera has already been adjusted" << std::endl;
			firstrun_ = false;
		}
		else return;
	} else {
		std::cout << "Camera not yet adjusted, trying to do that now..." << std::endl;
		int result = WriteProcessMemory(proc_handle_, (void*)address_zoom_max_,
									    &zoom_max_custom_, (DWORD)sizeof(zoom_max_custom_), NULL);

		if (result != 1) {
			std::cout << "Error: couldn't write to process memory" << std::endl;
			return;
		}

		std::cout << "Successfully set max zoom value to " << zoom_max_custom_ << "!" << std::endl;
		zoom_current_ = zoom_max_custom_;
	}
	
	std::cout << "Now monitoring game..." << std::endl << std::endl;
}