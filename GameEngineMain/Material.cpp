#include "Material.h"

Material::Material() {}
Material::~Material() { Release(); }

bool Material::Initialize(ID3D11Device* device,
    const wchar_t* vsPath, const char* vsEntry, const char* vsTarget,
    const wchar_t* psPath, const char* psEntry, const char* psTarget,
    const std::vector<D3D11_INPUT_ELEMENT_DESC>& layoutDesc,
    ID3DBlob** outVsBlob)
{
    // ------ 硬编码顶点着色器（支持法线与光照） ------
    const char* vsCode = R"(
        cbuffer LightCB : register(b0)
        {
            matrix world;
            matrix viewProj;
            float4 lightDir;
            float4 lightColor;
        }

        struct VS_INPUT
        {
            float3 pos : POSITION;
            float3 normal : NORMAL;
        };

        struct VS_OUTPUT
        {
            float4 pos : SV_POSITION;
            float3 worldNormal : NORMAL;
            float3 worldPos : TEXCOORD0;
        };

        VS_OUTPUT main(VS_INPUT input)
        {
            VS_OUTPUT output;
            float4 worldPos = mul(float4(input.pos, 1.0f), world);
            output.worldPos = worldPos.xyz;
            output.worldNormal = mul(input.normal, (float3x3)world);
            output.pos = mul(worldPos, viewProj);
            return output;
        }
    )";

    // ------ 硬编码像素着色器（简单漫反射） ------
    const char* psCode = R"(
    cbuffer LightCB : register(b0)
    {
        matrix world;          // PS 中可能不需要，但必须声明以匹配整个缓冲区布局
        matrix viewProj;       // 同上
        float4 lightDir;
        float4 lightColor;
    };

    struct PS_INPUT
    {
        float4 pos : SV_POSITION;
        float3 worldNormal : NORMAL;
        float3 worldPos : TEXCOORD0;
    };

    float4 main(PS_INPUT input) : SV_TARGET
    {
        float3 N = normalize(input.worldNormal);
        float3 L = normalize(lightDir.xyz);
        float diffuse = max(dot(N, L), 0.0f);
        float3 ambient = 0.15f;
        float3 color = (ambient + diffuse) * lightColor.rgb;
        return float4(color, 1.0f);
    }
)";

    ID3DBlob* vsBlob = nullptr;
    ID3DBlob* psBlob = nullptr;
    ID3DBlob* errBlob = nullptr;

    // 编译顶点着色器
    HRESULT hrVS = D3DCompile(vsCode, strlen(vsCode), nullptr, nullptr, nullptr,
        "main", "vs_5_0", 0, 0, &vsBlob, &errBlob);
    if (FAILED(hrVS))
    {
        if (errBlob) {
            OutputDebugStringA((char*)errBlob->GetBufferPointer());
            errBlob->Release();
        }
        return false;
    }

    // 编译像素着色器
    HRESULT hrPS = D3DCompile(psCode, strlen(psCode), nullptr, nullptr, nullptr,
        "main", "ps_5_0", 0, 0, &psBlob, &errBlob);
    if (FAILED(hrPS))
    {
        vsBlob->Release();
        if (errBlob) {
            OutputDebugStringA((char*)errBlob->GetBufferPointer());
            errBlob->Release();
        }
        return false;
    }

    // 创建着色器对象
    device->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(),
        nullptr, &m_pVS);
    device->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(),
        nullptr, &m_pPS);

    // 输出 VS Blob 给调用者（用于创建输入布局）
    if (outVsBlob) {
        *outVsBlob = vsBlob;
    }
    else {
        vsBlob->Release();
    }
    psBlob->Release();

    // 创建常量缓冲区（光照版本）
    D3D11_BUFFER_DESC cbDesc = {};
    cbDesc.Usage = D3D11_USAGE_DYNAMIC;
    cbDesc.ByteWidth = sizeof(LightConstantBuffer);
    cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    if (FAILED(device->CreateBuffer(&cbDesc, nullptr, &m_pConstantBuffer)))
        return false;

    return true;
}

void Material::Apply(ID3D11DeviceContext* context) const
{
    context->VSSetShader(m_pVS, nullptr, 0);
    context->PSSetShader(m_pPS, nullptr, 0);
    context->VSSetConstantBuffers(0, 1, &m_pConstantBuffer);
    context->PSSetConstantBuffers(0, 1, &m_pConstantBuffer);
}

void Material::UpdateConstantBuffer(ID3D11DeviceContext* context,
    const DirectX::XMMATRIX& world,
    const DirectX::XMMATRIX& viewProj,
    const DirectX::XMFLOAT3& lightDir,
    const DirectX::XMFLOAT4& lightColor)
{
    LightConstantBuffer cb;
    DirectX::XMStoreFloat4x4(&cb.world, DirectX::XMMatrixTranspose(world));
    DirectX::XMStoreFloat4x4(&cb.viewProj, DirectX::XMMatrixTranspose(viewProj));
    cb.lightDir = DirectX::XMFLOAT4(lightDir.x, lightDir.y, lightDir.z, 0.0f);
    cb.lightColor = lightColor;

    D3D11_MAPPED_SUBRESOURCE mapped;
    if (SUCCEEDED(context->Map(m_pConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped)))
    {
        memcpy(mapped.pData, &cb, sizeof(cb));
        context->Unmap(m_pConstantBuffer, 0);
    }
}

void Material::Release()
{
    if (m_pConstantBuffer) { m_pConstantBuffer->Release(); m_pConstantBuffer = nullptr; }
    if (m_pVS) { m_pVS->Release();             m_pVS = nullptr; }
    if (m_pPS) { m_pPS->Release();             m_pPS = nullptr; }
}