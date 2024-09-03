#pragma once

class CCamera;
class CGameObject;
class CLight2D;
class CStructuredBuffer;

struct tTextInfo
{
	wstring Text;
	Vector2 ScreenPos;
	float FontSize;
	Vector4 Color;

	float LifeTime;
	float accTime = 0.f;
};

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

	CGameObject*			m_DebugObject = nullptr;
	list<tDebugShapeInfo>   m_DebugShapeList;

	// Light
	vector<CLight2D*>       m_vecLight2D;
	CStructuredBuffer*		m_Light2DBuffer = nullptr;

	// PostProcess
	vector<Ptr<CTexture>>   m_vecPostProcessRTTex;
	//Ptr<CTexture>			m_PostProcessDSTex;

	bool					m_bEditorMode = false;	// Debug 모드면 true (Init에서 설정)

	list<tTextInfo>		m_TextList;

public:
	void RegisterCamera(CCamera* _cam, int _camPriority);
	void RegisterEditorCamera(CCamera* _Cam) { m_EditorCamera = _Cam; }
	void AddDebugShapeInfo(const tDebugShapeInfo& _Info) { m_DebugShapeList.push_back(_Info); }
	void RegisterLight2D(CLight2D* _light);
	void DeregisterLight2D(CLight2D* _light);

	void PostProcessCopy(int _postProcessRTTex_index);

	void SetEditorMode(bool _Mode) { m_bEditorMode = _Mode; }

	bool IsEditorMode() { return m_bEditorMode; }

	// color : R, G, B, A는 0~255 사이의 값
	void AddText(wstring _Text, Vector2 _ScreenPos, float _FontSize, Vector4 _Color, float _LifeTime);

private:
	void SetPostProcessTex();
	void RenderStart();
	void Clear();

	void RenderDebugShape();

public:
	void Init();
	void Tick();
};

