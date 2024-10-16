SamplerState splr;
Texture2DArray tex;

cbuffer buf
{
    uint slice;
};

float4 main(float2 tc : TEXCOORD) : SV_Target
{
    return tex.Sample(splr, float3(tc, float(slice)));
}