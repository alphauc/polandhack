#include "cfg.hh"

cfg g_config;

std::map< std::string, value > g_cfg;

void save(std::string name = "config.ini") {
	static char path[MAX_PATH];
	std::string folder, file;

	if (SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_APPDATA, NULL, 0, path))) {
		folder = std::string(path) + ("\\poland_hack\\");
		file = std::string(path) + ("\\poland_hack\\") + name;
	}

	CreateDirectory(folder.c_str(), NULL);

	for (auto &e : g_cfg) {
		WritePrivateProfileStringA("poland_hack", e.first.c_str(), std::to_string(e.second.get<double>()).c_str(), file.c_str());
	}
}

void load(std::string name = "config.ini") {
	static char path[MAX_PATH];
	std::string folder, file;

	if (SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_APPDATA, NULL, 0, path))) {
		folder = std::string(path) + ("\\poland_hack\\");
		file = std::string(path) + ("\\poland_hack\\") + name;
	}

	CreateDirectory(folder.c_str(), NULL);

	for (auto &e : g_cfg) {
		char value[64] = { '\0' };

		GetPrivateProfileStringA("poland_hack", e.first.c_str(), "", value, 64, file.c_str());

		e.second.set<double>(atof(value));
	}
}
