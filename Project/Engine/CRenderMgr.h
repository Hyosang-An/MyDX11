#pragma once

class CCamera;
class CGameObject;

class CRenderMgr :
	public CSingleton<CRenderMgr>
{
private:
	friend class CSingleton<CRenderMgr>;
	CRenderMgr();
	~CRenderMgr();

private:
	vector<CCamera*>		m_vecCam;
	CCamera*				m_EditorCamera = nullptr;

	CGameObject*			m_DebugObject;
	list<tDebugShapeInfo>   m_DebugShapeList;

public:
	void RegisterCamera(CCamera* _cam, int _camPriority);
	void RegisterEditorCamera(CCamera* _Cam) { m_EditorCamera = _Cam; }
	void AddDebugShapeInfo(const tDebugShapeInfo& _Info) { m_DebugShapeList.push_back(_Info); }

private:
	void RenderDebugShape();

public:
	void Init();
	void Tick();
};

