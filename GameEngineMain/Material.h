#pragma once
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <vector>

class Material
{
public:
    Material();
    ~Material();

    bool Initialize(ID3D11Device* device,
        const wchar_t* vsPath, const char* vsEntry, const char* vsTarget,
        const wchar_t* psPath, const char* psEntry, const char* psTarget,
        const std::vector<D3D11_INPUT_ELEMENT_DESC>& layoutDesc,
        ID3DBlob** outVsBlob = nullptr);

    void Apply(ID3D11DeviceContext* context) const;   // 声明，实现在 Material.cpp

    void UpdateConstantBuffer(ID3D11DeviceContext* context,
        const DirectX::XMMATRIX& world,
        const DirectX::XMMATRIX& viewProj,
        const DirectX::XMFLOAT3& lightDir,
        const DirectX::XMFLOAT4& lightColor);

    void Release();

private:
    ID3D11VertexShader* m_pVS = nullptr;
    ID3D11PixelShader* m_pPS = nullptr;
    ID3D11Buffer* m_pConstantBuffer = nullptr;
};

// 常量缓冲区结构体，16字节对齐
struct LightConstantBuffer
{
    DirectX::XMFLOAT4X4 world;
    DirectX::XMFLOAT4X4 viewProj;
    DirectX::XMFLOAT4   lightDir;
    DirectX::XMFLOAT4   lightColor;
};