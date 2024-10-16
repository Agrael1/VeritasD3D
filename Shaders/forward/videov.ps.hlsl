SamplerState splr;
Texture2D tex;

cbuffer buf
{
    uint slice;
};

float4 main(float2 tc : TEXCOORD) : SV_Target
{
    float2 rtc = float2(tc.x, slice==0 ? tc.y * 0.5f + 0.5f : tc.y * 0.5f);
    return tex.Sample(splr, rtc);
}