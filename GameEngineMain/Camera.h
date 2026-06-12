#pragma once
#include "Component.h"
#include <DirectXMath.h>

class Camera : public Component
{
public:
    float fovY = 60.0f;
    float nearPlane = 0.1f;
    float farPlane = 100.0f;
    float aspectRatio = 16.0f / 9.0f;

    DirectX::XMFLOAT3 lookAtTarget = { 0.0f, 0.0f, 0.0f };
    bool useLookAtTarget = true;   // 启用后忽略 Transform 旋转，直接看向目标

    DirectX::XMMATRIX GetViewMatrix() const;
    DirectX::XMMATRIX GetProjectionMatrix() const;
    DirectX::XMMATRIX GetViewProjection() const;
    void UpdateAspectRatio(float width, float height);
};