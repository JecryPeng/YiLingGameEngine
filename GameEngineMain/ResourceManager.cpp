#include "ResourceManager.h"
#include <cassert>

ResourceManager& ResourceManager::Instance()
{
    static ResourceManager instance;
    return instance;
}

ResourceManager::~ResourceManager()
{
    Clear();
}

Material* ResourceManager::GetOrCreateDefaultMaterial(ID3D11Device* device,
    const std::vector<D3D11_INPUT_ELEMENT_DESC>& layoutDesc,
    ID3DBlob** outVsBlob)
{
    const std::string key = "DefaultWhite";
    auto it = m_Materials.find(key);
    if (it != m_Materials.end())
        return it->second;

    Material* mat = new Material();
    if (!mat->Initialize(device,
        L"VertexShader.hlsl", "main", "vs_5_0",
        L"PixelShader.hlsl", "main", "ps_5_0",
        layoutDesc, outVsBlob))
    {
        delete mat;
        return nullptr;
    }
    m_Materials[key] = mat;
    return mat;
}

Mesh* ResourceManager::GetOrCreateCubeMesh(ID3D11Device* device,
    const std::vector<D3D11_INPUT_ELEMENT_DESC>& layoutDesc,
    ID3DBlob* vsBlob)
{
    // ... 查找缓存代码不变 ...

    // 24 个顶点，每个面有独立的四个顶点，法线为面的法线
    std::vector<Vertex> vertices =
    {
        // 前面 (Z = 0.5)  法线 (0,0,1)
        { { -0.5f, -0.5f,  0.5f }, { 0.0f, 0.0f, 1.0f } },
        { { -0.5f,  0.5f,  0.5f }, { 0.0f, 0.0f, 1.0f } },
        { {  0.5f,  0.5f,  0.5f }, { 0.0f, 0.0f, 1.0f } },
        { {  0.5f, -0.5f,  0.5f }, { 0.0f, 0.0f, 1.0f } },
        // 后面 (Z = -0.5) 法线 (0,0,-1)
        { {  0.5f, -0.5f, -0.5f }, { 0.0f, 0.0f, -1.0f } },
        { {  0.5f,  0.5f, -0.5f }, { 0.0f, 0.0f, -1.0f } },
        { { -0.5f,  0.5f, -0.5f }, { 0.0f, 0.0f, -1.0f } },
        { { -0.5f, -0.5f, -0.5f }, { 0.0f, 0.0f, -1.0f } },
        // 左面 (X = -0.5) 法线 (-1,0,0)
        { { -0.5f, -0.5f, -0.5f }, { -1.0f, 0.0f, 0.0f } },
        { { -0.5f,  0.5f, -0.5f }, { -1.0f, 0.0f, 0.0f } },
        { { -0.5f,  0.5f,  0.5f }, { -1.0f, 0.0f, 0.0f } },
        { { -0.5f, -0.5f,  0.5f }, { -1.0f, 0.0f, 0.0f } },
        // 右面 (X = 0.5)  法线 (1,0,0)
        { {  0.5f, -0.5f,  0.5f }, { 1.0f, 0.0f, 0.0f } },
        { {  0.5f,  0.5f,  0.5f }, { 1.0f, 0.0f, 0.0f } },
        { {  0.5f,  0.5f, -0.5f }, { 1.0f, 0.0f, 0.0f } },
        { {  0.5f, -0.5f, -0.5f }, { 1.0f, 0.0f, 0.0f } },
        // 底面 (Y = -0.5) 法线 (0,-1,0)
        { { -0.5f, -0.5f, -0.5f }, { 0.0f, -1.0f, 0.0f } },
        { {  0.5f, -0.5f, -0.5f }, { 0.0f, -1.0f, 0.0f } },
        { {  0.5f, -0.5f,  0.5f }, { 0.0f, -1.0f, 0.0f } },
        { { -0.5f, -0.5f,  0.5f }, { 0.0f, -1.0f, 0.0f } },
        // 顶面 (Y = 0.5)  法线 (0,1,0)
        { { -0.5f,  0.5f,  0.5f }, { 0.0f, 1.0f, 0.0f } },
        { {  0.5f,  0.5f,  0.5f }, { 0.0f, 1.0f, 0.0f } },
        { {  0.5f,  0.5f, -0.5f }, { 0.0f, 1.0f, 0.0f } },
        { { -0.5f,  0.5f, -0.5f }, { 0.0f, 1.0f, 0.0f } },
    };

    // 索引（36个，每个面两个三角形）
    std::vector<unsigned int> indices =
    {
        0,  1,  2,  0,  2,  3,    // 前面
        4,  5,  6,  4,  6,  7,    // 后面
        8,  9,  10, 8,  10, 11,   // 左面
        12, 13, 14, 12, 14, 15,   // 右面
        16, 17, 18, 16, 18, 19,   // 底面
        20, 21, 22, 20, 22, 23    // 顶面
    };

    Mesh* mesh = new Mesh();
    if (!mesh->Initialize(device, vertices, indices, layoutDesc, vsBlob))
    {
        delete mesh;
        return nullptr;
    }
    const std::string key = "Cube";
    m_Meshes[key] = mesh;
    return mesh;
}

void ResourceManager::Clear()
{
    for (auto& p : m_Meshes)
        delete p.second;
    for (auto& p : m_Materials)
        delete p.second;
    m_Meshes.clear();
    m_Materials.clear();
}