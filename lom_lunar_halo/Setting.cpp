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

wstring Setting::GetDataDir()
{
    return _originDataDir + L"./Data/";
}

wstring Setting::GetMapDir()
{
    return _originDataDir + L"./Map/";
}

wstring Setting::GetSoundDir()
{
    return _originDataDir + L"./Sound/";
}

wstring Setting::GetUILayoutDir()
{
    return _uiLayoutDir;
}

void Setting::Read()
{
    ReadKey(KEY_ORIGIN_DATA, _originDataDir, L"./");
    ReadKey(KEY_UILAYOUT, _uiLayoutDir, L"./UILayout/");
}

void Setting::ReadKey(string key, wstring& val, wstring defaultVal)
{
    if (_setting.contains(key))
    {
        std::string s = _setting.at(key);
        val = YX::Utf8ToWString(s);
    }
    else {
        val = defaultVal;
    }
}