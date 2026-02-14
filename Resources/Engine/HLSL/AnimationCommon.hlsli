#ifndef _ANIMATION_COMMON
#define _ANIMATION_COMMON

#include "GlobalDefine.hlsli"

/* 매크로 */

#define MAX_KEY_FRAME 2000

struct KeyFrameDesc
{
    int animIndex;
    
    uint curFrame;
    uint nextFrame;
    float ratio;                // 프레임 간 보간 알파
    
    float sumTime;
    float speed;
    float2 padding;
};

struct TweenFrameDesc
{
    float tweenDuration;
    float tweenRatio;           // 두 애니메이션 간 보간 알파
    float tweenSumTime;
    float padding;
    KeyFrameDesc curFrameDesc;
    KeyFrameDesc nextFrameDesc;
};

#endif