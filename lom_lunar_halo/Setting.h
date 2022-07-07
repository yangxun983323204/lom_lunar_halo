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
	wstring GetDataDir();
	wstring GetMapDir();
	wstring GetSoundDir();
	wstring GetUILayoutDir();
private:
	json _setting;
	wstring _originDataDir;
	wstring _uiLayoutDir;

	void Read();
	void ReadKey(string key, wstring& val, wstring defaultVal);
};

