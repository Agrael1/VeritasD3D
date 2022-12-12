#include "../headers/ShaderProcs.hlsli"
#include "../headers/LightVectorData.hlsli"
#include "../headers/lights.hlsli"
#include "../headers/shadow.hlsli"

Texture2D diffuse;
Texture2D normal;
Texture2D position;
Texture2D specular;
Texture2D shadow;

SamplerState splr;

struct Sout
{
    float3 diff;
    float3 spec;
};

Sout Shade(in PointLight l,
    in const float3 view_normal,
    in const float3 view_pos,
    in const float4 specular
)
{
    Sout _out;
    _out.diff = float3(0, 0, 0);
    _out.spec = float3(0, 0, 0);
    if (!l.active)
        return _out;
    
    const LightVectorData lv = CalculateLightVectorData(l.viewLightPos.xyz, view_pos);
    
	    // attenuation
    const float att = Attenuate(l.attConst, l.attLin, l.attQuad, lv.distToL);
        
    const float3 dc = l.diffuseColor * l.diffuseIntensity;
    const float dot1 = dot(lv.dirToL, view_normal);
        
    [flatten]
    if (dot1 > 0.0f)
    {
        const float3 diffuse = dc * att * dot1;
        
        // calculate reflected light vector
        const float3 r = normalize(view_normal * dot1 * 2.0f - lv.dirToL);
        
        // vector from camera to fragment (in view space)
        const float3 viewCamToFrag = normalize(view_pos);
        
        // calculate specular component color based on angle between
        // viewing vector and reflection vector, narrow with power function
        const float3 specularReflected =
                att * specular.xyz * pow(max(0.0f, dot(-r, viewCamToFrag)), specular.w);
        _out.diff = diffuse;
        _out.spec = specularReflected;
        
        return _out;
    }
    return _out;
}

[earlydepthstencil]
float4 main(float2 tc : Texcoord) : SV_Target
{
    const float3 diffuse_col = diffuse.Sample(splr, tc).xyz;
    const float4 normal_samle = normal.Sample(splr, tc);
    
    // solid objects
    [flatten]
    if (normal_samle.w == 0.0)
        return float4(diffuse_col, 1.0f);
    
    const float3 view_normal = normal_samle.xyz;
    
    const float3 view_pos = position.Sample(splr, tc).xyz;
    const float4 specular_sample = specular.Sample(splr, tc);
    const float3 specular_color = specular_sample.xyz;
    const float specular_gloss = specular_sample.w;
    const float shadowLevel = Shadow(shadow.Sample(splr, tc));
     
    float3 final_color = 0.0f;
    float3 final_spec = 0.0f;
    
    if (shadowLevel != 0.0f)
    {
        Sout s = Shade(lights[0], view_normal, view_pos, specular_sample);
        final_color += s.diff;
        final_spec += s.spec;
    }
    final_color *= shadowLevel;
    final_spec *= shadowLevel;
    
    for (uint i = 1; i < count; i++)
    {
        Sout s = Shade(lights[i], view_normal, view_pos, specular_sample);
        
        final_color += s.diff;
        final_spec += s.spec;
    }
        
	// final color = attenuate diffuse & ambient by diffuse texture color and add specular reflected
    return float4(saturate((final_color + float3(.3, .3, .3)) * diffuse_col.rgb + final_spec), 1.0f);
}