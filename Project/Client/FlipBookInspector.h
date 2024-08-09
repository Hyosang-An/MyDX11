#pragma once
#include "FlipBookEditor.h"


class FlipBookInspector :
    public FlipBookEditor
{
    friend class FlipBookEditor;
public:
    FlipBookInspector();
    ~FlipBookInspector();

private:
    FlipBookEditor*         m_owner = nullptr;

    wstring                 m_lastFlipBookDirectory;
    wstring                 m_selectedSpriteDirectory;
    vector<Ptr<CSprite>>    m_vecSpriteInFolder;

private:
    HRESULT SelectSpriteFolderPath();
    void    SelectFlipBook();
    void    FindSprites(wstring _folderPath);

public:
    virtual void Init() override;
    virtual void Update() override;
};

