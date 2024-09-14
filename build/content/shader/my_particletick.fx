#ifndef _MY_PARTICLE_TICK
#define _MY_PARTICLE_TICK

#include "value.fx"
#include "struct.fx"
#include "func.fx"

RWStructuredBuffer<tMyParticle> ParticleBuffer : register(u0);
RWStructuredBuffer<tMySpawnCount> SpawnCountBuffer : register(u1);
Texture2D NoiseTex : register(t20);

#define ParticleObjectPos   g_vec4_0.xyz
#define MAX_COUNT           g_int_0
#define Particle            ParticleBuffer[ThreadID.x]

[numthreads(1024, 1, 1)]
void CS_MyParticleTick(int3 ThreadID : SV_DispatchThreadID)
{
    // �ִ� ��ƼŬ �������� ū ID�� ó������ ����
    if (ThreadID.x >= MAX_COUNT)
        return;
    
    // spawn count �� ���簪�� �о� ��
    int current_spawn_count = SpawnCountBuffer[0].iSpawnCount;
    
    float z_Max = 200.f;
    
    // ��ƼŬ�� Ȱ��ȭ ���� ���� ���
    if (!Particle.Active)
    {
        // current_spawn_count�� 0���� ũ�� Ȱ��ȭ
        
        while (current_spawn_count > 0)
        {
            int originalCount;
            
            // SpawnCountBuffer[0].iSpawnCount ���� ���������� 1 ���ҽ�Ű��, ���� ���� originalCount�� ����
            InterlockedAdd(SpawnCountBuffer[0].iSpawnCount, -1, originalCount);
            
            // originalCount�� 0���� ũ�� �ش� ��ƼŬ�� Ȱ��ȭ�ϰ� ���� ����
            if (originalCount > 0)
            {
                Particle.Active = true;
                
                float3 vRandom0 = GetRandomFloat3(ThreadID.x);
                float3 vRandom1 = GetRandomFloat3(MAX_COUNT + ThreadID.x);
                // float3 vRandom2 = GetRandomFloat3(2 * MAX_COUNT + ThreadID.x);
                
                
                float3 spawnRange = float3(500, 500, z_Max);
                spawnRange.x = vRandom0.x * spawnRange.x;
                spawnRange.y = (-0.5f + vRandom0.y) * spawnRange.y;
                spawnRange.z = vRandom0.z * spawnRange.z;
                float3 spawnPos = ParticleObjectPos + spawnRange;
                
                
                // Particle �ʱ�ȭ
                Particle.vWorldInitPos = Particle.vWorldPos = spawnPos;
                Particle.Age = 0.f;
                Particle.NormalizedAge = 0.f;
                Particle.Life = 8.f;
                
                Particle.RandomValue = vRandom1.x;
                
                // ��ƼŬ ���� spawnPos�� z���� 0�� �������� ���, z_Max�� �������� ȸ��
                float3 vColor = lerp(float3(1, 1, 1), float3(0.5, 0.5, 0.5), spawnPos.z / z_Max);
                Particle.vColor = float4(vColor, 1.f);
                
                // ��ƼŬ �ӵ��� y���� spawnPos�� z���� Ŭ���� ���� �ֱ��� cos �׷��� ���·� ��ȭ, �ӵ��� x���� spawnPos�� z���� Ŭ���� �۵��� �ʱ�ȭ
                float3 velocity = float3(0.f, 0.f, 0.f);
                velocity.x = 50 + 150 * (1 - spawnPos.z / z_Max);
                
                // �ֱ�� 200 ~ 1000 ���̿��� ��ȭ, �ӵ��� ũ��� 200 ~ 720 ���̿��� ��ȭ
                velocity.y = cos((g_EngineTime + Particle.RandomValue) * 2 * PI / ((1 - Particle.vWorldInitPos.z / z_Max) * 800 + 200)) * (1 - Particle.vWorldInitPos.z / z_Max) * 520 + 200;
                Particle.vVelocity = velocity;
                
                
                break;
            }
            
            // �ٸ� �����尡 ���� �������� �� �����Ƿ� �ٽ� �о��
            current_spawn_count = SpawnCountBuffer[0].iSpawnCount;
        }
    }
    
    // �̹� ��ƼŬ�� Ȱ��ȭ �Ǿ� �ִ� ���
    else
    {
        // ��ƼŬ ������Ʈ

        // y�ӵ� ������Ʈ
        Particle.vVelocity.y = cos((g_EngineTime + Particle.RandomValue) * 2 * PI / ((1 - Particle.vWorldInitPos.z / z_Max) * 800 + 200)) * (1 - Particle.vWorldInitPos.z / z_Max) * 520 + 200;
        
        // ��ġ ������Ʈ
        Particle.vWorldPos += Particle.vVelocity * g_EngineDT;
        
        Particle.Age += g_EngineDT;
        Particle.NormalizedAge = Particle.Age / Particle.Life;
        // ��ƼŬ�� ������ ���� ��� ��Ȱ��ȭ
        if (Particle.Age >= Particle.Life)
        {
            Particle.Active = false;
        }
    }
    
    
    
}

#endif // _MY_PARTICLE_TICK