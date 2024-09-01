#pragma once
#include "CAsset.h"

#include "CEngine.h"

typedef std::function<void(FILE*, class CGameObject*)> OBJECT_SAVE;
typedef std::function<CGameObject* (FILE*, int&)> OBJECT_LOAD;

class CPrefab :
    public CAsset
{
    friend class CEngine;

private:
    static OBJECT_SAVE  g_ObjectSaveFunc;
    static OBJECT_LOAD  g_ObjectLoadFunc;

private:
    int m_originalLayerIdx = -1;

private:
    class CGameObject* m_ProtoObject = nullptr;

  

public:
    void SetProtoObject(CGameObject* _Object) { m_ProtoObject = _Object; }
    CGameObject* Instantiate();
	int GetOriginalLayerIdx() { return m_originalLayerIdx; }

public:
    virtual int Load(const wstring& _FilePath) override;
    virtual int Save(const wstring& _FilePath) override;

public:
    CPrefab();
    ~CPrefab();
};

