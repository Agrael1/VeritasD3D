TextureCube tex : register(t0);
SamplerState sam : register(s0);

struct PSIn
{
    float3 worldPos : Position;
    float4 pos : SV_Position;
};

float4 main(PSIn input) : SV_Target
{    
    return tex.Sample(sam, input.worldPos);
}