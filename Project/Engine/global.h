#pragma once

#include <Windows.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>

//DX 스마트 포인터
#include <wrl.h>
using namespace Microsoft::WRL;

#pragma comment(lib, "d3d11")
#pragma comment(lib, "d3dcompiler")

#include "SimpleMath.h"
using namespace DirectX;
using namespace DirectX::SimpleMath;


#include <string>
using std::string;
using std::wstring;

#include <vector>
using std::vector;

#include <list>
using std::list;

#include <map>
using std::map;
using std::make_pair;

#include <typeinfo>

#include "singleton.h"
#include "define.h"
#include "enum.h"
#include "struct.h"
#include "func.h"
#include "Ptr.h"