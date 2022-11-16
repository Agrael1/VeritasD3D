#include "../headers/deferred.hlsli"

cbuffer ObjectCBuf : register(b1)
{
	float3 materialColor;
	float3 specularColor;
	float specularWeight;
	float specularGloss;
};


PixelOutDeferred main(float3 viewFragPos : Position, float3 viewNormal : Normal, float4 shadowPos : ShadowPosition)
{
	PixelOutDeferred pOut;
	pOut.diffuse = float4(materialColor, 1.0f);
	pOut.normal = float4(normalize(viewNormal), 1.0f);
	pOut.position = float4(viewFragPos, 1.0f);
	pOut.specular = float4(specularColor * specularWeight, specularGloss);
    pOut.shadow = shadowPos;
	return pOut;
}