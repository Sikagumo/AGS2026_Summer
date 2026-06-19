#include "../Common/Pixel/PixelShader2DHeader.hlsli"

Texture2D normalMapTexture : register(t2);

cbuffer LightingParameter : register(b4)
{
    float3 lightDirection;
    float specularShininess;
    float ambientIntensity;
    float3 padding;
};

float4 main(PS_INPUT input) : SV_TARGET
{
    // テクスチャから色情報と法線情報を取得
    float4 baseColor = baseTexture.Sample(textureSampler, input.textureUV);
    float3 normalVectorRaw = normalMapTexture.Sample(textureSampler, input.textureUV).rgb;
    
    // 法線ベクトルの復元と正規化
    float3 normalVector = normalize(normalVectorRaw * 2.0 - 1.0);
    normalVector.z = -normalVector.z;
    
    // ライティング計算（拡散反射）
    float3 normalLightDist = normalize(lightDirection);
    float diffuseFactor = saturate(dot(-normalLightDist, normalVector));
    
    // スペキュラ計算（鏡面反射）
    float3 viewDirection = float3(0, 0, 1);
    float3 reflectVector = reflect(normalLightDist, normalVector);
    float specularFactor = pow(saturate(dot(reflectVector, -viewDirection)), specularShininess);
    
    // 最終色の合成
    float3 litColor = baseColor.rgb * (diffuseFactor + ambientIntensity) + specularFactor;
    
    return float4(litColor, baseColor.a);
}