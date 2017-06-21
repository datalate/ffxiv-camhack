#ifndef CONFIG_H_
#define CONFIG_H_

#include <string>

struct Config {
	bool autostart;
	bool hide_window;
	bool debug;

	int max_zoom;

	bool toggle;
	unsigned int toggle_key;
	unsigned int toggle_modifier;
};


void loadDefaultConfig(Config& cfg);

bool loadConfig(Config& cfg);

#endif