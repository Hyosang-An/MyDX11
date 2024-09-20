#include "pch.h"
#include "CScriptMgr.h"

#include "CBackgroundScript.h"
#include "CBadelineScript.h"
#include "CCameraMoveScript.h"
#include "CDashShockWaveEffectScript.h"
#include "CDashTrailScript.h"
#include "CJumpThruScript.h"
#include "CMissileScript.h"
#include "CPlayerScript.h"
#include "CRigidBody.h"
#include "CRoomScript.h"

void CScriptMgr::GetScriptInfo(vector<wstring>& _vec)
{
	_vec.push_back(L"CBackgroundScript");
	_vec.push_back(L"CBadelineScript");
	_vec.push_back(L"CCameraMoveScript");
	_vec.push_back(L"CDashShockWaveEffectScript");
	_vec.push_back(L"CDashTrailScript");
	_vec.push_back(L"CJumpThruScript");
	_vec.push_back(L"CMissileScript");
	_vec.push_back(L"CPlayerScript");
	_vec.push_back(L"CRigidBody");
	_vec.push_back(L"CRoomScript");
}

CScript * CScriptMgr::GetScript(const wstring& _strScriptName)
{
	if (L"CBackgroundScript" == _strScriptName)
		return new CBackgroundScript;
	if (L"CBadelineScript" == _strScriptName)
		return new CBadelineScript;
	if (L"CCameraMoveScript" == _strScriptName)
		return new CCameraMoveScript;
	if (L"CDashShockWaveEffectScript" == _strScriptName)
		return new CDashShockWaveEffectScript;
	if (L"CDashTrailScript" == _strScriptName)
		return new CDashTrailScript;
	if (L"CJumpThruScript" == _strScriptName)
		return new CJumpThruScript;
	if (L"CMissileScript" == _strScriptName)
		return new CMissileScript;
	if (L"CPlayerScript" == _strScriptName)
		return new CPlayerScript;
	if (L"CRigidBody" == _strScriptName)
		return new CRigidBody;
	if (L"CRoomScript" == _strScriptName)
		return new CRoomScript;
	return nullptr;
}

CScript * CScriptMgr::GetScript(UINT _iScriptType)
{
	switch (_iScriptType)
	{
	case (UINT)SCRIPT_TYPE::BACKGROUNDSCRIPT:
		return new CBackgroundScript;
		break;
	case (UINT)SCRIPT_TYPE::BADELINESCRIPT:
		return new CBadelineScript;
		break;
	case (UINT)SCRIPT_TYPE::CAMERAMOVESCRIPT:
		return new CCameraMoveScript;
		break;
	case (UINT)SCRIPT_TYPE::DASHSHOCKWAVEEFFECTSCRIPT:
		return new CDashShockWaveEffectScript;
		break;
	case (UINT)SCRIPT_TYPE::DASHTRAILSCRIPT:
		return new CDashTrailScript;
		break;
	case (UINT)SCRIPT_TYPE::JUMPTHRUSCRIPT:
		return new CJumpThruScript;
		break;
	case (UINT)SCRIPT_TYPE::MISSILESCRIPT:
		return new CMissileScript;
		break;
	case (UINT)SCRIPT_TYPE::PLAYERSCRIPT:
		return new CPlayerScript;
		break;
	case (UINT)SCRIPT_TYPE::RIGIDBODY:
		return new CRigidBody;
		break;
	case (UINT)SCRIPT_TYPE::ROOMSCRIPT:
		return new CRoomScript;
		break;
	}
	return nullptr;
}

const wchar_t * CScriptMgr::GetScriptName(CScript * _pScript)
{
	switch ((SCRIPT_TYPE)_pScript->GetScriptType())
	{
	case SCRIPT_TYPE::BACKGROUNDSCRIPT:
		return L"CBackgroundScript";
		break;

	case SCRIPT_TYPE::BADELINESCRIPT:
		return L"CBadelineScript";
		break;

	case SCRIPT_TYPE::CAMERAMOVESCRIPT:
		return L"CCameraMoveScript";
		break;

	case SCRIPT_TYPE::DASHSHOCKWAVEEFFECTSCRIPT:
		return L"CDashShockWaveEffectScript";
		break;

	case SCRIPT_TYPE::DASHTRAILSCRIPT:
		return L"CDashTrailScript";
		break;

	case SCRIPT_TYPE::JUMPTHRUSCRIPT:
		return L"CJumpThruScript";
		break;

	case SCRIPT_TYPE::MISSILESCRIPT:
		return L"CMissileScript";
		break;

	case SCRIPT_TYPE::PLAYERSCRIPT:
		return L"CPlayerScript";
		break;

	case SCRIPT_TYPE::RIGIDBODY:
		return L"CRigidBody";
		break;

	case SCRIPT_TYPE::ROOMSCRIPT:
		return L"CRoomScript";
		break;

	}
	return nullptr;
}