#include "Toon.hlsli"

Texture2D diffuseMap : register(t0);
SamplerState diffuseMapSamplerState : register(s0);

float4 main(VS_OUT pin) : SV_TARGET
{
    // テクスチャカラーを取得
   // float4 texColor = diffuseMap.Sample(diffuseMapSamplerState, pin.texcoord);
    float4 texColor = diffuseMap.Sample(diffuseMapSamplerState, pin.texcoord) * 1.3f; // 明るさを調整
    // 基本的なカラーとライティングを組み合わせる
    float3 baseColor = texColor.rgb * pin.color.rgb;

    // 明るさの計算（グレースケール的ウェイトを使用）
    float intensity = dot(baseColor, float3(0.3, 0.59, 0.11));

    intensity *= 1.5; // 明るさを 1.5 倍に

    // トゥーンライティングステッピング（段階的な明るさ）
    if (intensity > 0.8)
        intensity = 1.0;  // 明るい部分
    else if (intensity > 0.6)
        intensity = 0.8;  // 中間部分
    else if (intensity > 0.4)
        intensity = 0.6;  // 暗い部分
    else 
        intensity = 0.1;

    // 最終色の出力
    return float4(intensity * baseColor, texColor.a * pin.color.a);
}