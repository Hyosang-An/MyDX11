#pragma once

class CLevel;
class CGameObject;

class CLevelMgr :
	public CSingleton<CLevelMgr>
{
private:
	friend CSingleton<CLevelMgr>;
	CLevelMgr();
	~CLevelMgr();

private:
	CLevel* m_CurLevel;

public:
	CLevel* GetCurrentLevel() { return m_CurLevel; }
	CGameObject* FindObjectByName(const wstring& _Name);

public:
	void Init();
	void Progress();
};

