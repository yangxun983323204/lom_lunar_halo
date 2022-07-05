#include "pch.h"
#include "Setting.h"
#include <fstream>
#include <streambuf>
#include "StringCodec.hpp"

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
        return true;
    }

    return false;
}

wstring Setting::GetDataDir()
{
    if (_setting.contains("DataDir"))
    {
        std::string s = _setting.at("DataDir");
        return YX::Utf8ToWString(s);
    }
    else {
        return L"./Data/";
    }
}