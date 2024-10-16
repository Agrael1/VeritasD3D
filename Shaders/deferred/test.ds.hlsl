cbuffer Transform : register(b0)
{
    matrix model;
    matrix view;
    matrix proj;
};
struct PatchTess
{
    float EdgeTess[3] : SV_TessFactor;
    float InsideTess : SV_InsideTessFactor;
};
struct HullOut
{
    float3 viewPos : Position;
    float3 viewNormal : Normal;
    float3 tan : Tangent;
    float3 bitan : Bitangent;
    float2 tc : Texcoord;
    float4 shadowPos : ShadowPosition;
};
struct DomainOut
{
    float3 viewPos : Position;
    float3 viewNormal : Normal;
    float3 tan : Tangent;
    float3 bitan : Bitangent;
    float2 tc : Texcoord;
    float4 shadowPos : ShadowPosition;
    float4 pos : SV_Position;
};
Texture2D height : register(t3);
SamplerState splr;

// The domain shader is called for every vertex created by the tessellator.  
// It is like the vertex shader after tessellation.
[domain("tri")]
DomainOut main(PatchTess patchTess,
             float3 bary : SV_DomainLocation,
             const OutputPatch<HullOut, 3> tri)
{
    DomainOut dout;
    dout.viewPos = bary.x * tri[0].viewPos + bary.y * tri[1].viewPos + bary.z * tri[2].viewPos;
    dout.viewNormal = bary.x * tri[0].viewNormal + bary.y * tri[1].viewNormal + bary.z * tri[2].viewNormal;
    dout.tan = bary.x * tri[0].tan + bary.y * tri[1].tan + bary.z * tri[2].tan;
    dout.bitan = bary.x * tri[0].bitan + bary.y * tri[1].bitan + bary.z * tri[2].bitan;
    dout.tc = bary.x * tri[0].tc + bary.y * tri[1].tc + bary.z * tri[2].tc;
    dout.shadowPos = bary.x * tri[0].shadowPos + bary.y * tri[1].shadowPos + bary.z * tri[2].shadowPos;
    dout.pos = mul(float4(dout.viewPos, 1.0f), proj);
    
    // Interpolating normal can unnormalize it, so normalize it.
    //dout.viewNormal = normalize(dout.viewNormal);
    
    //
    // Displacement mapping.
    //
    
    // Choose the mipmap level based on distance to the eye; specifically, choose
    // the next miplevel every MipInterval units, and clamp the miplevel in [0,6].
    const float MipInterval = 20.0f;
    float mipLevel = clamp((dout.viewPos.z - MipInterval) / MipInterval, 0.0f, 6.0f);
    
    // Sample height map (stored in alpha channel).
    float h = height.SampleLevel(splr, dout.tc, mipLevel).r;
    
    // Offset vertex along normal.
    //dout.viewPos += (0.5f * (h - 1.0)) * dout.viewNormal;
    
    // Project to homogeneous clip space.
    //dout.pos = mul(float4(dout.viewPos, 1.0f), proj);
    
    return dout;
}