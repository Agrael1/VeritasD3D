#include "../headers/deferred.hlsli"

cbuffer ObjectCBuf : register(b1)
{
    bool useGlossAlpha;
    bool useSpecularMap;
    float3 specularColor;
    float specularWeight;
    float specularGloss;
    float mesh_id;
};

cbuffer Picking : register(b3)
{
    float pick_id;
};

SamplerState splr;
Texture2D tex : register(t0);
Texture2D spec : register(t1);


PixelOutDeferred main(float3 viewFragPos : Position, float3 viewNormal : Normal, float2 tc : Texcoord, float4 shadowPos : ShadowPosition)
{
    float4 spec_sample = spec.Sample(splr, tc);
    float3 spec_refl = useSpecularMap ? spec_sample.rgb : specularColor;
    float spec_pow = useGlossAlpha ? pow(2.0f, spec_sample.a * 13.0f) : specularGloss;
    
    float4 color = tex.Sample(splr, tc);

    [flatten]
    if (pick_id == mesh_id)color *= 0.5f;

    PixelOutDeferred pOut;
    pOut.diffuse = color;
    pOut.normal = float4(normalize(viewNormal), 1.0f);
    pOut.position = float4(viewFragPos, 1.0f);
    pOut.specular = float4(spec_refl * specularWeight, spec_pow);
    pOut.shadow = shadowPos;
    return pOut;
}