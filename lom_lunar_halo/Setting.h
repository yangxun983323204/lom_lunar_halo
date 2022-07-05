#pragma once
#include "json.hpp"
#include <string>

using json = nlohmann::json;
using std::wstring;

class Setting
{
public:
	bool Load();
	wstring GetDataDir();
private:
	json _setting;
};

