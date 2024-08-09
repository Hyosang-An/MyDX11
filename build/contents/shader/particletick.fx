#ifndef _PARTICLE_TICK
#define _PARTICLE_TICK

#include "value.fx"
#include "struct.fx"


RWStructuredBuffer<tParticle>   ParticleBuffer : register(u0);
RWStructuredBuffer<tSpawnCount> SpawnCountBuffer : register(u1);
Texture2D                       NoiseTex : register(t20);

#define ParticleObjectPos   g_vec4_0.xyz
#define MAX_COUNT           g_int_0
#define Particle            ParticleBuffer[ThreadID.x]

[numthreads(1024, 1, 1)]
void CS_ParticleTick(int3 ThreadID : SV_DispatchThreadID)
{
    // �ִ� ��ƼŬ �������� ū ID�� �۾��� �������� ����
    if (MAX_COUNT <= ThreadID.x)
        return;
    
    // ���� ī��Ʈ�� ���� ���� �о��
    int currentSpawnCount = SpawnCountBuffer[0].iSpawnCount;
    
    // ��ƼŬ�� Ȱ��ȭ���� ���� ���
    if (!Particle.Active)
    {
        // ���� ī��Ʈ�� 0���� ū ���� �ݺ�
        while (currentSpawnCount > 0)
        {
            int originalCount = 0;
            int decrementedCount = currentSpawnCount - 1;
                        
            // InterlockedCompareExchange�� ����� ���� ī��Ʈ�� ���������� ����
            // �ٸ� ������� �����Ͽ� ���� ������Ʈ
            InterlockedCompareExchange(
                SpawnCountBuffer[0].iSpawnCount,
                currentSpawnCount,
                decrementedCount,
                originalCount
            );

            // ���� currentSpawnCount�� ���� ���� ���� ��ġ�ϸ� ��ƼŬ�� Ȱ��ȭ�ϰ� ���� ����
            if (currentSpawnCount == originalCount)
            {
                float2 vUV = (float2) 0.f;
                
                // �����带 UV �� �����ϱ����ؼ� ID �� 0~1 ������ ����ȭ
                vUV.x = ((float) ThreadID.x / (float) (MAX_COUNT - 1)) + g_EngineTime * 0.5f;
                vUV.y = sin(vUV.x * 20 * PI) * 0.5f + g_EngineTime * 0.1f;
                                
                float3 vNoise = NoiseTex.SampleLevel(g_sam_1, vUV, 0).xyz;
                
                float BoxScale = 300.f;
                
                float3 vRandomPos = (float3) 0.f;
                
                vRandomPos.x = vNoise.x * BoxScale - (BoxScale / 2.f);
                vRandomPos.y = vNoise.y * BoxScale - (BoxScale / 2.f);
                vRandomPos.z = vNoise.z * BoxScale - (BoxScale / 2.f);
                
                Particle.vLocalPos = vRandomPos;
                Particle.vWorldPos = Particle.vLocalPos + ParticleObjectPos;
                Particle.Age = 0.f;
                Particle.Active = true;
                //Particle.Life = 2.f;
                //Particle.NormalizedAge = 0.f;
                
                break;
            }
            
            // �ٸ� �����尡 ���� �������� �� �����Ƿ� ���� ���� ī��Ʈ�� ������Ʈ
            currentSpawnCount = SpawnCountBuffer[0].iSpawnCount;
        }
    }
    else
    {
        // ��ƼŬ�� Ȱ��ȭ�� ��� ��ġ ������Ʈ
        Particle.vWorldPos += Particle.vVelocity * g_EngineDT;
        
        Particle.Age += g_EngineDT;
        Particle.NormalizedAge = Particle.Age / Particle.Life;
        if (Particle.Life <= Particle.Age)
        {
            Particle.Active = false;
        }
    }
}

//[numthreads(1024, 1, 1)]
//void CS_ParticleTick(int3 ThreadID : SV_DispatchThreadID)
//{
//    // �ִ� ��ƼŬ �������� ū ID�� �۾��� �������� ����
//    if (MAX_COUNT <= ThreadID.x)
//        return;

//    // ��ƼŬ�� Ȱ��ȭ���� ���� ���
//    if (!Particle.Active)
//    {
//        int currentSpawnCount = SpawnCountBuffer[0].iSpawnCount;

//        while (currentSpawnCount > 0)
//        {
//            int originalCount;

//            // SpawnCountBuffer[0].iSpawnCount ���� ���������� 1 ���ҽ�Ű��, ���� ���� originalCount�� ����
//            InterlockedAdd(SpawnCountBuffer[0].iSpawnCount, -1, originalCount);

//            // originalCount�� 0���� ū ��� ��ƼŬ�� Ȱ��ȭ�ϰ� ���� ����
//            if (originalCount > 0)
//            {
//                Particle.Active = true;
                
//                float2 vUV = (float2) 0.f;
                
//                // �����带 UV �� �����ϱ����ؼ� ID �� 0~1 ������ ����ȭ
//                vUV.x = (ThreadID.x / (MAX_COUNT - 1)) + g_EngineTime;
//                vUV.y = sin(vUV.x);
                
//                break;
//            }

//            // �ٸ� �����尡 ���� �������� �� �����Ƿ� ���� ���� ī��Ʈ�� ������Ʈ
//            currentSpawnCount = SpawnCountBuffer[0].iSpawnCount;
//        }
//    }
//    else
//    {
//        // ��ƼŬ�� Ȱ��ȭ�� ��� ��ġ ������Ʈ
//        Particle.vWorldPos += Particle.vVelocity * g_EngineDT;
//    }
//}



// 1. ��ƼŬ ������, ��Ȱ��ȭ ��ƼŬ ó�� + ������ (��)

// 2. ��ƼŬ ������ ����ȭ

// 3. ��ƼŬ CS ���� ����.



#endif