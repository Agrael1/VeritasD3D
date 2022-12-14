#include "../headers/deferred.hlsli"
#include "../headers/ShaderProcs.hlsli"

cbuffer ObjectCBuf : register(b1)
{
    bool useGlossAlpha;
    bool useSpecularMap;
    float3 specularColor;
    float specularWeight;
    float specularGloss;
    bool useNormalMap;
    float normalMapWeight;
};

SamplerState splr;
Texture2D tex : register(t0);
Texture2D spec : register(t1);
Texture2D nmap : register(t2);
Texture2D hmap : register(t3);

float2 CalculatePOMUVOffset(float2 parallaxOffset, float2 uv, int numSteps)
{
    float currentHeight = 0.0f;
    float stepSize = 1.0f / (float) numSteps;
    float prevHeight = 1.0f;
    float nextHeight = 0.0f;
    int stepIndex = 0;

    float2 texOffsetPerStep = stepSize * parallaxOffset;
    float2 texCurrentOffset = uv;
    float currentBound = 1.0;
    float parallaxAmount = 0.0;

    float2 pt1 = 0;
    float2 pt2 = 0;
    float2 dx = ddx(uv);
    float2 dy = ddy(uv);
    
    while (stepIndex < numSteps)
    {
        texCurrentOffset -= texOffsetPerStep;
        currentHeight = hmap.SampleGrad(splr, texCurrentOffset, dx, dy).r;

        currentBound -= stepSize;

        if (currentHeight > currentBound)
        {
            pt1 = float2(currentBound, currentHeight); // point from current height
            pt2 = float2(currentBound + stepSize, prevHeight); // point from previous height
            
            stepIndex = numSteps + 1;
        }
        else
        {
            stepIndex++;
            prevHeight = currentHeight;
        }
    }
   
    //linear interpolation
    float delta2 = pt2.x - pt2.y;
    float delta1 = pt1.x - pt1.y;
    float diff = delta2 - delta1;
      
    if (diff == 0.0f)
        return uv;
    
    parallaxAmount = (pt1.x * delta2 - pt2.x * delta1) / diff;
   
    float2 vParallaxOffset = parallaxOffset * (1.0 - parallaxAmount);
    return uv - vParallaxOffset;
}
int GetPOMRayStepsCount(float3 pos, float3 normal)
{
    const int minLayers = 0;
    const int maxLayers = 24;
    
    float angle = dot(normalize(pos), normal);
    int numLayers = (int) lerp(maxLayers, minLayers, angle);
    return numLayers;
}

PixelOutDeferred main(float3 viewFragPos : Position, float3 viewNormal : Normal, float3 viewTan : Tangent, float3 viewBitan : Bitangent, float2 tc : Texcoord, float4 shadowPos : ShadowPosition)
{
    viewNormal = normalize(viewNormal);
    float3x3 TBN = float3x3(normalize(viewTan), normalize(viewBitan), viewNormal);
    float3 TangentViewDir = normalize(mul(TBN, viewFragPos));

    float2 vParallaxOffset = -TangentViewDir.xy * 0.03;
    
    int steps = GetPOMRayStepsCount(viewFragPos, viewNormal);
    
    if (steps)
        tc = CalculatePOMUVOffset(vParallaxOffset, tc, steps);
    
    if (useNormalMap)
    {
        const float3 mappedNormal = MapNormal(TBN, tc, nmap, splr);
        viewNormal = lerp(viewNormal, mappedNormal, normalMapWeight);
    }
    
    float4 spec_sample = spec.Sample(splr, tc);
    float3 spec_refl = useSpecularMap ? spec_sample.rgb : specularColor;
    float spec_pow = useGlossAlpha ? pow(2.0f, spec_sample.a * 13.0f) : specularGloss;
    
    PixelOutDeferred pOut;
    pOut.diffuse = tex.Sample(splr, tc);
    pOut.normal = float4(normalize(viewNormal), 1.0f);
    pOut.position = float4(viewFragPos, 1.0f);
    pOut.specular = float4(spec_refl * specularWeight, spec_pow);
    pOut.shadow = shadowPos;
    return pOut;
}