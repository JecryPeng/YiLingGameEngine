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
    // 世界空间位置
    float4 worldPos = mul(float4(input.pos, 1.0f), world);
    output.worldPos = worldPos.xyz;
    // 世界空间法线（仅旋转变换，忽略平移，假设无缩放或均匀缩放）
    output.worldNormal = mul(input.normal, (float3x3)world);
    // 齐次裁剪空间位置
    output.pos = mul(worldPos, viewProj);
    return output;
}