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

// 1. ��Ȱ��ȭ ��ƼŬ ó��
// GeometryShader ���� ������ ���� �� ����
// ��Ȱ��ȭ�� ��ƼŬ�� �������� ���ʸ�, ��½�Ʈ���� �����, �߰��� ���������� �����Ŵ

// 2. ��ƼŬ ������ ó��


struct GS_OUT
{
    float4 vPosition : SV_Position;
    float2 vUV : TEXCOORD;
    uint InstID : INSTANCE_ID;
};

// PointMesh�� ����ϱ� ������ �Է� ������ 1��
[maxvertexcount(6)]
void GS_MyParticle(point VS_OUT _in[1], inout TriangleStream<GS_OUT> _OutStream)
{
    

}



#endif