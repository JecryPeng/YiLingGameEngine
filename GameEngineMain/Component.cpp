#include "Component.h"
class Component {
public:
    GameObject* gameObject;
    virtual void Awake() {}
    virtual void Update(float dt) {}
    virtual void OnRender() {}
};