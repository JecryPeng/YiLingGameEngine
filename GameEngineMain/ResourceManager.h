#pragma once
#include <unordered_map>
#include <string>
#include <d3d11.h>
#include "Mesh.h"
#include "Material.h"

class ResourceManager
{
public:
    static ResourceManager& Instance();

    // 获取或创建一个默认材质（可复用）
    Material* GetOrCreateDefaultMaterial(ID3D11Device* device,
        const std::vector<D3D11_INPUT_ELEMENT_DESC>& layoutDesc,
        ID3DBlob** outVsBlob = nullptr);

    // 获取或创建一个立方体 Mesh
    Mesh* GetOrCreateCubeMesh(ID3D11Device* device,
        const std::vector<D3D11_INPUT_ELEMENT_DESC>& layoutDesc,
        ID3DBlob* vsBlob);

    // 清理所有资源
    void Clear();

private:
    ResourceManager() = default;
    ~ResourceManager();
    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;

    std::unordered_map<std::string, Mesh*> m_Meshes;
    std::unordered_map<std::string, Material*> m_Materials;
};