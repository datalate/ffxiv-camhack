#include "config.h"

#include <libconfig.h++>
#include <iostream>


const std::string config_file = "ffxiv.cfg";

void loadDefaultConfig(Config& cfg) {
	cfg.autostart = false;
	cfg.hide_window = false;
	cfg.debug = false;

	cfg.max_zoom = 100;
	
	cfg.toggle = false;
	cfg.toggle_key = 42;
	cfg.toggle_modifier = 1;
}

bool loadConfig(Config& cfg) {
	loadDefaultConfig(cfg);

	libconfig::Config cfg_file;

	try {
		cfg_file.readFile(config_file.c_str());
	}
	catch (const libconfig::FileIOException &fioex) {
		std::cout << "Unable to load config file: " << config_file << std::endl;
		std::cout << "Using default configuration" << std::endl;
		return false;
	}
	catch (const libconfig::ParseException &pex) {
		std::cout << "Unable to parse config file: " << config_file << std::endl;
		std::cout << "Using default configuration" << std::endl;
		return false;
	}

	if (cfg_file.lookupValue("autostart", cfg.autostart)
		&& cfg_file.lookupValue("hide_window", cfg.hide_window)
		&& cfg_file.lookupValue("debug", cfg.debug)
		&& cfg_file.lookupValue("max_zoom", cfg.max_zoom)
		&& cfg_file.lookupValue("toggle", cfg.toggle)
		&& cfg_file.lookupValue("toggle_key", cfg.toggle_key)
		&& cfg_file.lookupValue("toggle_modifier", cfg.toggle_modifier)) {

		std::cout << "All settings loaded successfully from " << config_file << std::endl;

		return true;
	}

	std::cout << "Couldn't load all settings from " << config_file << std::endl;
	std::cout << "Using default values for them" << std::endl;

	return false;
}