#include "ShaderProcs.hlsli"
#include "LightVectorData.hlsli"
#include "lights.hlsli"

Texture2D diffuse;
Texture2D normal;
Texture2D position;
Texture2D specular;

SamplerState splr;


float4 main(float2 tc : Texcoord) : SV_Target
{
    float3 view_pos = position.Sample(splr, tc).xyz;
    
    float3 diffuse_col = diffuse.Sample(splr, tc).xyz;
    return diffuse.Sample(splr, tc);
    float3 view_normal = normal.Sample(splr, tc).xyz;
    float4 specular_sample = specular.Sample(splr, tc);
    float3 specular_color = specular_sample.xyz;
    float specular_gloss = specular_sample.w;
    
    float3 final_color = 0.0f;
    float3 final_spec = 0.0f;
    for (uint i = 0; i < count; i++)
    {
        PointLight l = lights[i];
        
        const LightVectorData lv = CalculateLightVectorData(mul(l.viewLightPos, view).xyz, view_pos);
    
	    // attenuation
        const float att = Attenuate(l.attConst, l.attLin, l.attQuad, lv.distToL);
        
        const float3 dc = l.diffuseColor * l.diffuseIntensity;
        const float3 dot1 = dot(lv.dirToL, view_normal);
        const float3 diffuse = dc * att * max(0.0f, dot1);
        
        // calculate reflected light vector
        const float3 r = normalize(view_normal * dot1 * 2.0f - lv.dirToL);
        
        // vector from camera to fragment (in view space)
        const float3 viewCamToFrag = normalize(view_pos);
        
        // calculate specular component color based on angle between
        // viewing vector and reflection vector, narrow with power function
        const float3 specularReflected =
        att * specular_color * pow(max(0.0f, dot(-r, viewCamToFrag)), specular_gloss); 
        final_spec += specularReflected;
        final_color += diffuse;
    }
    
        
	// final color = attenuate diffuse & ambient by diffuse texture color and add specular reflected
    return float4(saturate((final_color + float3(.1, .1, .1)) * diffuse_col.rgb + final_spec), 1.0f);
}