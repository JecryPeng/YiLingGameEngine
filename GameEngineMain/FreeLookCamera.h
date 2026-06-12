#pragma once
#include "Component.h"

class FreeLookCamera : public Component
{
public:
    float moveSpeed = 5.0f;        // 移动速度（单位/秒）
    float lookSensitivity = 0.2f;  // 鼠标灵敏度

    void Update(float dt) override;
};