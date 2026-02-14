#include "ShaderCommon.hlsli"

Texture2D widgetResourceTB;

UIPB VS(UIVB input)
{
    UIPB output;
    output.position = input.position;
    output.uv = input.uv;
    output.tint = input.tint;
	
    return output;
}

float4 PS(UIPB input) : SV_TARGET
{
    return widgetResourceTB.Sample(linearWrapSS, input.uv) * input.tint;
}