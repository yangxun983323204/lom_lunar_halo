#pragma once
#include <memory>
#include <vector>
#include "Sprite.h"

namespace YX {
	namespace GUI {
		class GUIElement
		{
		public:
			bool SelfVisiable;
			bool SelfActive;
			int Width;
			int Height;
			DirectX::XMINT2 LocalPos;
			DirectX::XMFLOAT2 Pivot;
			
			inline GUIElement():
				SelfVisiable{ true },
				SelfActive{ true },
				Width{4},
				Height{4},
				LocalPos{0,0},
				Pivot{0,0}
			{
			};

			bool IsVisiable();
			bool IsActive();
			inline void SetSize(int w, int h) { Width = w; Height = h; }
			inline void SetPivot(float px, float py) { Pivot.x = px; Pivot.y = py; }
			inline void SetLocalPos(int x, int y) { LocalPos.x = x; LocalPos.y = y; }
			inline std::weak_ptr<GUIElement> Parent() { return _parent; }
			inline std::vector<std::shared_ptr<GUIElement>> Children() { return _children; }

			virtual void Update(float deltaSec) = 0;
			void SetParent(std::shared_ptr<GUIElement> e);
			DirectX::XMINT2 GetWorldPos();
			RECT GetLocalRect();
			RECT GetWorldRect();
			bool FillParent();
		protected:
			
			std::weak_ptr<GUIElement> _parent;
			std::vector<std::shared_ptr<GUIElement>> _children;
		};

		class Graphic :public GUIElement
		{
		public:
			inline void Update(float deltaSec) override {}
			inline Sprite* GetSprite() { return _sprite.get(); }
		protected:
			std::shared_ptr<Sprite> _sprite;
		};

		class Control : public GUIElement
		{
		public:
			inline void Update(float deltaSec) override {}
		};
	}
}