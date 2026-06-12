#pragma once
#include <d3d11.h>
#include <vector>
#include <DirectXMath.h>

// 简单顶点结构（后续可扩展法线、UV等）
struct Vertex
{
    DirectX::XMFLOAT3 position;
    DirectX::XMFLOAT3 normal;   // 新增法线
};

class Mesh
{
public:
    Mesh();
    ~Mesh();

    // 初始化：创建顶点/索引缓冲区
    bool Initialize(ID3D11Device* device,
        const std::vector<Vertex>& vertices,
        const std::vector<unsigned int>& indices,
        const std::vector<D3D11_INPUT_ELEMENT_DESC>& layoutDesc,
        ID3DBlob* vsBlob);

    // 绑定到管线（输入装配阶段）
    void Bind(ID3D11DeviceContext* context) const;

    // 绘制
    void Draw(ID3D11DeviceContext* context) const;

    void Release();

private:
    ID3D11Buffer* m_pVertexBuffer = nullptr;
    ID3D11Buffer* m_pIndexBuffer = nullptr;
    ID3D11InputLayout* m_pInputLayout = nullptr;
    UINT m_VertexCount = 0;
    UINT m_IndexCount = 0;
    DXGI_FORMAT m_IndexFormat = DXGI_FORMAT_R32_UINT;
};