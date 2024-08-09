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

    wstring                 m_selectedDirectory;
    vector<Ptr<CSprite>>    m_vecSprite;

private:
    HRESULT GetSpriteFolderPath();
    void    FindSprites(wstring _folderPath);

public:
    virtual void Init() override;
    virtual void Update() override;
};

