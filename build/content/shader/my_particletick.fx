#ifndef _MY_PARTICLE_TICK
#define _MY_PARTICLE_TICK

#include "value.fx"
#include "struct.fx"
#include "func.fx"

RWStructuredBuffer<tMyParticle> ParticleBuffer : register(u0);
RWStructuredBuffer<tMySpawnCount> SpawnCountBuffer : register(u1);
Texture2D NoiseTex : register(t20);
StructuredBuffer<tMyParticleModule> Module : register(t21);

#define ParticleObjectPos g_vec4_0.xyz
#define MAX_COUNT g_int_0
#define Particle ParticleBuffer[ThreadID.x]

#define NoiseForce Module[0].isModuleOn[5]
#define DragModule Module[0].isModuleOn[4]
#define Render     Module[0].isModuleOn[6]

[numthreads(1024, 1, 1)] void CS_MyParticleTick(int3 ThreadID : SV_DispatchThreadID)
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

                // 눈발 파티클
                if (Module[0].Type == 0)
                {
                    float3 spawnRange = float3(Module[0].SpawnShapeScale.xy, z_Max);
                    // float3 spawnRange = float3(500, 500, z_Max);
                    // spawnRange.x = vRandom0.x * spawnRange.x;
                    spawnRange.x = (-0.5f + vRandom0.x) * spawnRange.x;
                    spawnRange.y = (-0.5f + vRandom0.y) * spawnRange.y;
                    spawnRange.z = vRandom0.z * spawnRange.z;
                    float3 spawnPos = ParticleObjectPos + spawnRange;

                    // Particle 초기화
                    Particle.vWorldInitPos = Particle.vWorldPos = spawnPos;
                    Particle.vWorldCurrentScale = float3(10.f, 10.f, 10.f);
                    Particle.Age = 0.f;
                    Particle.NormalizedAge = 0.f;
                    Particle.Life = 8.f;

                    Particle.RandomValue = vRandom1.x;

                    // 파티클 색은 spawnPos의 z값이 0에 가까울수록 흰색, z_Max에 가까울수록 회색
                    float3 vColor = lerp(float3(1, 1, 1), float3(0.3f, 0.3f, 0.3f), spawnPos.z / z_Max);
                    Particle.vColor = float4(vColor, 1.f);

                    // 파티클 속도의 y값은 spawnPos의 z값이 클수록 작은 주기의 cos 그래프 형태로 변화, 속도의 x값은 spawnPos의 z값이 클수록 작도록 초기화
                    float3 velocity = float3(0.f, 0.f, 0.f);

                    // 속도의 x값은 600 ~ 1800 사이에서 변화
                    velocity.x = -(600 + 1200 * (1 - spawnPos.z / z_Max));

                    // 주기는 800 ~ 4000 사이에서 변화, 속도의 크기는 200 ~ 1000 사이에서 변화

                    // 주기
                    float L = (1 - Particle.vWorldInitPos.z / z_Max) * 3200 + 800;
                    float period = 2 * PI / ((1 - Particle.vWorldInitPos.z / z_Max) * 3200 + 800);

                    // 0 ~ 주기 사이의 랜덤값
                    float randomValue = Particle.RandomValue * period;
                    velocity.y = cos(Particle.RandomValue * L * period) * ((1 - Particle.vWorldInitPos.z / z_Max) * 800 + 200);
                    Particle.vVelocity = velocity;
                }

                // 대쉬 파티클
                else if (Module[0].Type == 1)
                {
                    float3 spawnRange = Module[0].SpawnShapeScale;
                    spawnRange.x = (-0.5f + vRandom0.x) * spawnRange.x;
                    spawnRange.y = (-0.5f + vRandom0.y) * spawnRange.y;
                    spawnRange.z = 0;
                    float3 spawnPos = ParticleObjectPos + spawnRange;

                    // Particle 초기화
                    Particle.vWorldInitPos = Particle.vWorldPos = spawnPos;
                    Particle.vWorldCurrentScale = float3(10.f, 10.f, 10.f);
                    Particle.Age = 0.f;
                    Particle.NormalizedAge = 0.f;
                    Particle.Life = 5.f;

                    // 파티클 색은 하늘색
                    Particle.vColor = float4(0.5f, 0.5f, 1.f, 1.f);

                    // 속도는 기준 방향을 기준으로 +- 30도 범위에서 랜덤
                    // 방향 벡터 정규화
                    float2 dir = normalize(Module[0].ReferenceDir.xy);

                    // 랜덤 각도 계산 (-30도에서 +30도 사이)
                    float theta = (-PI / 6.0) + (PI / 3.0) * vRandom1.x;

                    // 회전 변환 적용
                    float cosTheta = cos(theta);
                    float sinTheta = sin(theta);

                    float2 rotatedDir;
                    rotatedDir.x = dir.x * cosTheta - dir.y * sinTheta;
                    rotatedDir.y = dir.x * sinTheta + dir.y * cosTheta;

                    // 속도 크기 50 ~ 100 사이의 속도
                    float speed = vRandom1.y * 50 + 50;

                    Particle.vVelocity = float3(rotatedDir, 0) * speed;
                }

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

        // 눈발 파티클
        if (Module[0].Type == 0)
        {
            // y속도 업데이트
            // 주기
            float L = (1 - Particle.vWorldInitPos.z / z_Max) * 3200 + 800;
            float period = 2 * PI / ((1 - Particle.vWorldInitPos.z / z_Max) * 3200 + 800);
            // 0 ~ 주기 사이의 랜덤값
            float randomValue = Particle.RandomValue * period;
            float x_dist = abs(Particle.vWorldPos.x - Particle.vWorldInitPos.x);
            Particle.vVelocity.y = cos((x_dist + Particle.RandomValue * L) * period) * ((1 - Particle.vWorldInitPos.z / z_Max) * 800 + 200);

            // 위치 업데이트
            Particle.vWorldPos += Particle.vVelocity * g_EngineDT;

            Particle.Age += g_EngineDT;
            Particle.NormalizedAge = Particle.Age / Particle.Life;
        }

        // 대쉬 파티클
        else if (Module[0].Type == 1)
        {
            Particle.vForce = float3(0.f, 0.f, 0.f);

            // Noise Force Module
            if (NoiseForce)
            {

                
                // 일정 시간마다 Noise Force 의 방향을 랜덤하게 구함
                if (Module[0].NoiseForceTerm <= Particle.NoiseForceAccTime)
                {
                    Particle.NoiseForceAccTime -= Module[0].NoiseForceTerm;

                    float3 vRandom = GetRandom(NoiseTex, ThreadID.x, MAX_COUNT);
                    float3 vNoiseForce = normalize(vRandom.xyz - 0.5f);
                    Particle.NoiseForceDir = vNoiseForce;
                }
                

                Particle.vForce += Particle.NoiseForceDir * Module[0].NoiseForceScale;
                
                // force는 현재 속도의 크기에 비례
                Particle.vForce *= length(Particle.vVelocity);
                
                Particle.NoiseForceAccTime += g_EngineDT;                
            }

            // Particle 에 주어진 힘에 따른 가속도 계산 (mass는 1로 가정)
            float3 vAccel = Particle.vForce;

            // 가속도에 따른 속도의 변화
            Particle.vVelocity += vAccel * g_EngineDT;
            
            if (DragModule)
            {
                if (Particle.NormalizedAge < Module[0].DestNormalizedAge)
                {
                // 최소 허용 분모 값 설정
                    float minDenominator = 1e-6; // 매우 작은 값, 분모가 너무 작아지는 것을 방지
                    float denominator = max(Module[0].DestNormalizedAge - Particle.NormalizedAge, minDenominator);

                // 기울기 계산
                    float Gradient = (Module[0].TargetSpeed - length(Particle.vVelocity)) / denominator;
                    float NADT = g_EngineDT / Particle.Life;

                // 새로운 속도를 계산
                    float NewSpeed = length(Particle.vVelocity) + Gradient * NADT;

                // TargetSpeed를 넘어가는 것을 방지
                    if ((Module[0].TargetSpeed - length(Particle.vVelocity)) * (Module[0].TargetSpeed - NewSpeed) < 0.0f)
                    {
                    // 속도가 목표 속도를 넘어가면 목표 속도로 고정
                        NewSpeed = Module[0].TargetSpeed;
                    }
                    
                // 속도 벡터 업데이트            
                Particle.vVelocity = normalize(Particle.vVelocity) * NewSpeed;
                }
                else
                {
                // NormalizedAge >= DestNormalizedAge인 경우, 속도를 바로 TargetSpeed로 고정
                Particle.vVelocity = normalize(Particle.vVelocity) * Module[0].TargetSpeed;
                }
            }

            // 위치 업데이트
            Particle.vWorldPos += Particle.vVelocity * g_EngineDT;

            Particle.Age += g_EngineDT;
            Particle.NormalizedAge = Particle.Age / Particle.Life;
            
            // FadeOut 적용
            if (Render)
            {
                if (Module[0].FadeOut)
                {
                    float fRatio = saturate(1.f - (Particle.NormalizedAge - Module[0].StartRatio) / (1.f - Module[0].StartRatio));
                    Particle.vColor.a = fRatio;
                }
            }
        }

        // 파티클이 수명을 다한 경우 비활성화
        if (Particle.Age >= Particle.Life)
        {
            Particle.Active = false;
        }
    }
}

#endif // _MY_PARTICLE_TICK