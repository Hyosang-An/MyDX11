#pragma once
#include "SC_Sub.h"

enum class SpriteSlectMode
{
    FromXML,                // XML파일로부터 읽어오기
    ClickAndDrag,           // 클릭 드래그 방식
    AutoSelectOnClick,      // 클릭 시 자동 선택
    ManualSpecification,    // 영역 값 지정

};

class SE_Detail :
    public SE_Sub
{
public:
    SE_Detail();
    ~SE_Detail();

    Ptr<CTexture>   m_AtlasTex;
    SpriteSlectMode m_SelectMode = SpriteSlectMode::FromXML;

    Vec2            m_SpriteLT;
    Vec2            m_SpriteSize;
    Vec2            m_BackgroundSize = { 200, 200 };

    Vec2            m_StartLT; // ManualSpecification 모드에서 시작 Left Top
    int             m_Count = 1; // ManualSpecification 모드에서 Count
    wstring         m_SpriteName; // ManualSpecification 모드에서 SpriteName

    wstring         m_lastFileName;
    wstring         m_lastDirectoryInDialog;

	vector<RECT>    m_vecSpriteRectsFromXML;

private:
    void Atlas();
    void AtlasInfo();
    void SpriteInfo();

    HRESULT SelectSpriteSaveFolderByDialog();
	HRESULT SelectAtlasByDialog();

public:
    void SetAtlasTex(Ptr<CTexture> _Tex);

    void SetSpriteSelectMode(SpriteSlectMode _Mode) { m_SelectMode = _Mode; }
    SpriteSlectMode GetSpriteSelectMode() { return m_SelectMode; }

    std::pair<Vec2, Vec2> GetStartLTandSize() { return { m_StartLT, m_SpriteSize }; }
    int GetCount() { return m_Count; }

	const vector<RECT>& GetSpriteRectsFromXML() { return m_vecSpriteRectsFromXML; }

private:
    void SelectTexture(DWORD_PTR _ListUI);
    
    void SaveSprite();
    void SaveAllSprites_Manual(); // ManualSpecification 모드에서 사용
	void SaveAllSprites_FromXML(); // FromXML 모드에서 사용


public:
    virtual void Init() override;
    virtual void Update() override;

};

