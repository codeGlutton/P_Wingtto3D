#ifndef _INSTANCING_COMMON
#define _INSTANCING_COMMON

#include "GlobalDefine.hlsli"

/* 매크로 */

#define MAX_SKELETAL_INSTANCING 500

/* VS 입력 구조체 */

struct InstanceStaticVB
{
    float4 position : POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    
    // 인스턴싱
    uint instancingID : SV_INSTANCEID; // 몇번쨰 인스턴싱인가
    matrix matWorld : INST;
};

struct InstanceSkeletalVB
{
    float4 position : POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float4 blendIndices : BLEND_INDICES;
    float4 blendWeights : BLEND_WEIGHT;
    
    // 인스턴싱
    uint instancingID : SV_InstanceID; // 몇번쨰 인스턴싱인가
    matrix matWorld : INST;
};

#endif