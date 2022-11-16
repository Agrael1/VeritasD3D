#include "../headers/deferred.hlsli"

cbuffer CBuf : register(b1)
{
    float3 materialColor;
}

PixelOutDeferred main(float3 viewFragPos : Position, float4 shadowPos : ShadowPosition)
{
    PixelOutDeferred _out;
    _out.diffuse = float4(materialColor, 1.0f);
    _out.normal = float4(0.0f, 0.0f, 0.0f, 0.0f);
    _out.position = float4(viewFragPos, 1.0f);
    _out.specular = float4(0.0f, 0.0f, 0.0f, 1.0f);
    _out.shadow = shadowPos;
    return _out;
}