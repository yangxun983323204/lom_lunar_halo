#include "pch.h"
#include "Canvas.h"
#include <stack>

using std::stack;
using std::vector;
using namespace YX::GUI;

std::vector<Canvas*> Canvas::_canvasList{};

Canvas::Canvas():
	RenderOrder{0}
{
	_canvasList.push_back(this);
}

Canvas::~Canvas()
{
	std::remove_if(_canvasList.begin(), _canvasList.end(), [this](Canvas* n)
		{
			return n == this;
		}
	);
}

void Canvas::Update(float deltaSec)
{
	Foreach([deltaSec](shared_ptr<GUIElement> n)
		{
			if (!n->IsActive())
				return;

			n->Update(deltaSec);
		}
	);
}

void Canvas::Draw(SpriteBatch* batch)
{
	Foreach([batch](shared_ptr<GUIElement> n) 
		{
			if (!n->IsVisiable())
				return;

			if(!n->Drawable()) {
				return;
			}
			else {
				n->Draw(batch);
			}
		}
	);
}

void Canvas::Foreach(function<void(shared_ptr<GUIElement>)> func)
{
	stack<shared_ptr<GUIElement>> s;
	auto children = Children();
	for (auto i = children.rbegin(); i != children.rend(); i++)
	{
		s.push(*i);
	}

	while (s.size() > 0)
	{
		auto n = s.top();
		s.pop();
		if (func)
			func(n);

		if (n->IsRoot())
			continue;// 子canvas不由父canvas驱动更新
		else
		{
			children = n->Children();
			for (auto i = children.rbegin(); i != children.rend(); i++)
			{
				s.push(*i);
			}
		}
	}
}

void Canvas::UpdateAll(float deltaSec)
{
	for (auto canvas : _canvasList)
	{
		canvas->Update(deltaSec);
	}
}

void Canvas::DrawAll(SpriteBatch* batch)
{
	for (auto canvas : _canvasList)
	{
		canvas->Draw(batch);
	}
}