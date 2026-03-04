#include "ShaderCommon.hlsli"

Texture2D widgetResourceTB;

cbuffer M_WidgetTypeCB
{
    float isText;
};

cbuffer F_ScreenCB
{
    float2 screenSize;
};

UIPB VS(UIVB input)
{
    UIPB output;
    output.position = float4((input.position.x / screenSize.x) * 2.f - 1.f, -(input.position.y / screenSize.y) * 2.f + 1.f, 0.f, 1.f);
    output.uv = input.uv;
    output.tint = input.tint;
	
    return output;
}

float4 PS(UIPB input) : SV_TARGET
{
    if (isText > 0.5f)
    {
        float alpha = widgetResourceTB.Sample(linearWrapSS, input.uv).r * input.tint.a;
        return float4(input.tint.rgb, alpha);
    }
    else
    {
        return widgetResourceTB.Sample(linearWrapSS, input.uv) * input.tint;
    }
}
