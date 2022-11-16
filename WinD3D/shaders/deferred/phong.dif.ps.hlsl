#include "../headers/deferred.hlsli"

cbuffer ObjectCBuf : register(b1)
{
    float3 specularColor;
    float specularWeight;
    float specularGloss;
};

SamplerState splr;
Texture2D tex;


PixelOutDeferred main(float3 viewFragPos : Position, float3 viewNormal : Normal, float2 tc : Texcoord, float4 shadowPos : ShadowPosition)
{
    PixelOutDeferred pOut;
    pOut.diffuse = tex.Sample(splr,tc);    
    pOut.normal = float4(normalize(viewNormal), 1.0f);
    pOut.position = float4(viewFragPos, 1.0f);
    pOut.specular = float4(specularColor * specularWeight, specularGloss);
    pOut.shadow = shadowPos;
    return pOut;
}