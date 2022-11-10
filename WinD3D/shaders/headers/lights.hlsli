#define MAXLIGHTS 32

struct PointLight
{
    float4 viewLightPos;
    float3 diffuseColor;
    float diffuseIntensity;
    float attConst;
    float attLin;
    float attQuad;
    float padding;
};

cbuffer PointLightCBuf : register(b7)
{
    float4x4 view;
    float3 ambient;
    uint count;   
    PointLight lights[MAXLIGHTS];
};