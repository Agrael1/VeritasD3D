struct PixelOutDeferred
{
    float4 diffuse : SV_Target0;
    float4 normal : SV_Target1;
    float4 position : SV_Target2;
    float4 specular : SV_Target3;
    float4 shadow : SV_Target4;
};