#pragma once
#include "ComponentUI.h"
class TileMapUI :
    public ComponentUI
{
    virtual void Update() override;

private:
    wstring m_lastTextureDirectory;
    class CTileMap* m_selectedTileMap = nullptr;

    int m_selectedTileImgIndex = -1;

private:
    void SelectTileMapAtlasByDialog();

public:
    TileMapUI();
    ~TileMapUI();
};

