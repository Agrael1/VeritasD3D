#define MAXLIGHTS 32

struct PointLight
{
    float4 viewLightPos;
    
    float3 diffuseColor;
    float diffuseIntensity;
    
    float attConst;
    float attLin;
    float attQuad;
    bool active;
};

cbuffer PointLightCBuf : register(b7)
{
    float3 ambient;
    uint count;   
    PointLight lights[MAXLIGHTS];
};