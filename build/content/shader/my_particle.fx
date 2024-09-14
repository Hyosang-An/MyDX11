#ifndef _MY_PARTICLE
#define _MY_PARTICLE

#include "value.fx"
#include "struct.fx"
#include "func.fx"

StructuredBuffer<tMyParticle> ParticleBuffer : register(t20);

struct VS_IN
{
    float3 vPos : POSITION;
    uint InstID : SV_InstanceID;
};

struct VS_OUT
{
    float3 vLocalPos : POSITION;
    uint InstID : INSTANCE_ID;
};

VS_OUT VS_MyParticle(VS_IN _in)
{
    VS_OUT output = (VS_OUT) 0.f;
    
    output.vLocalPos = _in.vPos;
    output.InstID = _in.InstID;
    
    return output;
}

// 1. 비활성화 파티클 처리
// GeometryShader 에서 정점을 생성 및 삭제
// 비활성화된 파티클을 렌더링할 차례면, 출력스트림을 비워서, 중간에 파이프라인 종료시킴

// 2. 파티클 빌보드 처리


struct GS_OUT
{
    float4 vPosition : SV_Position;
    float2 vUV : TEXCOORD;
    uint InstID : INSTANCE_ID;
};

// PointMesh를 사용하기 때문에 입력 정점은 1개
[maxvertexcount(6)]
void GS_MyParticle(point VS_OUT _in[1], inout TriangleStream<GS_OUT> _OutStream)
{
    

}



#endif