SamplerState splr;
Texture2D tex;

cbuffer Material : register(b0)
{
    float3 color = float3(1,1,1);
};

float4 main(float2 tc : TEXCOORD):SV_Target
{
    float4 t = tex.Sample(splr, tc);
    t = lerp(float4(0, 0, 0, 0), float4(color, 1.0f), t.x);
    clip(t.a - 0.3f);
    return t;
}