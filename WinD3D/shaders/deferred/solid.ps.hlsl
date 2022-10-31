struct PixelOutDeferred
{
    float4 diffuse : SV_Target0;
    float4 normal : SV_Target1;
    float4 position : SV_Target2;
    float4 specular : SV_Target3;
};

cbuffer CBuf : register(b1)
{
    float3 materialColor;
}

PixelOutDeferred main(float3 viewFragPos : Position)
{
    PixelOutDeferred _out;
    _out.diffuse = float4(materialColor, 1.0f);
    _out.normal = float4(0.0f, 0.0f, 0.0f, 0.0f);
    _out.position = float4(viewFragPos, 1.0f);
    _out.specular = float4(0.0f, 0.0f, 0.0f, 1.0f);
    return _out;
}