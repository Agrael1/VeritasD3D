#include "../headers/deferred.hlsli"
#include "ShaderProcs.hlsli"

cbuffer ObjectCBuf : register(b1)
{
    float3 specularColor;
    float specularWeight;
    float specularGloss;
    bool useNormalMap;
    float normalMapWeight;
    float mesh_id;
};

cbuffer Picking : register(b3)
{
    float pick_id;
};

SamplerState splr;
Texture2D tex : register(t0);
Texture2D nmap : register(t2);


PixelOutDeferred main(float3 viewFragPos : Position, float3 viewNormal : Normal, float3 viewTan : Tangent, float3 viewBitan : Bitangent, float2 tc : Texcoord, float4 shadowPos : ShadowPosition)
{
    if (useNormalMap)
    {
        const float3 mappedNormal = MapNormal(normalize(viewTan), normalize(viewBitan), viewNormal, tc, nmap, splr);
        viewNormal = lerp(viewNormal, mappedNormal, normalMapWeight);
    }
    
    float4 color = tex.Sample(splr, tc);

    [flatten]
    if (pick_id == mesh_id)color *= 0.5f;

    PixelOutDeferred pOut;
    pOut.diffuse = color;
    pOut.normal = float4(normalize(viewNormal), 1.0f);
    pOut.position = float4(viewFragPos, 1.0f);
    pOut.specular = float4(specularColor * specularWeight, specularGloss);
    pOut.shadow = shadowPos;
    return pOut;
}