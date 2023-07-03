#pragma once
#include "json.hpp"
#include <string>

using json = nlohmann::json;
using std::wstring;
using std::string;

class Setting
{
public:
	bool Load();
	string GetRootDir();
	string GetDataDir();
	string GetMapDir();
	string GetSoundDir();
	string GetUILayoutDir();
private:
	json _setting;
	string _originDataDir;
	string _uiLayoutDir;

	void Read();
	void ReadKey(string key, string& val, string defaultVal);
};

