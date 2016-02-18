#ifndef CONFIG_H_
#define CONFIG_H_

#include <string>

struct Config {
	bool autostart;
	bool hide_window;

	int max_zoom;

	bool toggle;
	unsigned int toggle_key;
	unsigned int toggle_modifier;
};

static const std::string config_file = "ffxiv.cfg";


void loadDefaultConfig(Config& cfg);

bool loadConfig(Config& cfg);

#endif