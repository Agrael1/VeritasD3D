cbuffer ObjectCBuf : register(b1)
{
    float3 specularColor;
    float specularWeight;
    float specularGloss;
};

struct PixelOutDeferred
{
    float4 diffuse : SV_Target0;
    float4 normal : SV_Target1;
    float4 position : SV_Target2;
    float4 specular : SV_Target3;
};

SamplerState splr;
Texture2D tex;


PixelOutDeferred main(float3 viewFragPos : Position, float3 viewNormal : Normal, float2 tc : Texcoord)
{
    PixelOutDeferred pOut;
    pOut.diffuse = tex.Sample(splr,tc);    
    pOut.normal = float4(normalize(viewNormal), 1.0f);
    pOut.position = float4(viewFragPos, 1.0f);
    pOut.specular = float4(specularColor * specularWeight, specularGloss);
    return pOut;
}