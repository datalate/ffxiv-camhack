#ifndef FFXIV_H_
#define FFXIV_H_

#include "settings.h"

#include <string>
#include "windows.h"


static const char* window_name = "FINAL FANTASY XIV";
static const std::string process_name = "ffxiv_dx11.exe";

static const float zoom_max_default_ = 20;
static const float zoom_max_custom_ = 120;


class FFXIV
{
public:
	FFXIV(Settings& settings);
	~FFXIV();

	bool game_found();

	bool findGame();
	void checkValues();

private:
	Settings* settings_;

	HANDLE proc_handle_;
	DWORD_PTR base_address_;
	DWORD exitCode;

	DWORD address_zoom_current_;
	DWORD address_zoom_max_;

	float zoom_current_;
	float zoom_max_;

	bool game_found_;
	bool firstrun_;

	bool calculateAddresses();
	void checkGameStatus();

};

#endif