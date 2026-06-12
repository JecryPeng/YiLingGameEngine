#include "FreeLookCamera.h"
#include "Input.h"
#include "Transform.h"
#include "GameObject.h"
#include <DirectXMath.h>

void FreeLookCamera::Update(float dt)
{
    Transform* trans = GetGameObject()->GetTransform();
    if (!trans) return;

    // 获取当前前方向和右方向
    DirectX::XMVECTOR forward = DirectX::XMVector3TransformNormal(
        DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f),
        DirectX::XMMatrixRotationRollPitchYaw(
            DirectX::XMConvertToRadians(trans->rotation.x),
            DirectX::XMConvertToRadians(trans->rotation.y),
            0.0f
        )
    );
    DirectX::XMVECTOR right = DirectX::XMVector3Normalize(
        DirectX::XMVector3Cross(DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), forward)
    );

    // 移动
    float speed = moveSpeed * dt;
    if (Input::GetKey(KeyCode::W))
    {
        trans->position.x += DirectX::XMVectorGetX(forward) * speed;
        trans->position.y += DirectX::XMVectorGetY(forward) * speed;
        trans->position.z += DirectX::XMVectorGetZ(forward) * speed;
    }
    if (Input::GetKey(KeyCode::S))
    {
        trans->position.x -= DirectX::XMVectorGetX(forward) * speed;
        trans->position.y -= DirectX::XMVectorGetY(forward) * speed;
        trans->position.z -= DirectX::XMVectorGetZ(forward) * speed;
    }
    if (Input::GetKey(KeyCode::D))
    {
        trans->position.x += DirectX::XMVectorGetX(right) * speed;
        trans->position.y += DirectX::XMVectorGetY(right) * speed;
        trans->position.z += DirectX::XMVectorGetZ(right) * speed;
    }
    if (Input::GetKey(KeyCode::A))
    {
        trans->position.x -= DirectX::XMVectorGetX(right) * speed;
        trans->position.y -= DirectX::XMVectorGetY(right) * speed;
        trans->position.z -= DirectX::XMVectorGetZ(right) * speed;
    }
    if (Input::GetKey(KeyCode::Q))
    {
        trans->position.y += moveSpeed * dt;   // Q = 向上
    }
    if (Input::GetKey(KeyCode::E))
    {
        trans->position.y -= moveSpeed * dt;   // E = 向下
    }

    // 视角旋转（鼠标右键按住）
    if (Input::GetMouseButton(1))
    {
        float dx = (float)Input::GetMouseDeltaX();
        float dy = (float)Input::GetMouseDeltaY();
        trans->rotation.y += dx * lookSensitivity;
        trans->rotation.x += dy * lookSensitivity;
        //wchar_t buf[128];
        //swprintf_s(buf, L"Right held, dx=%.1f, dy=%.1f\n", dx, dy);
        //OutputDebugString(buf);
        // 限制俯仰角，避免万向锁
        if (trans->rotation.x > 89.0f)  trans->rotation.x = 89.0f;
        if (trans->rotation.x < -89.0f) trans->rotation.x = -89.0f;
    }

    // 光标锁定管理
    if (Input::GetMouseButtonDown(1))
        Input::SetCursorLocked(true);
    if (Input::GetMouseButtonUp(1))
        Input::SetCursorLocked(false);
    if (Input::GetKeyDown(KeyCode::Escape))
        Input::SetCursorLocked(false);
}