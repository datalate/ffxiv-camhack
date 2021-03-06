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

FFXIV::FFXIV(Settings& settings, float zoom_value, bool debug):
	settings_(&settings), zoom_max_custom_(zoom_value), debug_(debug) {

	game_found_ = false;
	firstrun_ = true;
	disabled_ = false;

	proc_handle_ = NULL;
	base_address_ = -1;

	address_zoom_current_ = 0;
	address_zoom_max_ = 0;

	zoom_current_ = 0;
	zoom_max_ = 0;
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

	DWORD_PTR read_address = base_address_ + settings_->camera_pointer;
	DWORD_PTR camera_address;

	if (!ReadProcessMemory(proc_handle_, (LPVOID)read_address,
						   &camera_address, sizeof(camera_address), NULL)) {

		std::cout << "Error: cannot read memory from address 0x"
				  << std::hex << read_address << std::endl;

		game_found_ = false;
		return false;
	}

	if (debug_) {
		std::cout << "Camera address found: 0x"
				  << std::hex << camera_address << std::endl << std::endl;
	}
	
	address_zoom_current_ = camera_address + settings_->offset_current;
	address_zoom_max_ = camera_address + settings_->offset_max;

	std::cout << "Camera addresses calculated successfully" << std::endl;

	return true;
}

void FFXIV::checkGameStatus() {
	if (!game_found_)
		return;

	exitCode = 0;
	GetExitCodeProcess(proc_handle_, &exitCode);

	if (exitCode != STILL_ACTIVE) {
		std::cout << "Game process exited or something..." << std::endl;

		firstrun_ = true;
		disabled_ = false;
		game_found_ = false;

		base_address_ = -1;

		address_zoom_current_ = 0;
		address_zoom_max_ = 0;

		zoom_current_ = 0;
		zoom_max_ = 0;
	}
}

void FFXIV::checkValues() {
	checkGameStatus();

	if (!game_found_)
		return;

	ReadProcessMemory(proc_handle_, (LPVOID)address_zoom_current_,
		              &zoom_current_, sizeof(zoom_current_), NULL);
	if (debug_)
		std::cout << "Current zoom value: " << zoom_current_ << std::endl;

	ReadProcessMemory(proc_handle_, (LPVOID)address_zoom_max_,
					  &zoom_max_, sizeof(zoom_max_), NULL);
	if (debug_)
		std::cout << "Current max zoom value: " << zoom_max_ << std::endl;

	if (disabled_)
		return;

	if (zoom_max_ == zoom_max_custom_) {
		if (firstrun_)
			std::cout << "The camera has already been adjusted" << std::endl;
		else
			return;
	}
	else {
		std::cout << "Camera not yet adjusted, trying to do that now..." << std::endl;

		int result = WriteProcessMemory(proc_handle_, (LPVOID)address_zoom_max_,
									    &zoom_max_custom_, sizeof(zoom_max_custom_), NULL);

		if (result != 1) {
			std::cout << "Error: couldn't write to process memory" << std::endl;
			if (debug_) {
				std::cout << "WriteProcessMemory(): Error code " << GetLastError() << std::endl;
			}
			return;
		}

		std::cout << "Successfully set max zoom value to " << zoom_max_custom_ << "!" << std::endl;
		zoom_current_ = zoom_max_custom_;
	}
	
	std::cout << "Now monitoring game..." << std::endl << std::endl;
	firstrun_ = false;
}

void FFXIV::toggleZoom() {
	checkGameStatus();

	if (!game_found_)
		return;

	int result;
	ReadProcessMemory(proc_handle_, (LPVOID)address_zoom_max_,
					  &zoom_max_, sizeof(zoom_max_), NULL);

	if (zoom_max_ == zoom_max_custom_) {
		float temp = zoom_max_default;

		result = WriteProcessMemory(proc_handle_, (LPVOID)address_zoom_max_,
									&temp, sizeof(temp), NULL);
		disabled_ = true;
	}
	else {
		result = WriteProcessMemory(proc_handle_, (LPVOID)address_zoom_max_,
									&zoom_max_custom_, sizeof(zoom_max_custom_), NULL);
		disabled_ = false;
	}

	if (result != 1)
		std::cout << "Error: couldn't write to process memory" << std::endl;
	else
		std::cout << "Hotkey pressed: toggled zoom value" << std::endl;
}