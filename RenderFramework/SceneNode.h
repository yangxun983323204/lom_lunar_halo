#pragma once
#include <memory>
#include <vector>
#include <string>
#include <DirectXMath.h>
#include "ISceneNodeComponent.h"

#define SCENE_NODE_SCALE

using std::unique_ptr;
using std::shared_ptr;
using std::weak_ptr;
using std::vector;

class SceneNode:
	public std::enable_shared_from_this<SceneNode>
{
public:
	std::wstring Name;
	SceneNode();
	inline DirectX::XMINT2 GetLocalPosition() { return _localPosition; }
	inline void SetLocalPosition(DirectX::XMINT2 lPos) { _localPosition = lPos; }
	DirectX::XMINT2 GetWorldPosition();

	inline float GetLocalAngle() { return _localAngle; }
	inline void SetLocalAngle(float lAngle) { _localAngle = lAngle; }
	float GetWorldAngle();

#ifdef SCENE_NODE_SCALE
	inline DirectX::XMFLOAT2 GetLocalScale() { return _localScale; }
	inline void SetLocalScale(DirectX::XMFLOAT2 lScale) { _localScale = lScale; }
	DirectX::XMFLOAT2 GetWorldScale();
#endif

	void SetParent(weak_ptr<SceneNode> parent);
	weak_ptr<SceneNode> GetParent();
	vector<weak_ptr<SceneNode>> GetChildren();
	template<typename TComponent>
	weak_ptr<TComponent> AddComponent();
	weak_ptr<ISceneNodeComponent> GetComponent(int typeId);
	vector<weak_ptr<ISceneNodeComponent>> GetComponents(int typeId);
	vector<weak_ptr<ISceneNodeComponent>> GetComponentsInChildren(int typeId);
	void RemoveComponent(weak_ptr<ISceneNodeComponent> component);

private:
	DirectX::XMINT2 _localPosition;
	float _localAngle;
#ifdef SCENE_NODE_SCALE
	DirectX::XMFLOAT2 _localScale;
#endif
	weak_ptr<SceneNode> _parent;
	vector<shared_ptr<SceneNode>> _children;
	vector<shared_ptr<ISceneNodeComponent>> _components;
};

template<class TComponent>
weak_ptr<TComponent> SceneNode::AddComponent()
{
	auto ptr = std::make_shared<TComponent>();
	ptr->Reg();
	ptr->_node = weak_from_this();
	_components.push_back(ptr);
	return std::weak_ptr<TComponent>{ptr};
}