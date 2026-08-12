#include "../Common/Pixel/PixelShader2DHeader.hlsli"
Texture2D<float4> normTex : register(t1);


cbuffer BaseBuffer : register(b4)
{
    // 1ブロック目（16バイト）：ライティング情報
    float3 lvec;
    float ambient;
   
    // 2ブロック目（16バイト）：波・制御フラグ情報
    float time;
    float waveSpeed;
    float waveForce;
    float useNormal;
}

float4 main(PS_INPUT input) : SV_TARGET
{
    float2 uv = input.uv;

    // 波の計算
    uv.y += sin(uv.x * 20.0f + time * waveSpeed) * waveForce;
    uv.x -= cos(time * (waveSpeed * 0.5f)) * (waveForce * 0.5f);

    float4 color = baseTexture.Sample(textureSampler, uv);
    if (color.a <= 0.01f)
    {
        discard;
    }

    // 立体感フラグによる分岐判定
    if (useNormal > 0.5f)
    {
        // 立体ONの処理
        float4 norm = normTex.Sample(textureSampler, uv);
        norm *= 2.0;
        norm -= 1.0;
        norm.z = -norm.z;
        norm.xyz = normalize(norm.xyz);
        
        float3 viewVec = float3(0, 0, 1);
        float3 lightVec = normalize(lvec);
        
        float diffuse = saturate(dot(-lightVec, norm.xyz));
        diffuse = max(diffuse, ambient); 
        
        float3 refVec = reflect(lightVec, norm.xyz);
        float spec = pow(saturate(dot(refVec, -viewVec)), 20.0f);
        spec *= color.a;
        
        return float4(color.rgb * diffuse + spec, color.a);
    }
    else
    {
        // 立体OFFの処理
        return color;
    }
}