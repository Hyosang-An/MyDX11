#ifndef _VALUE
#define _VALUE

// 4096
cbuffer OBJECT_POS : register(b0)
{
    row_major matrix matWorld;
    row_major matrix matView;
    row_major matrix matProj;
};

SamplerState g_sam_0 : register(s0); // 이방성 필터 샘플러
SamplerState g_sam_1 : register(s1); // 포인트 필터 샘플러

Texture2D g_tex_0 : register(t0);

#endif

