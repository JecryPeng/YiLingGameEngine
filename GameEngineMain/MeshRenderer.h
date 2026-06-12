#pragma once
#include "Component.h"
#include "Mesh.h"
#include "Material.h"
#include <DirectXMath.h>

class MeshRenderer : public Component
{
public:
    Mesh* mesh = nullptr;
    Material* material = nullptr;

    void Render(ID3D11DeviceContext* context,
        const DirectX::XMMATRIX& world,
        const DirectX::XMMATRIX& viewProj,
        const DirectX::XMFLOAT3& lightDir,
        const DirectX::XMFLOAT4& lightColor);
};