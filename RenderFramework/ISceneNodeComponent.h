#pragma once
#include <cstdint>
#include <memory>
#include <map>
#include <vector>
#include <list>
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
	bool Enable;
	inline SceneNode* GetSceneNode() { return _node.expired() ? nullptr : _node.lock().get(); }
	inline weak_ptr<SceneNode> GetSceneNodeWeakPtr() { return _node; }
	template<typename TComponent>
	TComponent* As();
protected:
	friend class SceneNode;
	friend class ISystem;

	static list<ISceneNodeComponent*> GetComponentsByType(int typeId);

	weak_ptr<SceneNode> _node;

	inline void CallReg() { Reg(); };
	inline void CallUnReg() { UnReg(); };

	virtual void Reg();
	virtual void UnReg();
	void RegWithId(uint32_t id);
	void UnRegWithId(uint32_t id);

	static std::map<int, list<ISceneNodeComponent*>> __componentsReg;
};

template<typename TComponent>
TComponent* ISceneNodeComponent::As()
{
	return dynamic_cast<TComponent*>(this);
}

//============================================================================================
#define _SCENE_NODE_COMPONENT_BEGIN0(T)													\
class T:public ISceneNodeComponent														\
{																						\
public:																					\
	_SCENE_NODE_COMPONENT_ID(T)															\
protected:																				\
	virtual void Reg()																	\
	{																					\
		ISceneNodeComponent::RegWithId(_ID_OF(T));										\
	}																					\
	virtual void UnReg()																\
	{																					\
		ISceneNodeComponent::UnRegWithId(_ID_OF(T));									\
	}																					\
//============================================================================================
#define _SCENE_NODE_COMPONENT_BEGIN1(T, PARENT1)										\
class T:public ISceneNodeComponent, public PARENT1										\
{																						\
public:																					\
	_SCENE_NODE_COMPONENT_ID(T)															\
protected:																				\
	virtual void Reg()																	\
	{																					\
		ISceneNodeComponent::RegWithId(_ID_OF(PARENT1));								\
		ISceneNodeComponent::RegWithId(_ID_OF(T));										\
	}																					\
	virtual void UnReg()																\
	{																					\
		ISceneNodeComponent::UnRegWithId(_ID_OF(T));									\
		ISceneNodeComponent::UnRegWithId(_ID_OF(PARENT1));								\
	}																					\
//============================================================================================
#define _SCENE_NODE_COMPONENT_BEGIN2(T, PARENT1, PARENT2)								\
class T:public ISceneNodeComponent, public PARENT1, public PARENT2 						\
{																						\
public:																					\
	_SCENE_NODE_COMPONENT_ID(T)															\
protected:																				\
	virtual void Reg()																	\
	{																					\
		ISceneNodeComponent::RegWithId(_ID_OF(PARENT1));								\
		ISceneNodeComponent::RegWithId(_ID_OF(PARENT2));								\
		ISceneNodeComponent::RegWithId(_ID_OF(T));										\
	}																					\
	virtual void UnReg()																\
	{																					\
		ISceneNodeComponent::UnRegWithId(_ID_OF(T));									\
		ISceneNodeComponent::UnRegWithId(_ID_OF(PARENT2));								\
		ISceneNodeComponent::UnRegWithId(_ID_OF(PARENT1));								\
	}																					\
//============================================================================================
#define _SCENE_NODE_COMPONENT_BEGIN3(T, PARENT1, PARENT2, PARENT3)						\
class T:public ISceneNodeComponent, public PARENT1, public PARENT2, public PARENT3		\
{																						\
public:																					\
	_SCENE_NODE_COMPONENT_ID(T)															\
protected:																				\
	virtual void Reg()																	\
	{																					\
		ISceneNodeComponent::RegWithId(_ID_OF(PARENT1));								\
		ISceneNodeComponent::RegWithId(_ID_OF(PARENT2));								\
		ISceneNodeComponent::RegWithId(_ID_OF(PARENT3));								\
		ISceneNodeComponent::RegWithId(_ID_OF(T));										\
	}																					\
	virtual void UnReg()																\
	{																					\
		ISceneNodeComponent::UnRegWithId(_ID_OF(T));									\
		ISceneNodeComponent::UnRegWithId(_ID_OF(PARENT1));								\
		ISceneNodeComponent::UnRegWithId(_ID_OF(PARENT2));								\
		ISceneNodeComponent::UnRegWithId(_ID_OF(PARENT3));								\
	}																					\
//============================================================================================


#define _SCENE_NODE_COMPONENT_END };