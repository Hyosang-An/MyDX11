#ifndef _POSTPROCESS
#define POSTPROCESS

#include "value.fx"

// ================
// GrayFilterShader
// ================
struct VS_IN
{
    float3 vPos : POSITION;
    float2 vUV : TEXCOORD;
};

struct VS_OUT
{
    float4 vPosition : SV_Position;
    float2 vUV : TEXCOORD;
};

VS_OUT VS_GrayFilter(VS_IN _in)
{
    VS_OUT output = (VS_OUT) 0.f;
    
    output.vPosition = float4(_in.vPos.xy * 2.f, 0.f, 1.f);
    output.vUV = _in.vUV;
    
    return output;
}


float4 PS_GrayFilter(VS_OUT _in) : SV_Target
{
    float4 vColor = g_tex_0.Sample(g_sam_0, _in.vUV);
    
    float Average = (vColor.x + vColor.y + vColor.z) / 3.f;
    
    vColor = float4(Average, Average, Average, 1.f);
    
    return vColor;
}





#endif