#pragma once
#include "CAsset.h"
class CFlipBook :
    public CAsset
{
private:
    vector<Ptr<CSprite>>    m_vecSprite;

public:
    void AddSprite(Ptr<CSprite> _Sprite) { m_vecSprite.push_back(_Sprite); }

    Ptr<CSprite> GetSprite(int _Idx) { return m_vecSprite[_Idx]; }
    int GetMaxFrameCount() { return (int)m_vecSprite.size(); }


public:
    void FinalTick();

    virtual int Load(const wstring& _FilePath) { return S_OK; };
    virtual int Save(const wstring& _FilePath) { return S_OK; };

public:
    CFlipBook();
    ~CFlipBook();
};

