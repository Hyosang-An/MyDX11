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


// ����þ� ��
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
    
    // Proj ����� ���� ����� �� xyz �� �ڽ��� ViewZ �� �������ִ� �����̴�.
    // W �ڸ��� �ڽ��� ViewZ �� ��µǱ� ������ �̰����� �� xyz �� ������� ���� Proj ��ǥ�� ���´�.
    // ���� Rasterizer State �� ��������� ���� ����� �����ϸ� ���� ������ ���������ο��� �� xyz �� w �� ����� NDC��ǥ�� ����� ����Ѵ�.
    output.vPosition = float4(_in.vPos.xy * 2.f, 0.f, 1.f);
    output.vUV = _in.vUV;
    
    return output;
}

float4 PS_GaussianBlur(VS_OUT _in) : SV_Target
{
    
    float2 texelSize = float2(1 / g_Resolution.x, 1 / g_Resolution.y);
    
    // ����þ� ��
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