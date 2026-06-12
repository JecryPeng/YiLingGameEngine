#include "GameObject.h"
#include "Transform.h"

GameObject::GameObject()
{
    // Transform 是必需的，直接创建并保存快捷指针
    m_Transform = AddComponent<Transform>();
}

void GameObject::Destroy()
{
    for (auto c : m_Components)
        delete c;
    m_Components.clear();
    m_Transform = nullptr;
}