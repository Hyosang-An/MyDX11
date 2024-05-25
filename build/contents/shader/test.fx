#ifndef _TEST
#define _TEST

#include "value.fx"

// Vertex Shader
struct VTX_IN
{
    float3 vPos : POSITION;
    float4 vColor : COLOR;
    float2 vUV : TEXCOORD;
};

struct VTX_OUT
{
    float4 vPosition : SV_Position;
    float4 vColor : COLOR;
    float2 vUV : TEXCOORD;
};

VTX_OUT VS_Test(VTX_IN _in)
{
    VTX_OUT output = (VTX_OUT) 0.f;
  
    // LocalSpace -> WorldSpace
    // float3 x float4x4(matrix)
    // float3 를 float4 로 차수를 맞추어준다.
    // 동차좌표를 1 로 설정, 상태행렬 4행에 들어있는 이동을 적용받겠다는 뜻
    float3 vWorldPos = mul(float4(_in.vPos, 1.f), matWorld);
    float4 vViewPos = mul(float4(vWorldPos, 1.f), matView);
    float4 vProjPos = mul(vViewPos, matProj);
    
    output.vPosition = vProjPos;
    output.vColor = _in.vColor;
    output.vUV = _in.vUV;
    
    return output;
}

float4 PS_Test(VTX_OUT _in) : SV_Target
{
    float4 vColor = g_tex_0.Sample(g_sam_1, _in.vUV);
    vColor.a = 0.f;
    return vColor;
}

#endif