#ifndef _SHADER_COMMON
#define _SHADER_COMMON

#include "GlobalDefine.hlsli"

/* 매크로 */

#define MAX_BONE 250

/* VS 입력 구조체 */

struct UIVB
{
    float4 position : POSITION;
    float2 uv : UV;
    float4 tint : TINT;
};

struct StaticVB
{
    float4 position : POSITION;
    float2 uv : UV;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
};

struct SkeletalVB
{
    float4 position : POSITION;
    float2 uv : UV;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float4 blendIndices : BLEND_INDICES;
    float4 blendWeights : BLEND_WEIGHT;
};

/* PS 입력 구조체 */

struct UIPB
{
    float4 position : SV_POSITION;
    float2 uv : UV;
    float4 tint : TINT;
};

struct StaticPB
{
    float4 position : SV_POSITION;
    float2 uv : UV;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
};

struct SkeletalPB
{
    float4 position : SV_POSITION;
    float4 worldPosition : POSITION1;
    float2 uv : UV;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
};

/* 상수 버퍼 */

cbuffer F_CameraCB
{
    matrix matView;
    matrix matProj;
    matrix matVP;
    
    float3 cameraPosition;
};

cbuffer M_TransformCB
{
    matrix matWorld;
    matrix matWV;
    matrix matWVP;
};

cbuffer M_BoneCB
{
    matrix matBone[MAX_BONE];
};

/* 샘플러 */

SamplerState pointWrapSS;
SamplerState linearWrapSS;

#endif
