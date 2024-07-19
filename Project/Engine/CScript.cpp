#include "pch.h"
#include "CScript.h"


CScript::CScript(UINT _Type) :
	CComponent(COMPONENT_TYPE::SCRIPT),
	m_ScriptType(_Type)
{
}

CScript::~CScript()
{

}