#ifndef _STD2D
#define _STD2D

#include "value.fx"
#include "func.fx"

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
    
    float3 vWorldPos : POSITION;
};

VTX_OUT VS_Std2D(VTX_IN _in)
{
    VTX_OUT output = (VTX_OUT) 0.f;
  
    // LocalSpace -> WorldSpace
    // float3 x float4x4(matrix)
    // float3 �� float4 �� ������ ���߾��ش�.
    // ������ǥ�� 1 �� ����, ������� 4�࿡ ����ִ� �̵��� ����ްڴٴ� ��
    
    output.vPosition = mul(float4(_in.vPos, 1.f), matWVP);
    output.vColor = _in.vColor;
    output.vUV = _in.vUV;
    
    output.vWorldPos = mul(float4(_in.vPos, 1.f), matWorld).xyz;
    
    return output;
}

float4 PS_Std2D(VTX_OUT _in) : SV_Target
{
    float4 vColor = float4(0.f, 0.f, 0.f, 1.f);
    
    // FlipBook �� ����Ѵ�.
    if (UseFlipbook)
    {
        // _in.vUV : �������̸� ������ ��ġ�� ������ ȯ���� ��        
        
        // ��Ʋ�� ������ ��׶����� ���� �� UV ��ǥ�� ����մϴ�.
        float2 BackGroundLeftTopInAtlasUV = LeftTopInAtlasUV - (BackGroundSizeInAtlasUV - SliceSizeInAtlasUV) / 2.f;
        
        // ��������Ʈ�� ��Ʋ�� ������ ������ ��ġ�� ����մϴ�.
        float2 vSpriteInAtlasUV = BackGroundLeftTopInAtlasUV + (_in.vUV * BackGroundSizeInAtlasUV);
        vSpriteInAtlasUV -= OffsetUV;
                
        // ��������Ʈ�� UV ��ǥ�� ��ȿ�� ���� ���� �ִ��� �˻��մϴ�.
        if (LeftTopInAtlasUV.x <= vSpriteInAtlasUV.x && vSpriteInAtlasUV.x <= LeftTopInAtlasUV.x + SliceSizeInAtlasUV.x
            && LeftTopInAtlasUV.y <= vSpriteInAtlasUV.y && vSpriteInAtlasUV.y <= LeftTopInAtlasUV.y + SliceSizeInAtlasUV.y)
        {
            // ��ȿ�� ���� ���� ������, �ؽ�ó�� ���ø��Ͽ� ������ �����ɴϴ�.
            vColor = g_AtlasTex.Sample(g_sam_1, vSpriteInAtlasUV);
        }
        else
        {
            //vColor = float4(1.f, 1.f, 0.f, 1.f);
            discard;
        }
    }
    
    // FlipBook �� ������� �ʴ´�.
    else
    {
        if (g_btex_0)
        {
            vColor = g_tex_0.Sample(g_sam_1, _in.vUV);
        }
        else
        {
            vColor = float4(1.f, 0.f, 1.f, 1.f);
        }
    }
    
    if (vColor.a == 0.f)
    {
        discard;
    }
    

    // ���� ����      
    tLight Light = (tLight) 0.f;
    
    for (int i = 0; i < g_Light2DCount; ++i)
    {
        CalculateLight2D(i, _in.vWorldPos, Light); // �����鸸 ���������� ����
    }
    
    vColor.rgb = vColor.rgb * Light.Color.rgb 
               + vColor.rgb * Light.Ambient.rgb;
    
    return vColor;
}



float4 PS_Std2D_Alphablend(VTX_OUT _in) : SV_Target
{
    float4 vColor = float4(0.f, 0.f, 0.f, 1.f);
     
    if (g_btex_0)
    {
        vColor = g_tex_0.Sample(g_sam_1, _in.vUV);
    }
    else
    {
        vColor = float4(1.f, 0.f, 1.f, 1.f);
    }
    
    return vColor;
}

#endif