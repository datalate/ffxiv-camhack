#pragma once

#include "windows.h"
#include <string>

struct Settings {
	DWORD camera_pointer;
	DWORD offset_current;
	DWORD offset_max;
	std::string version;
};

const std::string settings_url = "http://miemala.com/ffxiv_memory.txt";


bool getSettings(const std::string& url, std::ostringstream& stream);

bool loadSettings(Settings& settings);
