#include "SceneNode.h"
#include <algorithm>

DirectX::XMFLOAT2 SceneNode::GetWorldPosition()
{
    if(_parent.expired())
        return _localPosition;
    else 
    {
        auto p = _parent.lock();
        auto pWPos = p->GetWorldPosition();
#ifdef SCENE_NODE_SCALE
        auto pWScale = p->GetWorldScale();
        return DirectX::XMFLOAT2(pWPos.x + _localPosition.x*pWScale.x, pWPos.y + _localPosition.y*pWScale.y);
#else
        return DirectX::XMFLOAT2(pWPos.x + _localPosition.x, pWPos.y + _localPosition.y);
#endif
    }
}

float SceneNode::GetWorldAngle()
{
    if (_parent.expired())
        return _localAngle;
    else {
        auto p = _parent.lock();
        auto pWAngle = p->GetWorldAngle();
        return pWAngle + _localAngle;
    }
}

#ifdef SCENE_NODE_SCALE
DirectX::XMFLOAT2 SceneNode::GetWorldScale()
{
    if (_parent.expired())
        return _localScale;
    else {
        auto p = _parent.lock();
        auto pWScale = p->GetWorldScale();
        return DirectX::XMFLOAT2(pWScale.x + _localScale.x, pWScale.y + _localScale.y);
    }
}
#endif

void SceneNode::SetParent(weak_ptr<SceneNode> parent)
{
    if (!_parent.expired())
    {
        auto p = _parent.lock();
        auto pChildren = p->_children;
        auto f = find_if(pChildren.begin(), pChildren.end(), [this](shared_ptr<SceneNode> n) { return n.get() == this; });
        if(f!=pChildren.end())
            pChildren.erase(f);
    }
    _parent = parent;
    if (!_parent.expired())
    {
        auto p = _parent.lock();
        auto pChildren = p->_children;
        pChildren.push_back(shared_from_this());
    }
}

weak_ptr<SceneNode> SceneNode::GetParent()
{
    return _parent;
}

vector<weak_ptr<SceneNode>> SceneNode::GetChildren()
{
    vector<weak_ptr<SceneNode>> ret;
    for (auto i : _children) 
    {
        ret.push_back(weak_ptr<SceneNode>(i));
    }

    return ret;
}

void SceneNode::AddComponent(weak_ptr<ISceneNodeComponent> component)
{
    if (component.expired())
        return;

    auto ptr = component.lock();
    ptr->Reg();
    ptr->_node = weak_from_this();
    _components.push_back(ptr);
}

weak_ptr<ISceneNodeComponent> SceneNode::GetComponent(int typeId)
{
    for(auto i : _components) 
    {
        if (i->GetTypeId() == typeId)
            return weak_ptr<ISceneNodeComponent>(i);
    }

    return {};
}

vector<weak_ptr<ISceneNodeComponent>> SceneNode::GetComponents(int typeId)
{
    vector<weak_ptr<ISceneNodeComponent>> ret;
    for (auto i : _components)
    {
        if (i->GetTypeId() == typeId)
            ret.push_back(weak_ptr<ISceneNodeComponent>(i));
    }

    return ret;
}

vector<weak_ptr<ISceneNodeComponent>> SceneNode::GetComponentsInChildren(int typeId)
{
    return vector<weak_ptr<ISceneNodeComponent>>();
}

void SceneNode::RemoveComponent(weak_ptr<ISceneNodeComponent> component)
{
    if (component.expired())
        return;

    auto ptr = component.lock();
    auto iter = std::find_if(_components.begin(), _components.end(), [&ptr](shared_ptr<ISceneNodeComponent> n) { return n.get() == ptr.get(); });
    if (iter != _components.end())
    {
        auto last = _components.end();
        last--;
        std::swap(*iter, *last);
        _components.pop_back();
        ptr->_node = {};
        ptr->UnReg();
    }
}
