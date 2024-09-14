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
    // 최대 파티클 개수보다 큰 ID는 처리하지 않음
    if (ThreadID.x >= MAX_COUNT)
        return;
    
    // spawn count 의 현재값을 읽어 옴
    int current_spawn_count = SpawnCountBuffer[0].iSpawnCount;
    
    float z_Max = 200.f;
    
    // 파티클이 활성화 되지 않은 경우
    if (!Particle.Active)
    {
        // current_spawn_count가 0보다 크면 활성화
        
        while (current_spawn_count > 0)
        {
            int originalCount;
            
            // SpawnCountBuffer[0].iSpawnCount 값을 원자적으로 1 감소시키고, 이전 값을 originalCount에 저장
            InterlockedAdd(SpawnCountBuffer[0].iSpawnCount, -1, originalCount);
            
            // originalCount가 0보다 크면 해당 파티클을 활성화하고 루프 종료
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
                
                
                // Particle 초기화
                Particle.vWorldInitPos = Particle.vWorldPos = spawnPos;
                Particle.Age = 0.f;
                Particle.NormalizedAge = 0.f;
                Particle.Life = 8.f;
                
                Particle.RandomValue = vRandom1.x;
                
                // 파티클 색은 spawnPos의 z값이 0에 가까울수록 흰색, z_Max에 가까울수록 회색
                float3 vColor = lerp(float3(1, 1, 1), float3(0.5, 0.5, 0.5), spawnPos.z / z_Max);
                Particle.vColor = float4(vColor, 1.f);
                
                // 파티클 속도의 y값은 spawnPos의 z값이 클수록 작은 주기의 cos 그래프 형태로 변화, 속도의 x값은 spawnPos의 z값이 클수록 작도록 초기화
                float3 velocity = float3(0.f, 0.f, 0.f);
                velocity.x = 50 + 150 * (1 - spawnPos.z / z_Max);
                
                // 주기는 200 ~ 1000 사이에서 변화, 속도의 크기는 200 ~ 720 사이에서 변화
                velocity.y = cos((g_EngineTime + Particle.RandomValue) * 2 * PI / ((1 - Particle.vWorldInitPos.z / z_Max) * 800 + 200)) * (1 - Particle.vWorldInitPos.z / z_Max) * 520 + 200;
                Particle.vVelocity = velocity;
                
                
                break;
            }
            
            // 다른 스레드가 값을 변경했을 수 있으므로 다시 읽어옴
            current_spawn_count = SpawnCountBuffer[0].iSpawnCount;
        }
    }
    
    // 이미 파티클이 활성화 되어 있는 경우
    else
    {
        // 파티클 업데이트

        // y속도 업데이트
        Particle.vVelocity.y = cos((g_EngineTime + Particle.RandomValue) * 2 * PI / ((1 - Particle.vWorldInitPos.z / z_Max) * 800 + 200)) * (1 - Particle.vWorldInitPos.z / z_Max) * 520 + 200;
        
        // 위치 업데이트
        Particle.vWorldPos += Particle.vVelocity * g_EngineDT;
        
        Particle.Age += g_EngineDT;
        Particle.NormalizedAge = Particle.Age / Particle.Life;
        // 파티클이 수명을 다한 경우 비활성화
        if (Particle.Age >= Particle.Life)
        {
            Particle.Active = false;
        }
    }
    
    
    
}

#endif // _MY_PARTICLE_TICK