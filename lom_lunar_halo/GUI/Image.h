#pragma once
#include "GUIBase.h"

namespace YX {
	namespace GUI {
		class Image:public Graphic
		{
		public:
			void SetSprite(std::shared_ptr<Sprite> sprite);
			virtual void LoadXml(tinyxml2::XMLElement* e) override;
		};
	}
}

