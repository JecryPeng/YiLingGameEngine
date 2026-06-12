#include "Mesh.h"

Mesh::Mesh() {}
Mesh::~Mesh() { Release(); }

bool Mesh::Initialize(ID3D11Device* device,
    const std::vector<Vertex>& vertices,
    const std::vector<unsigned int>& indices,
    const std::vector<D3D11_INPUT_ELEMENT_DESC>& layoutDesc,
    ID3DBlob* vsBlob)
{
    if (vertices.empty()) return false;

    // 1. 顶点缓冲区
    D3D11_BUFFER_DESC vbd = {};
    vbd.Usage = D3D11_USAGE_DEFAULT;
    vbd.ByteWidth = static_cast<UINT>(sizeof(Vertex) * vertices.size()); // ★ 显式转换
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    D3D11_SUBRESOURCE_DATA vInit = {};
    vInit.pSysMem = vertices.data();
    if (FAILED(device->CreateBuffer(&vbd, &vInit, &m_pVertexBuffer)))
        return false;

    // 2. 索引缓冲区（可选）
    if (!indices.empty())
    {
        D3D11_BUFFER_DESC ibd = {};
        ibd.Usage = D3D11_USAGE_DEFAULT;
        ibd.ByteWidth = static_cast<UINT>(sizeof(unsigned int) * indices.size()); // ★ 显式转换
        ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
        D3D11_SUBRESOURCE_DATA iInit = {};
        iInit.pSysMem = indices.data();
        if (FAILED(device->CreateBuffer(&ibd, &iInit, &m_pIndexBuffer)))
            return false;
        m_IndexCount = static_cast<UINT>(indices.size()); // 也可以加上，保持一致
    }

    m_VertexCount = static_cast<UINT>(vertices.size());

    // 3. 输入布局
    if (vsBlob)
    {
        device->CreateInputLayout(layoutDesc.data(), static_cast<UINT>(layoutDesc.size()),
            vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &m_pInputLayout);
    }

    return true;
}

void Mesh::Bind(ID3D11DeviceContext* context) const
{
    UINT stride = sizeof(Vertex);
    UINT offset = 0;
    context->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);
    if (m_pIndexBuffer)
        context->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
    context->IASetInputLayout(m_pInputLayout);
}

void Mesh::Draw(ID3D11DeviceContext* context) const
{
    if (m_pIndexBuffer)
        context->DrawIndexed(m_IndexCount, 0, 0);
    else
        context->Draw(m_VertexCount, 0);
}

void Mesh::Release()
{
    if (m_pInputLayout) { m_pInputLayout->Release(); m_pInputLayout = nullptr; }
    if (m_pVertexBuffer) { m_pVertexBuffer->Release(); m_pVertexBuffer = nullptr; }
    if (m_pIndexBuffer) { m_pIndexBuffer->Release(); m_pIndexBuffer = nullptr; }
}