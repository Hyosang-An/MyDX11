#pragma once
#include "ComponentUI.h"
#include <stack>
class TileMapUI :
    public ComponentUI
{
    virtual void Update() override;

private:
    wstring m_lastTextureDirectory;
    class CTileMap* m_selectedTileMap = nullptr;

    int m_selectedTileImgIndex = -1;

    std::stack<std::pair<int, int>> m_undoStack;

private:
    void SelectTileMapAtlasByDialog();

public:
    TileMapUI();
    ~TileMapUI();
};

