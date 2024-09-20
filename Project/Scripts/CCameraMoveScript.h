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

	CGameObject* m_SnowParticle = nullptr;

	bool m_bChangeRoom = false;

	bool m_bDashShake = false;
	float m_DashShakeTime = 0.25f;
	float m_accDashShakeTime = 0;

    float m_ShakeRange = 10.f;
    //float m_ShakeSpeed = 50.f;
	// float m_shakeFrequency = 5.f;
	Vec3 m_ShakeDir;
	Vec3 m_PrevShakePos; // Shake하기 전의 Pos


private:
    void OrthoGraphicMove();
    void PerspectiveMove();

	void TrackPlayer();

public:
    void ChangeRoom();

    void SetChangeRoom();

    void TurnOnDashShake(Vec3 _dashDir);
	void DashShake();


public:
	virtual void Begin() override;
    virtual void Tick() override;
	virtual void FinalTick() override;

    virtual void SaveToFile(FILE* _File) override;
    virtual void LoadFromFile(FILE* _File) override;
};

