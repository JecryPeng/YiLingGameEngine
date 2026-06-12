#pragma once

// 前向声明，避免头文件循环依赖
class GameObject;

class Component
{
public:
    // 构造函数：组件刚被创建，还未挂载到任何物体时调用
    Component() : gameObject(nullptr), enabled(true) {}

    // 虚析构：确保派生类能正确释放资源
    virtual ~Component() = default;

    // ---------- 生命周期（模仿 Unity MonoBehaviour）----------
    // 挂载到 GameObject 后、第一帧 Update 前调用一次
    virtual void Awake() {}

    // 在第一帧 Update 之前调用，但晚于 Awake（可留作后续扩展）
    virtual void Start() {}

    // 每帧调用一次（逻辑更新，应使用引擎提供的 deltaTime）
    virtual void Update(float dt) {}

    // 每帧渲染时调用（绑定资源、发出 Draw 命令等）
    virtual void OnRender() {}

    // ---------- 辅助功能 ----------
    // 获取所属 GameObject（只读）
    GameObject* GetGameObject() const { return gameObject; }

    // 组件是否激活（未激活时，Update/OnRender 可能被跳过）
    bool IsEnabled() const { return enabled; }
    void SetEnabled(bool value) { enabled = value; }

    // 当组件被销毁前调用（可用于清理非图形资源，图形资源由析构处理）
    virtual void OnDestroy() {}

protected:
    // 组件只能通过 GameObject 来设置归属，因此设为 protected
    // 在 GameObject::AddComponent 中会调用 SetGameObject
    void SetGameObject(GameObject* go) { gameObject = go; }

    // 将 GameObject 设为友元，使其可以访问 SetGameObject
    friend class GameObject;

private:
    GameObject* gameObject;  // 指向所属游戏对象的指针
    bool enabled;            // 启用状态
};