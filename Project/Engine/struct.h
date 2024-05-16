#pragma once

struct Vtx
{
	Vec3	vPos;
	Vec4	vColor;
};

// 상수버퍼 연동 구조체
struct tTransform
{
	Matrix matWorld;
};