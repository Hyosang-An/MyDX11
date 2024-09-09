#pragma once
#include "C:\Users\gytha\Documents\Programming\Assort51th\MyDX11\External\Include\Engine\CScript.h"
class CDashTrailScript :
    public CScript
{
public:
	CDashTrailScript();
	~CDashTrailScript();
	virtual CDashTrailScript* Clone() { return new CDashTrailScript(*this); }

private:
	float m_accTime = 0;
	float m_lifeTime = 0.6f;

public:
	virtual void Tick() override;


	virtual void SaveToFile(FILE* _File) override {};
	virtual void LoadFromFile(FILE* _File) override {};
};

