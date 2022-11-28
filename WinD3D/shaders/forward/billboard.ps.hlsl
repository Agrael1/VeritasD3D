SamplerState splr;
Texture2D tex;

float4 main(float2 tc : TEXCOORD):SV_Target
{
    float4 t = tex.Sample(splr, tc);
    clip(t.a - 0.3f);
    return t;
}