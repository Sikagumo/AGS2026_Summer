#include "../Common/Pixel/PixelShader2DHeader.hlsli"

Texture2D normalTexture : register(s2);

cbuffer BaseBuffer : register(b4)
{
    float3 lightVec; // ライトベクトル
    float specshins; // スペキュラの鋭さ
    float ambient;   // 環境光の強さ
    float3 dummy;    // パディング(定数バッファの整合性用)
};

float4 main(PS_INPUT input) : SV_TARGET
{
    // テクスチャから色情報と法線情報を取得
    float4 baseColor = baseTexture.Sample(textureSampler, input.textureUV);
    float3 normalVecRaw = normalTexture.Sample(textureSampler, input.textureUV).rgb;
    
    // 法線の復元
    norm = normalize(norm * 2.0 - 1.0);
    norm.z = -norm.z;
    
    // ライティング計算
    float3 lightDir = normalize(levc);
    float diffuse = saturate(dot(-lightDir, norm));
    
    // スペキュラ計算(鏡面反射)
    float3 viewDir = float3(0, 0, 1);
    float3 ref = 
	return float4(1.0f, 1.0f, 1.0f, 1.0f);
}