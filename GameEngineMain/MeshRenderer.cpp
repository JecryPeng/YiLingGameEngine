#include "MeshRenderer.h"

void MeshRenderer::Render(ID3D11DeviceContext* context,
    const DirectX::XMMATRIX& world,
    const DirectX::XMMATRIX& viewProj,
    const DirectX::XMFLOAT3& lightDir,
    const DirectX::XMFLOAT4& lightColor)
{
    if (!mesh || !material) return;

    material->UpdateConstantBuffer(context, world, viewProj, lightDir, lightColor);
    material->Apply(context);
    mesh->Bind(context);
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    mesh->Draw(context);
}