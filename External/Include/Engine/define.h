#pragma once

using Vec2 = Vector2;
using Vec3 = Vector3;
using Vec4 = Vector4;

#define DEVICE 	CDevice::GetInst()->GetDevice()
#define CONTEXT CDevice::GetInst()->GetContext()
#define MAX_LAYER 32

#define DT CTimeMgr::GetInst()->GetDeltaTime()
