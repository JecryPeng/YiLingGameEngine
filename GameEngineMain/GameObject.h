#pragma once
#include <string>
#include <vector>
#include "Component.h"

class Transform; // 前向声明

class GameObject
{
public:
    std::wstring name = L"GameObject";
    bool active = true;

    // 构造函数自动添加 Transform（核心组件）
    GameObject();

    // 禁止拷贝（每个对象唯一身份）
    GameObject(const GameObject&) = delete;
    GameObject& operator=(const GameObject&) = delete;

    // 添加组件（模板工厂）
    template<typename T>
    T* AddComponent()
    {
        static_assert(std::is_base_of<Component, T>::value, "T must derive from Component");
        T* component = new T();
        component->gameObject = this;
        m_Components.push_back(component);
        return component;
    }

    // 获取第一个指定类型的组件
    template<typename T>
    T* GetComponent()
    {
        for (auto c : m_Components)
        {
            T* casted = dynamic_cast<T*>(c);
            if (casted) return casted;
        }
        return nullptr;
    }

    // 获取 Transform（快捷）
    Transform* GetTransform() const { return m_Transform; }

    // 供引擎遍历所有组件
    const std::vector<Component*>& GetAllComponents() const { return m_Components; }

    // 清理所有组件
    void Destroy();

private:
    std::vector<Component*> m_Components;
    Transform* m_Transform; // 快速引用，避免每次 dynamic_cast
};