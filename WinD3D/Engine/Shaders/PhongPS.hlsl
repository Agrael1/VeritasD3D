#include "ShaderProcs.hlsli"
#include "PointLight.hlsli"
#include "LightVectorData.hlsli"

cbuffer ObjectCBuf
{
    float specularIntensity;
    float specularPower;
    float padding[2];
};
Texture2D tex : register(t0);
SamplerState splr;


float4 main(float3 viewFragPos : Position, float3 viewNormal : Normal, float2 tc : Texcoord) : SV_Target
{
    // renormalize interpolated normal
    viewNormal = normalize(viewNormal);
	// fragment to light vector data
    const LightVectorData lv = CalculateLightVectorData(viewLightPos, viewFragPos);
	// attenuation
    const float att = Attenuate(attConst, attLin, attQuad, lv.distToL);
	// diffuse
    const float3 diffuse = Diffuse(diffuseColor, diffuseIntensity, att, lv.dirToL, viewNormal);
	// specular
    const float3 specular = Speculate(diffuseColor, diffuseIntensity * specularIntensity, viewNormal, lv.vToL, viewFragPos, att, specularPower);
	// final color
    return float4(saturate((diffuse + ambient) * tex.Sample(splr, tc).rgb + specular), 1.0f);
}