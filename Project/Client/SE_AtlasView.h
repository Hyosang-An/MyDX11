#pragma once
#include "SE_Sub.h"
class SE_AtlasView :
    public SE_Sub
{
public:
    SE_AtlasView();
    ~SE_AtlasView();

private:
    Ptr<CTexture>   m_AtlasTex;
    ImVec2          m_imageRectMin; // Image ���� �»�� ���� ��ġ

    ImVec2          m_MousePos; // ���� ���콺 ��ġ

    ImVec2          m_MouseLTonOrignalTex;  // ���콺 ��ġ�� �ش��ϴ� Texture �� �ȼ� ��ǥ
    ImVec2          m_MouseRBonOriginalTex; // ���콺 ��ġ�� �ش��ϴ� Texture �� �ȼ� ��ǥ

    float           m_WidthSize = 200;
    float           m_WheelScale = 1;
    float           m_Ratio;        // ���� �ػ� ��� ��� �̹����� ����



public:
    void SetAtlasTex(Ptr<CTexture> _Tex);

private:
    void WheelCheck();
    void SelectCheck();
    void DrawSelectRect();

public:
    virtual void Init() override;
    virtual void Update() override;

};

