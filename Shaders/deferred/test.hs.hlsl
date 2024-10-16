struct PatchTess
{
    float EdgeTess[3] : SV_TessFactor;
    float InsideTess : SV_InsideTessFactor;
};

struct VSOut
{
    float3 viewPos : Position;
    float3 viewNormal : Normal;
    float3 tan : Tangent;
    float3 bitan : Bitangent;
    float2 tc : Texcoord;
    float4 shadowPos : ShadowPosition;
    float TessFactor : TESS;
};

PatchTess PatchHS(InputPatch<VSOut, 3> patch,
                  uint patchID : SV_PrimitiveID)
{
    PatchTess pt;
    
    // Average tess factors along edges, and pick an edge tess factor for 
    // the interior tessellation.  It is important to do the tess factor
    // calculation based on the edge properties so that edges shared by 
    // more than one triangle will have the same tessellation factor.  
    // Otherwise, gaps can appear.
    pt.EdgeTess[0] = 0.5f * (patch[1].TessFactor + patch[2].TessFactor);
    pt.EdgeTess[1] = 0.5f * (patch[2].TessFactor + patch[0].TessFactor);
    pt.EdgeTess[2] = 0.5f * (patch[0].TessFactor + patch[1].TessFactor);
    pt.InsideTess = pt.EdgeTess[0];
    
    return pt;
}

struct HullOut
{
    float3 viewPos : Position;
    float3 viewNormal : Normal;
    float3 tan : Tangent;
    float3 bitan : Bitangent;
    float2 tc : Texcoord;
    float4 shadowPos : ShadowPosition;
};

[domain("tri")]
[partitioning("fractional_odd")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(3)]
[patchconstantfunc("PatchHS")]
HullOut main(InputPatch<VSOut, 3> p,
           uint i : SV_OutputControlPointID,
           uint patchId : SV_PrimitiveID)
{
    HullOut hout;
    
    hout.viewPos = p[i].viewPos;
    hout.viewNormal = p[i].viewNormal;
    hout.tan = p[i].tan;
    hout.bitan = p[i].bitan;
    hout.tc = p[i].tc;
    hout.shadowPos = p[i].shadowPos;
    
    return hout;
}