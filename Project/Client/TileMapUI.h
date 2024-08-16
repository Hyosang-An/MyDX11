#pragma once
#include "ComponentUI.h"
class TileMapUI :
    public ComponentUI
{
    virtual void Update() override;

private:
    wstring m_lastTextureDirectory;
    class CTileMap* m_selectedTileMap = nullptr;

private:
    void SelectTileMapAtlasByDialog();

public:
    TileMapUI();
    ~TileMapUI();
};

