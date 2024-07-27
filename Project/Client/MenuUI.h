#pragma once
#include "EditorUI.h"
class MenuUI :
    public EditorUI
{
private:
    std::wstring m_lastSaveDirectory; // 초기 경로 설정


public:
    virtual void Init() override;
    virtual void Tick() override;
    virtual void Update() override;

private:
    void File();
    void Level();
    void GameObject();
    void Assets();

private:
    void AddScript();

    wstring CreateRelativePathAssetKey(ASSET_TYPE _Type, const wstring& _Format);


public:
    MenuUI();
    ~MenuUI();
};

