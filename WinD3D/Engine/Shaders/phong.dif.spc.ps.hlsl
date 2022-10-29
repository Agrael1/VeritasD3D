#include "ShaderProcs.hlsli"

cbuffer ObjectCBuf : register(b1)
{
    bool useGlossAlpha;
    bool useSpecularMap;
    float3 specularColor;
    float specularWeight;
    float specularGloss;
};

struct PixelOutDeferred
{
    float4 diffuse : SV_Target0;
    float4 normal : SV_Target1;
    float4 position : SV_Target2;
    float4 specular : SV_Target3;
};

SamplerState splr;
Texture2D tex : register(t0);
Texture2D spec : register(t1);


PixelOutDeferred main(float3 viewFragPos : Position, float3 viewNormal : Normal, float2 tc : Texcoord)
{
    float4 spec_sample = spec.Sample(splr, tc);
    float3 spec_refl = useSpecularMap ? spec_sample.rgb : specularColor;
    float spec_pow = useGlossAlpha ? pow(2.0f, spec_sample.a * 13.0f) : specularGloss;
    
    PixelOutDeferred pOut;
    pOut.diffuse = tex.Sample(splr, tc);
    pOut.normal = float4(normalize(viewNormal), 1.0f);
    pOut.position = float4(viewFragPos, 1.0f);
    pOut.specular = float4(spec_refl * specularWeight, spec_pow);
    return pOut;
}