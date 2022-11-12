cbuffer cbPerObject
{
	float4x4 gModel;
	float4x4 gView;
	float4x4 gProjection;
};
struct VertexOut
{
	float4 hPos		: HPOS;
	float4 position : SV_POSITION;
	float3 normal	: NORMAL;
	float2 tex		: TEXCOORD0;
};

struct PixelOutDeferred
{
	float4 diffuse  : SV_Target0;
	float4 normal   : SV_Target1;
	float4 position : SV_Target2;
};

PixelOutDeferred main(VertexOut pIn)
{
	PixelOutDeferred pOut;
	pOut.diffuse = float4(1.0f, 1.0f, 1.0f, 1.0f);
	pOut.normal = float4(pIn.normal, 1.0f);
	pOut.position = mul(pIn.hPos, gModel);
	return pOut;
}