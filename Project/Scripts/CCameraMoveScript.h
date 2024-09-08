#pragma once
#include "Engine/CScript.h"
class CCameraMoveScript :
    public CScript
{
public:
    CCameraMoveScript();
    ~CCameraMoveScript();
    virtual CCameraMoveScript* Clone() { return new CCameraMoveScript(*this); }

private:
    float   m_CamTrackingSpeed;
	float   m_CamRoomChangeSpeed = 2000;

	Vec3    m_ChangeRoomTargetPos;


	CGameObject* m_Player = nullptr;
	CGameObject* m_Room = nullptr;

	bool m_bChangeRoom = false;

private:
    void OrthoGraphicMove();
    void PerspectiveMove();

	void TrackPlayer();

public:
    void ChangeRoom();

    void SetChangeRoom();


public:
	virtual void Begin() override;
    virtual void Tick() override;
	virtual void FinalTick() override;

    virtual void SaveToFile(FILE* _File) override;
    virtual void LoadFromFile(FILE* _File) override;
};

