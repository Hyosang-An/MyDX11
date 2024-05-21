#pragma once

class CCamera;

class CRenderMgr :
	public CSingleton<CRenderMgr>
{
private:
	friend class CSingleton<CRenderMgr>;
	CRenderMgr();
	~CRenderMgr();

private:
	vector<CCamera*>	m_vecCam;

public:
	void RegisterCamera(CCamera* _cam, int _camPriority);

public:
	void Init();
	void Tick();
};

