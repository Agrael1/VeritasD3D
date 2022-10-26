struct PointLight
{
    float3 viewLightPos;
    float3 diffuseColor;
    float diffuseIntensity;
    float attConst;
    float attLin;
    float attQuad;
};

cbuffer PointLightCBuf : register(b0)
{
    uint count;   
    StructuredBuffer<PointLight> lights;
};