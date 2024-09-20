#pragma once
#include "Engine\CScript.h"
class CDashShockWaveEffectScript :
	public CScript
{
public:
	CDashShockWaveEffectScript();
	CDashShockWaveEffectScript(const CDashShockWaveEffectScript& _Other);
	~CDashShockWaveEffectScript();
	virtual CDashShockWaveEffectScript* Clone() { return new CDashShockWaveEffectScript(*this); }

private:
	float m_Time = 0.f;
	float m_LifeTime = 3.f;


public:
	virtual void Begin() override;
	virtual void Tick() override;

	virtual void SaveToFile(FILE* _File) override;
	virtual void LoadFromFile(FILE* _File) override;
};

