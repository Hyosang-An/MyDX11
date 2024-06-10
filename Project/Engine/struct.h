#pragma once

struct Vtx
{
	Vec3	vPos;
	Vec4	vColor;
	Vec2	vUV;
};

struct tDebugShapeInfo
{
	DEBUG_SHAPE		Shape;
	Vec3			vPos;
	Vec3			vScale;
	Vec3			vRot;
	Matrix			matWorld;
	Vec4			vColor;
	float			LifeTime;
	float			Age;
	bool			DepthTest;
};

// 상수버퍼 연동 구조체
struct tTransform
{
	Matrix matWorld;
	Matrix matView;
	Matrix matProj;

	Matrix matWV;
	Matrix matWVP;
};

extern tTransform g_Trans;

struct tMtrlConst
{
	int		iArr[4];
	float	fArr[4];
	Vec2	v2Arr[4];
	Vec4	v4Arr[4];
	Matrix	matArr[4];
	int		btex[TEX_PARAM::END + 2]; // 16바이트 단위로 만들어야 하므로.
};