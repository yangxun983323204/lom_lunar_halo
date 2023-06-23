#pragma once
#include <string>
#include <memory>
#include <vector>
#include "Sprite_old.h"
#include "../tinyxml2.h"

namespace YX {
	namespace GUI {
		class GUIElement
		{
		public:
			std::wstring Id;
			bool Interactable;
			bool SelfVisiable;
			bool SelfActive;
			int Width;
			int Height;
			DirectX::XMINT2 LocalPos;
			DirectX::XMFLOAT2 Pivot;
			
			inline GUIElement():
				Id{},
				Interactable{ true },
				SelfVisiable{ true },
				SelfActive{ true },
				Width{4},
				Height{4},
				LocalPos{0,0},
				Pivot{0,0},
				_delayFill{false}
			{
			};
			inline bool IsRoot() { return false; }
			bool IsVisiable();
			bool IsActive();
			virtual bool Drawable() = 0;
			inline void SetSize(int w, int h) { Width = w; Height = h; }
			inline void SetPivot(float px, float py) { Pivot.x = px; Pivot.y = py; }
			inline void SetLocalPos(int x, int y) { LocalPos.x = x; LocalPos.y = y; }
			inline std::weak_ptr<GUIElement> Parent() { return _parent; }
			inline std::vector<std::shared_ptr<GUIElement>> Children() { return _children; }

			virtual void Update(float deltaSec) = 0;
			inline virtual void Draw(){}
			void SetParent(std::shared_ptr<GUIElement> self,std::shared_ptr<GUIElement> parent);
			DirectX::XMINT2 GetWorldPos();
			RECT GetLocalRect();
			RECT GetScreenRect();
			bool FillParent();
			virtual bool HitTest(int sx, int sy);
			virtual void LoadXml(tinyxml2::XMLElement* e);
		protected:
			bool _delayFill;
			std::weak_ptr<GUIElement> _parent;
			std::vector<std::shared_ptr<GUIElement>> _children;
		};

		class Graphic :public GUIElement
		{
		public:
			inline virtual bool Drawable() override { return true; }
			inline virtual void Update(float deltaSec) override {}
			inline Sprite_old* GetSprite() { return _sprite.get(); }
			virtual void Draw() override;
		protected:
			std::shared_ptr<Sprite_old> _sprite;
		};

		class Control : public GUIElement
		{
		public:
			inline virtual bool Drawable() override { return false; }
			inline virtual void Update(float deltaSec) override {}
		};
	}
}