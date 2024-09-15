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

                // ���� ��ƼŬ
                if (Module[0].Type == 0)
                {
                    float3 spawnRange = float3(Module[0].SpawnShapeScale.xy, z_Max);
                    // float3 spawnRange = float3(500, 500, z_Max);
                    // spawnRange.x = vRandom0.x * spawnRange.x;
                    spawnRange.x = (-0.5f + vRandom0.x) * spawnRange.x;
                    spawnRange.y = (-0.5f + vRandom0.y) * spawnRange.y;
                    spawnRange.z = vRandom0.z * spawnRange.z;
                    float3 spawnPos = ParticleObjectPos + spawnRange;

                    // Particle �ʱ�ȭ
                    Particle.vWorldInitPos = Particle.vWorldPos = spawnPos;
                    Particle.vWorldCurrentScale = float3(10.f, 10.f, 10.f);
                    Particle.Age = 0.f;
                    Particle.NormalizedAge = 0.f;
                    Particle.Life = 8.f;

                    Particle.RandomValue = vRandom1.x;

                    // ��ƼŬ ���� spawnPos�� z���� 0�� �������� ���, z_Max�� �������� ȸ��
                    float3 vColor = lerp(float3(1, 1, 1), float3(0.3f, 0.3f, 0.3f), spawnPos.z / z_Max);
                    Particle.vColor = float4(vColor, 1.f);

                    // ��ƼŬ �ӵ��� y���� spawnPos�� z���� Ŭ���� ���� �ֱ��� cos �׷��� ���·� ��ȭ, �ӵ��� x���� spawnPos�� z���� Ŭ���� �۵��� �ʱ�ȭ
                    float3 velocity = float3(0.f, 0.f, 0.f);

                    // �ӵ��� x���� 600 ~ 1800 ���̿��� ��ȭ
                    velocity.x = -(600 + 1200 * (1 - spawnPos.z / z_Max));

                    // �ֱ�� 800 ~ 4000 ���̿��� ��ȭ, �ӵ��� ũ��� 200 ~ 1000 ���̿��� ��ȭ

                    // �ֱ�
                    float L = (1 - Particle.vWorldInitPos.z / z_Max) * 3200 + 800;
                    float period = 2 * PI / ((1 - Particle.vWorldInitPos.z / z_Max) * 3200 + 800);

                    // 0 ~ �ֱ� ������ ������
                    float randomValue = Particle.RandomValue * period;
                    velocity.y = cos(Particle.RandomValue * L * period) * ((1 - Particle.vWorldInitPos.z / z_Max) * 800 + 200);
                    Particle.vVelocity = velocity;
                }

                // �뽬 ��ƼŬ
                else if (Module[0].Type == 1)
                {
                    float3 spawnRange = Module[0].SpawnShapeScale;
                    spawnRange.x = (-0.5f + vRandom0.x) * spawnRange.x;
                    spawnRange.y = (-0.5f + vRandom0.y) * spawnRange.y;
                    spawnRange.z = 0;
                    float3 spawnPos = ParticleObjectPos + spawnRange;

                    // Particle �ʱ�ȭ
                    Particle.vWorldInitPos = Particle.vWorldPos = spawnPos;
                    Particle.vWorldCurrentScale = float3(10.f, 10.f, 10.f);
                    Particle.Age = 0.f;
                    Particle.NormalizedAge = 0.f;
                    Particle.Life = 5.f;

                    // ��ƼŬ ���� �ϴû�
                    Particle.vColor = float4(0.5f, 0.5f, 1.f, 1.f);

                    // �ӵ��� ���� ������ �������� +- 30�� �������� ����
                    // ���� ���� ����ȭ
                    float2 dir = normalize(Module[0].ReferenceDir.xy);

                    // ���� ���� ��� (-30������ +30�� ����)
                    float theta = (-PI / 6.0) + (PI / 3.0) * vRandom1.x;

                    // ȸ�� ��ȯ ����
                    float cosTheta = cos(theta);
                    float sinTheta = sin(theta);

                    float2 rotatedDir;
                    rotatedDir.x = dir.x * cosTheta - dir.y * sinTheta;
                    rotatedDir.y = dir.x * sinTheta + dir.y * cosTheta;

                    // �ӵ� ũ�� 50 ~ 100 ������ �ӵ�
                    float speed = vRandom1.y * 50 + 50;

                    Particle.vVelocity = float3(rotatedDir, 0) * speed;
                }

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

        // ���� ��ƼŬ
        if (Module[0].Type == 0)
        {
            // y�ӵ� ������Ʈ
            // �ֱ�
            float L = (1 - Particle.vWorldInitPos.z / z_Max) * 3200 + 800;
            float period = 2 * PI / ((1 - Particle.vWorldInitPos.z / z_Max) * 3200 + 800);
            // 0 ~ �ֱ� ������ ������
            float randomValue = Particle.RandomValue * period;
            float x_dist = abs(Particle.vWorldPos.x - Particle.vWorldInitPos.x);
            Particle.vVelocity.y = cos((x_dist + Particle.RandomValue * L) * period) * ((1 - Particle.vWorldInitPos.z / z_Max) * 800 + 200);

            // ��ġ ������Ʈ
            Particle.vWorldPos += Particle.vVelocity * g_EngineDT;

            Particle.Age += g_EngineDT;
            Particle.NormalizedAge = Particle.Age / Particle.Life;
        }

        // �뽬 ��ƼŬ
        else if (Module[0].Type == 1)
        {
            Particle.vForce = float3(0.f, 0.f, 0.f);

            // Noise Force Module
            if (NoiseForce)
            {

                
                // ���� �ð����� Noise Force �� ������ �����ϰ� ����
                if (Module[0].NoiseForceTerm <= Particle.NoiseForceAccTime)
                {
                    Particle.NoiseForceAccTime -= Module[0].NoiseForceTerm;

                    float3 vRandom = GetRandom(NoiseTex, ThreadID.x, MAX_COUNT);
                    float3 vNoiseForce = normalize(vRandom.xyz - 0.5f);
                    Particle.NoiseForceDir = vNoiseForce;
                }
                

                Particle.vForce += Particle.NoiseForceDir * Module[0].NoiseForceScale;
                
                // force�� ���� �ӵ��� ũ�⿡ ���
                Particle.vForce *= length(Particle.vVelocity);
                
                Particle.NoiseForceAccTime += g_EngineDT;                
            }

            // Particle �� �־��� ���� ���� ���ӵ� ��� (mass�� 1�� ����)
            float3 vAccel = Particle.vForce;

            // ���ӵ��� ���� �ӵ��� ��ȭ
            Particle.vVelocity += vAccel * g_EngineDT;
            
            if (DragModule)
            {
                if (Particle.NormalizedAge < Module[0].DestNormalizedAge)
                {
                // �ּ� ��� �и� �� ����
                    float minDenominator = 1e-6; // �ſ� ���� ��, �и� �ʹ� �۾����� ���� ����
                    float denominator = max(Module[0].DestNormalizedAge - Particle.NormalizedAge, minDenominator);

                // ���� ���
                    float Gradient = (Module[0].TargetSpeed - length(Particle.vVelocity)) / denominator;
                    float NADT = g_EngineDT / Particle.Life;

                // ���ο� �ӵ��� ���
                    float NewSpeed = length(Particle.vVelocity) + Gradient * NADT;

                // TargetSpeed�� �Ѿ�� ���� ����
                    if ((Module[0].TargetSpeed - length(Particle.vVelocity)) * (Module[0].TargetSpeed - NewSpeed) < 0.0f)
                    {
                    // �ӵ��� ��ǥ �ӵ��� �Ѿ�� ��ǥ �ӵ��� ����
                        NewSpeed = Module[0].TargetSpeed;
                    }
                    
                // �ӵ� ���� ������Ʈ            
                Particle.vVelocity = normalize(Particle.vVelocity) * NewSpeed;
                }
                else
                {
                // NormalizedAge >= DestNormalizedAge�� ���, �ӵ��� �ٷ� TargetSpeed�� ����
                Particle.vVelocity = normalize(Particle.vVelocity) * Module[0].TargetSpeed;
                }
            }

            // ��ġ ������Ʈ
            Particle.vWorldPos += Particle.vVelocity * g_EngineDT;

            Particle.Age += g_EngineDT;
            Particle.NormalizedAge = Particle.Age / Particle.Life;
            
            // FadeOut ����
            if (Render)
            {
                if (Module[0].FadeOut)
                {
                    float fRatio = saturate(1.f - (Particle.NormalizedAge - Module[0].StartRatio) / (1.f - Module[0].StartRatio));
                    Particle.vColor.a = fRatio;
                }
            }
        }

        // ��ƼŬ�� ������ ���� ��� ��Ȱ��ȭ
        if (Particle.Age >= Particle.Life)
        {
            Particle.Active = false;
        }
    }
}

#endif // _MY_PARTICLE_TICK