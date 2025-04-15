#include "Toon.hlsli"

VS_OUT main(
    float4 position     : POSITION,
    float3 normal : NORMAL,
    float3 tangent : TANGENT,
    float2 texcoord : TEXCOORD,
    float4 color : COLOR,
    float4 boneWeights : WEIGHTS,
    uint4  boneIndices : BONES
)
{
    float3 p = { 0, 0, 0 };
    float3 n = { 0, 0, 0 };
    for (int i = 0; i < 4; i++)
    {
        p += (boneWeights[i] * mul(position, boneTransforms[boneIndices[i]])).xyz;
        n += (boneWeights[i] * mul(float4(normal.xyz, 0), boneTransforms[boneIndices[i]])).xyz;
    }

    VS_OUT vout;

    // ワールド空間の座標を計算
    //vout.worldPos = mul(position, worldMatrix).xyz;

    // クリップ空間の座標を計算
    vout.position = mul(float4(p, 1.0f), viewProjection);

    // 法線をワールド空間に変換
  //  vout.normal = normalize(mul(float4(normal, 0.0f), worldMatrix).xyz);

    // 法線とライティング計算
    float3 N = normalize(n);
    float3 L = normalize(-lightDirection.xyz);

    // ライトの強度をトゥーンステップ用に計算
    float d = dot(L, N);
    float power = max(0, d) * 0.3f + 0.7f;

    // トゥーンステップ
    if (power > 0.8)
        power = 1.0;
    else if (power > 0.5)
        power = 0.7;
    else if (power > 0.2)
        power = 0.4;
    else
        power = 0.1;

    vout.color.rgb = color.rgb * materialColor.rgb * power;
    vout.color.a = color.a * materialColor.a;
    vout.texcoord = texcoord;

    return vout;
}