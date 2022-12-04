SamplerState splr;
Texture2DArray tex;

cbuffer Material : register(b0)
{
    float3 color = float3(1,1,1);
    uint frame = 0;
};


  float3 HUEtoRGB(in float H)
  {
    float R = abs(H * 6 - 3) - 1;
    float G = 2 - abs(H * 6 - 2);
    float B = 2 - abs(H * 6 - 4);
    return saturate(float3(R,G,B));
  }
 float3 HSLtoRGB(in float3 HSL)
  {
    float3 RGB = HUEtoRGB(HSL.x);
    float C = (1 - abs(2 * HSL.z - 1)) * HSL.y;
    return (RGB - 0.5) * C + HSL.z;
  }

float4 main(float2 tc : TEXCOORD):SV_Target
{
    float4 t = tex.Sample(splr, float3(tc, float(frame)));
    float3 cc = color;
    cc.z = t.x;
    
    t = lerp(float4(0, 0, 0, 0), float4(HSLtoRGB(cc), 1.0f), t.x);
    clip(t.a - 0.3f);
    return t;
}