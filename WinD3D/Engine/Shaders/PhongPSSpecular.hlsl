#include "ShaderProcs.hlsli"
#include "LightVectorData.hlsli"
#include "PointLight.hlsli"

cbuffer ObjectCBuf
{
    float specularPower;
    bool hasGloss;
    float specularMapWeight;
};

Texture2D tex : register(t0);
Texture2D spec : register(t1);
SamplerState splr;


float4 main(float3 viewFragPos : Position, float3 viewNormal : Normal, float2 tc : Texcoord) : SV_Target
{
    // normalize the mesh normal
    viewNormal = normalize(viewNormal);
	// fragment to light vector data
    const LightVectorData lv = CalculateLightVectorData(viewLightPos, viewFragPos);
    // specular parameters
    float specularPowerLoaded = specularPower;
    const float4 specularSample = spec.Sample(splr, tc);
    const float3 specularReflectionColor = specularSample.rgb * specularMapWeight;
    if (hasGloss)
    {
        specularPowerLoaded = pow(2.0f, specularSample.a * 13.0f);
    }
	// attenuation
    const float att = Attenuate(attConst, attLin, attQuad, lv.distToL);
	// diffuse light
    const float3 diffuse = Diffuse(diffuseColor, diffuseIntensity, att, lv.dirToL, viewNormal);
    // specular reflected
    const float3 specularReflected = Speculate(
        specularReflectionColor, 1.0f, viewNormal,
        lv.vToL, viewFragPos, att, specularPowerLoaded
    );
	// final color = attenuate diffuse & ambient by diffuse texture color and add specular reflected
    return float4(saturate((diffuse + ambient) * tex.Sample(splr, tc).rgb + specularReflected), 1.0f);
}