
cbuffer Transform : register(b0)
{
    matrix modelView;
    matrix proj;
};

struct VSOut
{
    float2 tc : TEXCOORD;
    float4 pos : SV_Position;
};

static const float2 tcs[4]=
{
    float2(0,0),
    float2(0,1),
    float2(1,0),
    float2(1,1),
};

VSOut main(float3 position : POSITION, uint vid : SV_VertexID)
{
    VSOut _out;    
    matrix xmode = matrix(
    float4(1, 0, 0, modelView[0][3]),
    float4(0, 1, 0, modelView[1][3]),
    float4(0, 0, 1, modelView[2][3]),
    modelView[3]);
    
    float4 P = mul(float4(position, 1.0f), xmode);
    
    _out.pos = mul(P, proj);
    _out.tc = tcs[vid];
    return _out;
}