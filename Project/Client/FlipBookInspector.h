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
    FlipBookViewer*         m_viewer = nullptr; 

    wstring                 m_lastFlipBookDirectory;
    wstring                 m_selectedSpriteDirectory;
    vector<Ptr<CSprite>>    m_vecSpriteInFolder;
    int                     m_selectedSpriteIndex = -1;

private:
    HRESULT SelectSpriteFolderPath();
    void    SelectFlipBook();
    void    FindSprites(wstring _folderPath);

public:
    int     GetSelectedSpriteIndex() { return m_selectedSpriteIndex; }

public:
    virtual void Init() override;
    virtual void Update() override;
};

