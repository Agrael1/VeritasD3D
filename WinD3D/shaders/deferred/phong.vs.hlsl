#include "../headers/vshadow.hlsli"
#include "../headers/transform.hlsli"


struct VSOut
{
    float3 viewPos : Position;
    float3 viewNormal : Normal;
    float4 shadowPos : ShadowPosition;
    float4 pos : SV_Position;
};

VSOut main(float3 pos : Position, float3 n : Normal)
{
    VSOut vso;
    vso.viewPos = mul(float4(pos, 1.0f), modelView).xyz;
    vso.viewNormal = mul(n, (float3x3) modelView);
    vso.shadowPos = ToShadowHomoSpace(pos, model);
    vso.pos = mul(float4(pos, 1.0f), modelViewProj);
    return vso;
}