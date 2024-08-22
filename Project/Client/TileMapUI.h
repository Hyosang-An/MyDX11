#pragma once
#include "ComponentUI.h"
#include <stack>

enum class TileMapEditMode
{
    EditTile,
    EditCollider
};

class TileMapUI :
    public ComponentUI
{
    virtual void Update() override;

private:
    wstring m_lastTextureDirectory;
    class CTileMap* m_selectedTileMap = nullptr;

    int m_selectedTileImgIndex = -1;

	TileMapEditMode m_editMode = TileMapEditMode::EditTile;

    std::stack<std::pair<int, int>> m_undoStack;

private:
    void SelectTileMapAtlasByDialog();
public:
    TileMapUI();
    ~TileMapUI();
};

