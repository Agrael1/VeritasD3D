Texture2D o1;
Texture2D o2;
SamplerState splr;

float4 main(float2 tc : TEXCOORD) : SV_Target
{
    float3 t1 = float3(o1.Sample(splr, tc).r,0,0);
    float2 t2x = o2.Sample(splr, tc).gb;
    float3 t2 = float3(0, t2x.x, t2x.y);
    return float4(t1+t2, 1.0f);
}