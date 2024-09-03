#pragma once
#include "SC_Sub.h"

enum class SpriteSlectMode
{
    FromXML,                // XML���Ϸκ��� �о����
    ClickAndDrag,           // Ŭ�� �巡�� ���
    AutoSelectOnClick,      // Ŭ�� �� �ڵ� ����
    ManualSpecification,    // ���� �� ����

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

    Vec2            m_StartLT; // ManualSpecification ��忡�� ���� Left Top
    int             m_Count = 1; // ManualSpecification ��忡�� Count
    wstring         m_SpriteName; // ManualSpecification ��忡�� SpriteName

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
    void SaveAllSprites_Manual(); // ManualSpecification ��忡�� ���
	void SaveAllSprites_FromXML(); // FromXML ��忡�� ���


public:
    virtual void Init() override;
    virtual void Update() override;

};

