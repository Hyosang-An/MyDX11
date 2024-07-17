#ifndef GAUSSIAN_BLUR
#define GAUSSIAN_BLUR

#include "value.fx"

struct VS_IN
{
    float3 vPos : POSITION;
    float2 vUV : TEXCOORD;
};

struct VS_OUT
{
    float4 vPosition : SV_Position;
    float2 vUV : TEXCOORD;
    
    float3 vWorldPos : POSITION;
};


// 가우시안 블러
#define KERNEL_SIZE 5
float weights[KERNEL_SIZE][KERNEL_SIZE] =
{
    { 0.0030, 0.0133, 0.0219, 0.0133, 0.0030 },
    { 0.0133, 0.0596, 0.0983, 0.0596, 0.0133 },
    { 0.0219, 0.0983, 0.1621, 0.0983, 0.0219 },
    { 0.0133, 0.0596, 0.0983, 0.0596, 0.0133 },
    { 0.0030, 0.0133, 0.0219, 0.0133, 0.0030 }
};
        
float offsets[KERNEL_SIZE] =
{
    -2.0, -1.0, 0.0, 1.0, 2.0
};

VS_OUT VS_GaussianBlur(VS_IN _in)
{
    VS_OUT output = (VS_OUT) 0.f;
    
    // Proj 행렬을 곱한 결과는 각 xyz 에 자신의 ViewZ 가 곱혀져있는 형태이다.
    // W 자리에 자신의 ViewZ 가 출력되기 때문에 이것으로 각 xyz 를 나누어야 실제 Proj 좌표가 나온다.
    // 따라서 Rasterizer State 에 투영행렬을 곱한 결과를 전달하면 이후 렌더링 파이프라인에서 각 xyz 를 w 로 나누어서 NDC좌표로 만들어 사용한다.
    output.vPosition = float4(_in.vPos.xy * 2.f, 0.f, 1.f);
    output.vUV = _in.vUV;
    
    return output;
}

float4 PS_GaussianBlur(VS_OUT _in) : SV_Target
{
    
    float2 texelSize = float2(1 / g_Resolution.x, 1 / g_Resolution.y);
    
    // 가우시안 블러
    #define KERNEL_SIZE 5
    float weights[KERNEL_SIZE][KERNEL_SIZE] =
    {
        { 0.0030, 0.0133, 0.0219, 0.0133, 0.0030 },
        { 0.0133, 0.0596, 0.0983, 0.0596, 0.0133 },
        { 0.0219, 0.0983, 0.1621, 0.0983, 0.0219 },
        { 0.0133, 0.0596, 0.0983, 0.0596, 0.0133 },
        { 0.0030, 0.0133, 0.0219, 0.0133, 0.0030 }
    };
        
    float offsets[KERNEL_SIZE] =
    {
        -2.0, -1.0, 0.0, 1.0, 2.0
    };
        
    float4 blurColor = float4(0.f, 0.f, 0.f, 1.f);
        
        // blur
    for (int i = 0; i < KERNEL_SIZE; i++)
        for (int j = 0; j < KERNEL_SIZE; j++)
        {
            float2 offset = float2(offsets[i] * texelSize.x, offsets[j] * texelSize.y);
            float2 samplingUV = clamp(_in.vUV + offset, 0.0, 1.0);
            blurColor += g_tex_1.Sample(g_sam_0, samplingUV) * weights[i][j];
        }

    return blurColor;
}
        




#endif