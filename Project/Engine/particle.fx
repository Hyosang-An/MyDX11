#ifndef _PARTICLE
#define _PARTICLE

#include "value.fx"
#include "struct.fx"

StructuredBuffer<tParticle> ParticleBuffer : register(t20);




struct VS_IN
{
    float3 vPos : POSITION;
    float2 vUV : TEXCOORD;
    
    uint InstID : SV_InstanceID;
};


struct VS_OUT
{
    float4 vPosition : SV_Position;
    float2 vUV : TEXCOORD;
};


VS_OUT VS_Particle(VS_IN _in)
{
    VS_OUT output = (VS_OUT) 0.f;
    
    // 크기 -> 이동
    float3 vWorldPos = _in.vPos * float3(10.f, 10.f, 1.f) + ParticleBuffer[_in.InstID].vWorldPos;
        
    output.vPosition = mul(mul(float4(vWorldPos, 1.f), matView), matProj);
    output.vUV = _in.vUV;
    
    return output;
}

float4 PS_Particle(VS_OUT _in) : SV_Target
{
    return float4(1.f, 0.f, 0.f, 1.f);
}




#endif