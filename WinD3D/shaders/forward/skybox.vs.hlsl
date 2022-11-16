cbuffer TransformCBuf : register(b0)
{
    matrix viewProj;
    matrix rotation;
};
struct VSOut
{
    float3 worldPos : Position;
    float4 pos : SV_Position;
};

static const float3 arr[8] =
{
    float3(0.5, 0.5, 0.5),
    float3(-0.5, 0.5, 0.5),
    float3(0.5, -0.5, 0.5),
    float3(-0.5, -0.5, 0.5),
    
    float3(0.5, 0.5, -0.5),
    float3(-0.5, 0.5, -0.5),
    float3(-0.5, -0.5, -0.5),
    float3(0.5, -0.5, -0.5),
};

VSOut main(uint VertexID : SV_VertexID)
{
    VSOut output;
    output.worldPos = arr[VertexID];
    output.pos = mul(mul(float4(output.worldPos, 0.0f), rotation), viewProj);
    output.pos.z = output.pos.w;
    return output;
}