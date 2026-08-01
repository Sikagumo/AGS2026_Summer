// VS/PS共通
#include "../Common/VertexToPixelHeader.hlsli"

// IN
#define PS_INPUT VertexToPixelLit

// PS
#include "../Common/Pixel/PixelShader3DHeader.hlsli"

float4 main(PS_INPUT PSInput) : SV_TARGET0
{
	// テクスチャーの色を取得(頂点シェーダ側で拡大されたUV座標を使用)
    float4 color = diffuseMapTexture.Sample(diffuseMapSampler, PSInput.uv);
    
    if (color.a < 0.01f)
    {
        discard;
    }

    return color;

}