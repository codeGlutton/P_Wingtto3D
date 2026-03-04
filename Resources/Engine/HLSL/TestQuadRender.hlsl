#include "ShaderCommon.hlsli"

Texture2D widgetResourceTB;

float4 VS(uint id : SV_VertexID) : SV_POSITION
{
    float2 QuadTest[3] =
    {
        float2(-1.0, 1.0),
        float2(1.0, 1.0),
        float2(-1.0, -1.0)
    };
    return float4(QuadTest[id], 0.5f, 1.f);
}

float4 PS(float4 pos : SV_POSITION) : SV_TARGET
{
    return float4(1.f, 0.f, 0.f, 1.f);
}
