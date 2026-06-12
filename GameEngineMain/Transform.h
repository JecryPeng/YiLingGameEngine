#pragma once
#include "Component.h"
#include <DirectXMath.h>

class Transform : public Component
{
public:
    DirectX::XMFLOAT3 position = { 0.0f, 0.0f, 0.0f };
    DirectX::XMFLOAT3 rotation = { 0.0f, 0.0f, 0.0f }; // 欧拉角（度）
    DirectX::XMFLOAT3 scale = { 1.0f, 1.0f, 1.0f };

    // 返回世界矩阵（模型矩阵），用于传入着色器
    DirectX::XMMATRIX GetWorldMatrix() const;
};