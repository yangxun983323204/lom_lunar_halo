#include "pch.h"
#include "LayoutLoader.h"
#include <queue>
#include "../StringCodec.hpp"

using std::queue;
using namespace YX::GUI;

const string TYPE_CANVAS = "Canvas";
const string TYPE_IMAGE = "Image";
const string TYPE_TEXT = "Text";

shared_ptr<Canvas> LayoutLoader::Parse(wstring xmlPath)
{
	tinyxml2::XMLDocument xml;
	auto path = WStringToUtf8(xmlPath);
	xml.LoadFile(path.c_str());
	auto root = xml.FirstChildElement();
	shared_ptr<Canvas> canvas{};
	// 先根遍历
	queue<shared_ptr<GUIElement>> parentQueue{};
	queue<tinyxml2::XMLElement*> xmlQueue{};
	xmlQueue.push(root);
	while (xmlQueue.size()>0)
	{
		auto node = xmlQueue.front();
		xmlQueue.pop();
		shared_ptr<GUIElement> parent{};
		shared_ptr<GUIElement> curr{};
		if (parentQueue.size()>0)
		{
			parent = parentQueue.front();
			parentQueue.pop();
		}

		auto name = node->Name();
		if (name == TYPE_CANVAS)
		{
			if (canvas)
				throw "不允许单个布局文件中包含多个Canvas";

			canvas = std::make_shared<Canvas>();
			curr = canvas;
		}
		else if(name == TYPE_IMAGE) {
			curr = std::make_shared<Image>();
		}
		else if (name == TYPE_TEXT) {
			curr = std::make_shared<Text>();
		}
		else {
			printf("不支持的UI元素类型:%s", name);
		}

		if(curr)
			curr->LoadXml(node);

		if (curr && parent)
			curr->SetParent(curr, parent);

		// 将其子节点加入队列
		if (node->FirstChildElement())
		{
			auto c = node->FirstChildElement();
			do
			{
				xmlQueue.push(c);
				parentQueue.push(curr);
				if (!c->NextSiblingElement())
					break;

				c = c->NextSiblingElement();
			} while (true);
		}
	}

	return canvas;
}