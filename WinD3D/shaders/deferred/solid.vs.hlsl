#include "../headers/transform.hlsli"
#include "../headers/vshadow.hlsli"

struct VSOut
{
    float3 view_pos : position;
    float4 shadowPos : ShadowPosition;
    float4 pos: SV_POSITION;
};

VSOut main(float3 pos : POSITION)
{
    VSOut _out;
    _out.view_pos = mul(float4(pos, 1.0f), modelView).xyz;
    _out.pos = mul(float4(pos, 1.0f), modelViewProj);
    _out.shadowPos = ToShadowHomoSpace(pos, model);
    return _out;
}