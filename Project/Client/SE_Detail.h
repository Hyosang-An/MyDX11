#pragma once
#include "SE_Sub.h"
class SE_Detail :
    public SE_Sub
{
public:
    SE_Detail();
    ~SE_Detail();

private:
    Ptr<CTexture>   m_AtlasTex;

private:
    void Atlas();
    void AtlasInfo();
    void StripeInfo();

public:
    void SetAtlasTex(Ptr<CTexture> _Tex);

private:
    void SelectTexture(DWORD_PTR _ListUI);
    


public:
    virtual void Init() override;
    virtual void Update() override;

};

