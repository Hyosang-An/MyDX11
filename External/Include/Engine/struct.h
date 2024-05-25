#pragma once

struct Vtx
{
	Vec3	vPos;
	Vec4	vColor;
	Vec2	vUV;
};

// 상수버퍼 연동 구조체
struct tTransform
{
	Matrix matWorld;
	Matrix matView;
	Matrix matProj;
};

extern tTransform g_Trans;

struct tMtrlConst
{
	//int;
	//float;
	//Vec2;
	//Vec4;
	//Matrix;
};