#pragma once
#include "ComponentUI.h"
#include <stack>

enum class TileMapEditMode
{
    EditTile,
    EditCollider,
    MoveTileMapInRoom,
};

class TileMapUI :
    public ComponentUI
{
    virtual void Update() override;

private:
    wstring m_lastTextureDirectory;
    class CTileMap* m_selectedTileMap = nullptr;

    int m_selectedTileImgIndex = -1;

    Vec2 m_MouseTileRowCol = Vec2(0.f, 0.f);

	TileMapEditMode m_editMode = TileMapEditMode::EditTile;
	LAYER m_colliderLayerType = LAYER::WALL_OR_GROUND;

    std::stack<std::pair<int, int>> m_undoStack;

    // 충돌체 편집 모드
    Vec2 m_MouseClickTileRowCol;
    Vec2 m_MouseReleaseTileRowCol;

    Vec3 vColliderLTWorldPos; 
    Vec3 vColliderRBWorldPos;

private:
    void SelectTileMapAtlasByDialog();
public:
    TileMapUI();
    ~TileMapUI();
};

