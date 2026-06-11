struct PS_INPUT
{
    float4 screenPosition : SV_POSITION;
    float4 diffuseColor : COLOR0;        //拡散反射の色
    float2 textureUV : TEXCOORD0;        //UV値
    float2 subTextureUV : TEXCOORD1;     //サブテクスチャのUV値
};

Texture2D baseTexture : register(t0); //テクスチャ
SamplerState textureSampler : register(s0); //サンプラー