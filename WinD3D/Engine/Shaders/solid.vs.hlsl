cbuffer CBuf
{
    matrix modelView;
    matrix modelViewProj;
};

struct VSOut
{
    float3 view_pos : position;
    float4 pos: SV_POSITION;
};

VSOut main(float3 pos : POSITION)
{
    VSOut _out;
    _out.view_pos = mul(float4(pos, 1.0f), modelView).xyz;
    _out.pos = mul(float4(pos, 1.0f), modelViewProj);
    return _out;
}