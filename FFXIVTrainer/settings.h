#ifndef SETTINGS_H_
#define SETTINGS_H_

#include "windows.h"
#include <string>

/*
camera_pointer = 0x14E11B0
offset_current = 0x108
offset_max = 0x110
version = "2015.11.21.0000.0000"
*/

struct Settings {
	DWORD camera_pointer;
	DWORD offset_current;
	DWORD offset_max;
	std::string version;
};

const std::string settings_url = "https://raw.githubusercontent.com/datalate/ffxiv-camhack/master/ffxiv_memory.txt";


bool getSettings(const std::string& url, std::ostringstream& stream);

bool loadSettings(Settings& settings);

#endif