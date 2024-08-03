#pragma once
#include "SE_Sub.h"
class SE_Detail :
    public SE_Sub
{
public:
    SE_Detail();
    ~SE_Detail();

    Ptr<CTexture>   m_AtlasTex;
    Vec2            m_SpriteLT;
    Vec2            m_SpriteSize;
    Vec2            m_BackgroundSize = { 200, 200 };

    wstring         m_lastFileName;
    wstring         m_lastSaveDirectory;

private:
    void Atlas();
    void AtlasInfo();
    void SpriteInfo();

public:
    void SetAtlasTex(Ptr<CTexture> _Tex);

private:
    void SelectTexture(DWORD_PTR _ListUI);
    
    void SaveSprite();


public:
    virtual void Init() override;
    virtual void Update() override;

};

