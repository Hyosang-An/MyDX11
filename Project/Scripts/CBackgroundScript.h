#pragma once
#include "Engine\CScript.h"
class CBackgroundScript :
    public CScript
{
public:
	CBackgroundScript();
	CBackgroundScript(const CBackgroundScript& _Other);
	~CBackgroundScript();
	virtual CBackgroundScript* Clone() { return new CBackgroundScript(*this); }

private:
	Ptr<CTexture> m_BackgroundTex;

public:
	void TrackingCamera(Vec3 _cameraPos);

public:
	virtual void Init() override;
    virtual void Begin() override;
    virtual void Tick() override;

    virtual void SaveToFile(FILE* _File) override;
    virtual void LoadFromFile(FILE* _File) override;
};

