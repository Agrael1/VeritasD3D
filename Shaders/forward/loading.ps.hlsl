SamplerState splr;
Texture2D tex;

float4 main(float2 tc : TEXCOORD) : SV_Target
{
    return tex.Sample(splr, tc);
}