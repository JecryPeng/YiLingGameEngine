float4 main(VS_OUTPUT input) : SV_TARGET
{
    float3 N = normalize(input.worldNormal);
    float3 L = normalize(lightDir.xyz);
    float diffuse = max(dot(N, L), 0.0f);
    float3 ambient = 0.15f; // 微弱环境光，让暗面不完全黑
    float3 finalColor = (ambient + diffuse) * lightColor.rgb;
    return float4(finalColor, 1.0f);
}