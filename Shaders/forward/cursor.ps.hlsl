
cbuffer CBuf : register(b3)
{
    float meshID = 0.0f;
}

[earlydepthstencil]
float4 main(float4 pos : SV_POSITION):SV_TARGET
{
    return float4(meshID, pos.z,0,0);
}