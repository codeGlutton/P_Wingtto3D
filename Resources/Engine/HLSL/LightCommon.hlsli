#ifndef _LIGHT_COMMON
#define _LIGHT_COMMON

#include "GlobalDefine.hlsli"
#include "ShaderCommon.hlsli"

/* 구조체 */

struct DirectionLightDesc
{
    float4 ambient;
    float4 diffuse;
    float4 specular;
    float4 emissive;
    
    float3 dir;
    float padding;
};

struct MeshMaterialDesc
{
    float4 ambient;
    float4 diffuse;
    float4 specular;
    float4 emissive;
};

/* 상수 버퍼 */

cbuffer G_LightCB
{
    DirectionLightDesc globalLight;
};

cbuffer M_MaterialCB
{
    MeshMaterialDesc material;
};

/* SRV */

Texture2D diffuseTB : register(REG_TB_DIFFUSE);
Texture2D specularTB : register(REG_TB_SPECULAR);
Texture2D normalTB : register(REG_TB_NORMAL);

/* 함수 */

float4 ComputePhongAlbedo(float3 normal, float2 uv, float3 wpos)
{
    float4 albedo = diffuseTB.Sample(linearWrapSS, uv);
    
    float3 reflectLight = globalLight.dir + (2 * normal * dot(-globalLight.dir, normal));
    float3 posToCamera = normalize(cameraPosition - wpos.rgb);
    
    // ambient
    float3 ambient = globalLight.ambient * material.ambient * albedo;
    // diffuse (Lambert 공식)
    float3 diffuse = max(dot(-globalLight.dir, normal), 0.f) * globalLight.diffuse * material.diffuse * albedo;
    // specular (Phong 공식)
    float3 specular = pow(saturate(dot(reflectLight, posToCamera)), 20) * globalLight.specular * material.specular;
    
    // 측면에서 1.f 정면에서 0.f
    float3 sideFactor = 1.f - saturate(dot(posToCamera, normal));
    float3 emissive = globalLight.emissive.rgb * material.emissive.rgb * smoothstep(0.f, 1.f, sideFactor); // 계산 값으로 min과 max 사이의 Hermit 그래프 값 출력
    emissive = pow(emissive, 3.f);
    
    albedo.rgb = ambient + diffuse + specular + emissive;
    return albedo;
}

void ComputeNormalFromMap(inout float3 normal, float3 tangent, float2 uv)
{
    // rgb [0, 1] 사이값
    float3 tnormal = normalTB.Sample(linearWrapSS, uv);

    float3x3 matTangentToWorld = float3x3(tangent, cross(normal, tangent), normal);
    normal = normalize(mul(tnormal * 2.f - 1.f, matTangentToWorld));
}

#endif