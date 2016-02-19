#include "registry.h"

#include "windows.h"
#include <iostream>


const wchar_t subkey_run[] = L"Software\\Microsoft\\Windows\\CurrentVersion\\Run";

bool registryGetAutostart() {
	HKEY hkey = NULL;

	if (RegCreateKeyExW(HKEY_LOCAL_MACHINE, subkey_run, 0, NULL, 0,
		KEY_READ, NULL, &hkey, NULL) != ERROR_SUCCESS) {

		std::cout << "Error: couldn't create registry key handle" << std::endl;
		return false;
	}

	wchar_t registry_exe_path[MAX_PATH] = {};
	DWORD size = sizeof(registry_exe_path);

	if (RegGetValueW(hkey, NULL, TEXT("ffxiv-camhack"), RRF_RT_REG_SZ,
					 NULL, registry_exe_path, &size) != ERROR_SUCCESS) {

		//std::cout << "Error: couldn't get key value" << std::endl;
		RegCloseKey(hkey);
		return false;
	}

	RegCloseKey(hkey);

	//TODO: function returning exe path
	wchar_t exe_path[MAX_PATH];
	DWORD path_len = GetModuleFileNameW(NULL, exe_path, MAX_PATH - 1);

	if (path_len == 0) {
		std::cout << "Error: couldn't get file path" << std::endl;
		RegCloseKey(hkey);
		return false;
	}

	exe_path[path_len] = 0;

	return (wcscmp(registry_exe_path, exe_path) == 0);
}

bool registrySetAutostart(bool start, bool hide) {
	HKEY hkey = NULL;

	std::cout << "Trying to update registry autostart value..." << std::endl;

	if (RegCreateKeyExW(HKEY_LOCAL_MACHINE, subkey_run, 0, NULL, 0,
		KEY_SET_VALUE, NULL, &hkey, NULL) != ERROR_SUCCESS) {

		std::cout << "Error: couldn't create registry key handle" << std::endl;
		return false;
	}

	if (start) {
		wchar_t exe_path[MAX_PATH];

		DWORD path_len = GetModuleFileNameW(NULL, exe_path, MAX_PATH - 1);
		if (path_len == 0) {
			std::cout << "Error: couldn't get file path" << std::endl;
			RegCloseKey(hkey);
			return false;
		}

		exe_path[path_len] = 0;

		if (RegSetValueExW(hkey, TEXT("ffxiv-camhack"), 0, REG_SZ, (LPBYTE)exe_path,
			(wcslen(exe_path)) * sizeof(wchar_t)) != ERROR_SUCCESS) {

			std::cout << "Error: couldn't add key to registry" << std::endl;
			RegCloseKey(hkey);
			return false;
		}

		std::cout << "Successfully enabled autostart!" << std::endl;
	} else {
		if (RegDeleteKeyValueW(hkey, NULL, TEXT("ffxiv-camhack")) != ERROR_SUCCESS) {
			std::cout << "Error: couldn't remove key from registry" << std::endl;
			RegCloseKey(hkey);
			return false;
		}

		std::cout << "Successfully disabled autostart!" << std::endl;
	}

	RegCloseKey(hkey);
	return true;
}