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
    GS_OUT output[4] = { (GS_OUT) 0.f, (GS_OUT) 0.f, (GS_OUT) 0.f, (GS_OUT) 0.f };
    
    const tMyParticle Particle = ParticleBuffer[_in[0].InstID]; // ParticleBuffer�� SRV�� ���̵� �Ǿ��ֱ� ������ const�� �����ϴ°� ����
    
    if (Particle.Active == false)
    {
        return;
    }
    
    //  0 -- 1
    //  | \  |
    //  3 -- 2
    
    float3 scale = Particle.vWorldCurrentScale;
    
    // view space������ ���� ��ġ
    float3 viewPos = mul(float4(Particle.vWorldPos, 1.f), matView);

    // �����带 �����ϴ� 4���� ������ ����
    output[0].vPosition.xyz = viewPos + float3(-scale.x / 2.f, scale.y / 2.f, 0.f);
    output[1].vPosition.xyz = viewPos + float3(scale.x / 2.f, scale.y / 2.f, 0.f);
    output[2].vPosition.xyz = viewPos + float3(scale.x / 2.f, -scale.y / 2.f, 0.f);
    output[3].vPosition.xyz = viewPos + float3(-scale.x / 2.f, -scale.y / 2.f, 0.f);
    
    // �� output ������ projection space�� ��ȯ
    for (int i = 0; i < 4; ++i)
    {
        output[i].vPosition.w = 1.f;
        output[i].vPosition = mul(output[i].vPosition, matProj);
        output[i].InstID = _in[0].InstID;
    }
    
    // UV ����
    output[0].vUV = float2(0.f, 0.f);
    output[1].vUV = float2(1.f, 0.f);
    output[2].vUV = float2(1.f, 1.f);
    output[3].vUV = float2(0.f, 1.f);
    
    
    // Stream ���
    _OutStream.Append(output[0]);
    _OutStream.Append(output[2]);
    _OutStream.Append(output[3]);
    _OutStream.RestartStrip();
    
    _OutStream.Append(output[0]);
    _OutStream.Append(output[1]);
    _OutStream.Append(output[2]);
    _OutStream.RestartStrip();
}


float4 PS_MyParticle(GS_OUT _in) : SV_TARGET
{
    const tMyParticle Particle = ParticleBuffer[_in.InstID];
    
    float4 vParticleColor = float4(1.f, 0.f, 0.f, 1.f);
    
    if (g_btex_0)
    {
        vParticleColor = g_tex_0.Sample(g_sam_0, _in.vUV);
    }
    
    else
    {
        vParticleColor = Particle.vColor;
    }
    
    return vParticleColor;
}


#endif