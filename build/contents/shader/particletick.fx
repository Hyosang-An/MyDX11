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
    // 최대 파티클 개수보다 큰 ID는 작업을 수행하지 않음
    if (MAX_COUNT <= ThreadID.x)
        return;
    
    // 스폰 카운트의 현재 값을 읽어옴
    int currentSpawnCount = SpawnCountBuffer[0].iSpawnCount;
    
    // 파티클이 활성화되지 않은 경우
    if (!Particle.Active)
    {
        // 스폰 카운트가 0보다 큰 동안 반복
        while (currentSpawnCount > 0)
        {
            int originalCount = 0;
            int decrementedCount = currentSpawnCount - 1;
                        
            // InterlockedCompareExchange를 사용해 스폰 카운트를 원자적으로 감소
            // 다른 스레드와 경합하여 값을 업데이트
            InterlockedCompareExchange(
                SpawnCountBuffer[0].iSpawnCount,
                currentSpawnCount,
                decrementedCount,
                originalCount
            );

            // 만약 currentSpawnCount가 변경 전의 값과 일치하면 파티클을 활성화하고 루프 종료
            if (currentSpawnCount == originalCount)
            {
                float2 vUV = (float2) 0.f;
                
                // 스레드를 UV 로 맵핑하기위해서 ID 를 0~1 범위로 정규화
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
            
            // 다른 스레드가 값을 변경했을 수 있으므로 현재 스폰 카운트를 업데이트
            currentSpawnCount = SpawnCountBuffer[0].iSpawnCount;
        }
    }
    else
    {
        // 파티클이 활성화된 경우 위치 업데이트
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
//    // 최대 파티클 개수보다 큰 ID는 작업을 수행하지 않음
//    if (MAX_COUNT <= ThreadID.x)
//        return;

//    // 파티클이 활성화되지 않은 경우
//    if (!Particle.Active)
//    {
//        int currentSpawnCount = SpawnCountBuffer[0].iSpawnCount;

//        while (currentSpawnCount > 0)
//        {
//            int originalCount;

//            // SpawnCountBuffer[0].iSpawnCount 값을 원자적으로 1 감소시키고, 이전 값을 originalCount에 저장
//            InterlockedAdd(SpawnCountBuffer[0].iSpawnCount, -1, originalCount);

//            // originalCount가 0보다 큰 경우 파티클을 활성화하고 루프 종료
//            if (originalCount > 0)
//            {
//                Particle.Active = true;
                
//                float2 vUV = (float2) 0.f;
                
//                // 스레드를 UV 로 맵핑하기위해서 ID 를 0~1 범위로 정규화
//                vUV.x = (ThreadID.x / (MAX_COUNT - 1)) + g_EngineTime;
//                vUV.y = sin(vUV.x);
                
//                break;
//            }

//            // 다른 스레드가 값을 변경했을 수 있으므로 현재 스폰 카운트를 업데이트
//            currentSpawnCount = SpawnCountBuffer[0].iSpawnCount;
//        }
//    }
//    else
//    {
//        // 파티클이 활성화된 경우 위치 업데이트
//        Particle.vWorldPos += Particle.vVelocity * g_EngineDT;
//    }
//}



// 1. 파티클 렌더링, 비활성화 파티클 처리 + 빌보드 (완)

// 2. 파티클 스레드 동기화

// 3. 파티클 CS 에서 랜덤.



#endif