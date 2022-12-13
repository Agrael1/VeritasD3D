#include "../headers/transform.hlsli"
#include "../headers/vshadow.hlsli"

cbuffer cbPerFrame : register(b2)
{
    float gMaxTessDistance;
    float gMinTessDistance;
    float gMinTessFactor;
    float gMaxTessFactor;
};

struct VSOut
{
    float3 viewPos : Position;
    float3 viewNormal : Normal;
    float3 tan : Tangent;
    float3 bitan : Bitangent;
    float2 tc : Texcoord;
    float4 shadowPos : ShadowPosition;
    float4 pos : SV_Position;
    float TessFactor : TESS;
};

VSOut main(float3 pos : Position, float3 n : Normal, float3 tc : Texcoord, float3 tan : Tangent, float3 bitan : Bitangent)
{
    VSOut vso;
    vso.viewPos = (float3) mul(float4(pos, 1.0f), modelView);
    vso.viewNormal = mul(n, (float3x3) modelView);
    vso.tan = mul(tan, (float3x3) modelView);
    vso.bitan = mul(bitan, (float3x3) modelView);
    vso.pos = mul(float4(pos, 1.0f), modelViewProj);
    vso.shadowPos = ToShadowHomoSpace(pos, model);
    vso.tc = tc.xy;
    
    float distToCamera = -vso.viewPos.z;
    float tess = saturate((gMinTessDistance - distToCamera) / (gMinTessDistance - gMaxTessDistance));
    vso.TessFactor = gMinTessFactor + tess * (gMaxTessFactor - gMinTessFactor);
    
    return vso;
}

