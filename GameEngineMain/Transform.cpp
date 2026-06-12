#include "Transform.h"

DirectX::XMMATRIX Transform::GetWorldMatrix() const
{
    // 使用 DirectXMath 构建 TRS 矩阵
    DirectX::XMMATRIX matScale = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
    // 欧拉角转四元数旋转
    DirectX::XMVECTOR quat = DirectX::XMQuaternionRotationRollPitchYaw(
        DirectX::XMConvertToRadians(rotation.x),
        DirectX::XMConvertToRadians(rotation.y),
        DirectX::XMConvertToRadians(rotation.z));
    DirectX::XMMATRIX matRot = DirectX::XMMatrixRotationQuaternion(quat);
    DirectX::XMMATRIX matTrans = DirectX::XMMatrixTranslation(position.x, position.y, position.z);

    // 世界坐标公转：Scale -> Rotate -> Translate
    return matTrans * matRot * matScale;
}