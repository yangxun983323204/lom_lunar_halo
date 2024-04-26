#pragma once
#include <cstdint>
#include <memory>
#include <map>
#include <vector>
#include <list>
#include <set>
#include <string>
#include "ISystem.h"

using std::string;
using std::weak_ptr;
using std::vector;
using std::list;

class SceneNode;

namespace RenderFramework
{
	namespace Meta {
		constexpr size_t Hash(const char* s)
		{
			size_t v = 0;
			for (; *s; ++s)
			{
				v = v*5 + *s;
			}
			return v;
		};
	}
}
//============================================================================================
#define _ID_OF(T) RenderFramework::Meta::Hash(#T)
//============================================================================================
#define _SCENE_NODE_COMPONENT_ID(T)														\
static const uint32_t TypeId = _ID_OF(T);												\
virtual const uint32_t GetTypeId()														\
{return _ID_OF(T);}																		\
//============================================================================================

class ISceneNodeComponent
{
public:
	_SCENE_NODE_COMPONENT_ID(ISceneNodeComponent)
	bool Enable = true;
	inline SceneNode* GetSceneNode() { return _node.expired() ? nullptr : _node.lock().get(); }
	inline weak_ptr<SceneNode> GetSceneNodeWeakPtr() { return _node; }
	template<typename TComponent>
	TComponent* As();
	virtual void Tick(uint64_t totalMs, uint32_t deltaMs);

	bool bEnableTick = false;
protected:
	friend class SceneNode;
	friend class ISystem;

	static list<ISceneNodeComponent*> GetComponentsByType(int typeId);

	weak_ptr<SceneNode> _node;

	void CallReg();
	void CallUnReg();
	/// <summary>
	/// 提交类的类型信息
	/// </summary>
	/// <param name="set"></param>
	virtual void FillTypeIds(std::vector<uint32_t>& typeIds);

	void RegWithId(uint32_t id);
	void UnRegWithId(uint32_t id);

	static std::map<int, list<ISceneNodeComponent*>> __componentsReg;
};

template<typename TComponent>
TComponent* ISceneNodeComponent::As()
{
	return static_cast<TComponent*>(this);
}