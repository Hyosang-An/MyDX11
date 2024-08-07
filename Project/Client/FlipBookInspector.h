#pragma once
#include "FlipBookEditor.h"


class FlipBookInspector :
    public FlipBookEditor
{
public:
    FlipBookInspector();
    ~FlipBookInspector();

private:
    wstring                 m_selectedDirectory;
    vector<Ptr<CSprite>>    m_vecSprite;

private:
    HRESULT GetSpriteFolderPath();
    void    FindSprites(wstring _folderPath);

public:
    virtual void Init() override;
    virtual void Update() override;
};

