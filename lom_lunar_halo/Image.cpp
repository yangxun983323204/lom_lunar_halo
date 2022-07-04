#include "pch.h"
#include "Image.h"

using namespace YX;
using namespace YX::GUI;

void Image::SetSprite(std::shared_ptr<Sprite> sprite)
{
	_sprite = sprite;
}