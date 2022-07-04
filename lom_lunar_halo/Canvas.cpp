#include "pch.h"
#include "Canvas.h"
#include <stack>

using std::stack;
using namespace YX::GUI;


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

			if (typeid(*n) == typeid(Canvas))
				dynamic_cast<Canvas*>(n.get())->Draw(batch);
			else if(typeid(*n) == typeid(Control)) {
				;
			}
			else {
				auto g = dynamic_cast<Graphic*>(n.get());
				batch->Draw(g->GetSprite()->SRV().Get(), g->GetWorldRect());
			}
		}
	);
}

void Canvas::Foreach(function<void(shared_ptr<GUIElement>)> func)
{
	stack<shared_ptr<GUIElement>> s;
	auto children = Children();
	for (auto i = children.begin(); i != children.end(); i++)
	{
		s.push(*i);
	}

	while (s.size() > 0)
	{
		auto n = s.top();
		s.pop();
		if (func)
			func(n);

		if (typeid(*n) == typeid(Canvas))
			;
		else
		{
			children = n->Children();
			for (auto i = children.begin(); i != children.end(); i++)
			{
				s.push(*i);
			}
		}
	}
}