#include "settings.h"

#include <curl/curl.h>
#include <string>
#include <iostream>
#include <sstream>


size_t write_data(char *ptr, size_t size, size_t nmemb, void *userdata) {
	std::ostringstream *stream = (std::ostringstream*)userdata;
	size_t count = size * nmemb;
	stream->write(ptr, count);
	return count;
}

bool getSettings(const std::string& url, std::ostringstream& stream) {
	CURL *curl;
	CURLcode res;

	curl_global_init(CURL_GLOBAL_DEFAULT);

	curl = curl_easy_init();

	if (!curl) {
		std::cout << "Error: unable to load CURL" << std::endl;
		return false;
	}

	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &stream);

	res = curl_easy_perform(curl);
	if (res != CURLE_OK) {
		std::cout << "Error: unable to load memory addresses from " << url << std::endl;
		std::cout << "Try restarting the program..." << std::endl;

		curl_easy_cleanup(curl);
		curl_global_cleanup();

		return false;
	}

	curl_easy_cleanup(curl);
	curl_global_cleanup();

	return true;
}

bool loadSettings(Settings& settings) {
	std::ostringstream stream;

	settings.camera_pointer = 0x0;
	settings.offset_current = 0x0;
	settings.offset_max = 0x0;
	settings.version = "";

	if (!getSettings(settings_url, stream)) {
		return false;
	}


	std::string::size_type found;
	std::string line;
	std::istringstream iss(stream.str());
	while (getline(iss, line)) {
		std::stringstream ss;
		found = std::string::npos;

		found = line.find("pointer:");
		if (found != std::string::npos) {
			ss << line.erase(0, std::string("pointer:").length());
			ss >> std::hex >> settings.camera_pointer;
			continue;
		}

		found = line.find("offset_current:");
		if (found != std::string::npos) {
			ss << line.erase(0, std::string("offset_current:").length());
			ss >> std::hex >> settings.offset_current;
			continue;
		}

		found = line.find("offset_max:");
		if (found != std::string::npos) {
			ss << line.erase(0, std::string("offset_max:").length());
			ss >> std::hex >> settings.offset_max;
			continue;
		}

		found = line.find("version:");
		if (found != std::string::npos) {
			settings.version = line.erase(0, std::string("version:").length());
			continue;
		}
	}

	if ((settings.camera_pointer == 0) || (settings.offset_current == 0) ||
		(settings.offset_max == 0) || (settings.version == "")) {

		std::cout << "Error: couldn't parse memory addresses" << std::endl;
		std::cout << "Try restarting the program..." << std::endl;
		return false;
	}

	return true;
}