#include "pch.h"
#include "Image.h"
#include <string>
#include "Setting.h"
#include "DeviceResources.h"
#include "../StringCodec.hpp"

using namespace YX;
using namespace YX::GUI;
using std::string;

extern DX::DeviceResources* GetDeviceResource();
extern std::shared_ptr<Setting> GetSetting();

void Image::SetSprite(std::shared_ptr<Sprite_old> sprite)
{
	_sprite = sprite;
}

void Image::LoadXml(tinyxml2::XMLElement* e)
{
	Graphic::LoadXml(e);
	char* pbuffer;
	e->QueryStringAttribute("Type", (const char**)&pbuffer);
	if (strcmp(pbuffer,"dynamic")==0)
	{

	}
	else if (strcmp(pbuffer,"wil")==0)
	{
		pbuffer[0] = '\0';
		if (e->QueryStringAttribute("File", (const char**)&pbuffer) == tinyxml2::XML_SUCCESS)
		{
			int idx = 0;
			if (e->QueryIntAttribute("Index",&idx) == tinyxml2::XML_SUCCESS)
			{
				// todo 仅临时写法
				ImageLib imgLib{};
				imgLib.Open(_W(GetSetting()->GetDataDir() + pbuffer));
				//assert(imgLib.IsOpened());
				if (imgLib.IsOpened())
				{
					int i = idx;
					auto info = imgLib.GetImageInfo(i);
					auto rgba32 = imgLib.GetImageRGBA32(i);
					imgLib.Close();
					auto sprite = YX::Sprite_old::CreateFromWIL(GetDeviceResource()->GetD3DDevice(), info, std::move(rgba32));
					SetSprite(sprite);
					bool UseNativeSize = e->BoolAttribute("UseNativeSize", false);
					if (UseNativeSize)
					{
						this->SetSize(info.Width, info.Height);
					}
				}
			}
		}
	}
	else {
		printf("不支持的图片类型:%s", pbuffer);
	}
}