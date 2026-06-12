#include "Camera.h"
#include "Transform.h"
#include "GameObject.h"

DirectX::XMMATRIX Camera::GetViewMatrix() const
{
    Transform* trans = GetGameObject()->GetTransform();
    DirectX::XMVECTOR eye = DirectX::XMLoadFloat3(&trans->position);
    DirectX::XMVECTOR up = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

    // 标准 FPS 摄像机：先 Yaw(rotation.y) 后 Pitch(rotation.x)
    // 初始前方向 = (0,0,1)，上方向 = (0,1,0)
    DirectX::XMVECTOR forward = DirectX::XMVector3TransformNormal(
        DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f),
        DirectX::XMMatrixRotationRollPitchYaw(
            DirectX::XMConvertToRadians(trans->rotation.x),   // Pitch
            DirectX::XMConvertToRadians(trans->rotation.y),   // Yaw
            0.0f                                              // Roll
        )
    );
    return DirectX::XMMatrixLookAtLH(eye, DirectX::XMVectorAdd(eye, forward), up);
}

DirectX::XMMATRIX Camera::GetProjectionMatrix() const
{
    return DirectX::XMMatrixPerspectiveFovLH(
        DirectX::XMConvertToRadians(fovY),
        aspectRatio,
        nearPlane,
        farPlane
    );
}

DirectX::XMMATRIX Camera::GetViewProjection() const
{
    return GetViewMatrix() * GetProjectionMatrix();
}

void Camera::UpdateAspectRatio(float width, float height)
{
    if (height > 0) aspectRatio = width / height;
}