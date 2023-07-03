#include "pch.h"
#include "Setting.h"
#include <fstream>
#include <streambuf>
#include "StringCodec.hpp"

string KEY_ORIGIN_DATA = "OriginDataDir";
string KEY_UILAYOUT = "UILayoutDir";

bool Setting::Load()
{
    struct _stat buffer;
    if (_wstat(L"config.json", &buffer) == 0)
    {
        std::ifstream cfg(L"config.json");
        std::string str = std::string((std::istreambuf_iterator<char>(cfg)),
            std::istreambuf_iterator<char>());

        _setting = json::parse(str);
        cfg.close();
        Read();
        return true;
    }

    return false;
}

string Setting::GetRootDir()
{
    return _originDataDir;
}

string Setting::GetDataDir()
{
    return _originDataDir + "Data/";
}

string Setting::GetMapDir()
{
    return _originDataDir + "Map/";
}

string Setting::GetSoundDir()
{
    return _originDataDir + "Sound/";
}

string Setting::GetUILayoutDir()
{
    return _uiLayoutDir;
}

void Setting::Read()
{
    ReadKey(KEY_ORIGIN_DATA, _originDataDir, "./");
    ReadKey(KEY_UILAYOUT, _uiLayoutDir, "./UILayout/");
}

void Setting::ReadKey(string key, string& val, string defaultVal)
{
    if (_setting.contains(key))
    {
        val = _setting.at(key);
    }
    else {
        val = defaultVal;
    }
}