struct PS_INPUT
{
    float4 position : SV_POSITION;
    float4 color : COLOR0;        //拡散反射の色
    float2 uv : TEXCOORD0;        //UV値
    float2 subUV : TEXCOORD1;     //サブテクスチャのUV値
};

Texture2D baseTexture : register(t0); //テクスチャ
SamplerState textureSampler : register(s0); //サンプラー