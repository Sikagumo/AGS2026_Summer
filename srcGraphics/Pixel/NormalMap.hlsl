SamplerState smp : register(s0);
Texture2D<float4> tex : register(t0);
Texture2D<float4> normTex : register(t1); 

cbuffer BaseBuffer : register(b4)
{
    float3 lvec;
    float padding; 
}

struct PSInput
{
    float4 position : SV_POSITION;
    float4 color : COLOR0;
    float2 uv : TEXCOORD0;
};

float4 main(PSInput input) : SV_TARGET
{
    float4 color = tex.Sample(smp, input.uv);
    
    if (color.a <= 0.01f)
    {
        discard;
    }

    // ƒm[ƒ}ƒ‹ƒ}ƒbƒv‚ÌŒvŽZ
    float4 norm = normTex.Sample(smp, input.uv);
    norm *= 2.0;
    norm -= 1.0;
    norm.z = -norm.z;
    norm.xyz = normalize(norm.xyz);
    
    float3 viewVec = float3(0, 0, 1);
    float3 lightVec = normalize(lvec);
    
    // ‰A‰e‚ÆŒõ‘ò‚ÌŒvŽZ
    float diffuse = saturate(dot(-lightVec, norm.xyz));
    
    float3 refVec = reflect(lightVec, norm.xyz);
    float spec = pow(saturate(dot(refVec, -viewVec)), 20.0f);
    spec *= color.a;
    
    return float4(color.rgb * diffuse + spec, color.a);
}