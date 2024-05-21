#pragma once

class CLevel;

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

public:
	void Init();
	void Progress();
};

