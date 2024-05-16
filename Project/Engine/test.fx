//#ifndef _TEST
//#define _TEST

//// 4096
//cbuffer OBJECT_POS : register(b0)
//{
//    row_major matrix matWorld;
//};


//// Vertex Shader
//struct VTX_IN
//{
//    float3 vPos : POSITION;
//    float4 vColor : COLOR;
    
//};

//struct VTX_OUT
//{
//    float4 vPosition : SV_Position;
//    float4 vColor : COLOR;
//};

//VTX_OUT VS_Test(VTX_IN _in)
//{
//    VTX_OUT output = (VTX_OUT) 0.f;
  
//    // LocalSpace -> WorldSpace
//    // float3 x float4x4(matrix)
//    // float3 를 float4 로 차수를 맞추어준다.
//    // 동차좌표를 1 로 설정, 상태행렬 4행에 들어있는 이동을 적용받겠다는 뜻
//    output.vPosition = mul(float4(_in.vPos, 1.f), matWorld);
//    output.vColor = _in.vColor;
    
//    return output;
//}

//float4 PS_Test(VTX_OUT _in) : SV_Target
//{
//    return _in.vColor;
//}

//#endif