#pragma once
#include "CEntity.h"

class CComponent;
class CRenderComponent;
class CScript;

class CGameObject :
    public CEntity
{
    friend class CLevel;
    friend class CLayer;
    friend class CTaskMgr;

public:
    CGameObject();
    CGameObject(const CGameObject& _Origin);
    ~CGameObject();
    virtual CGameObject* Clone() { return new CGameObject(*this); }

private:
    CComponent*             m_arrCom[(UINT)COMPONENT_TYPE::END];
    CRenderComponent*       m_RenderCom;
    vector<CScript*>        m_vecScript;

    CGameObject*            m_Parent;
    vector<CGameObject*>    m_vecChildren;

    int                     m_LayerIdx = -1; // 소속 레이어의 인덱스 번호

    bool                    m_Dead = false;

public:
    void AddComponent(CComponent* _Comopnent);
    void AddChild(CGameObject* _ChildObject);

	void RemoveComponent(COMPONENT_TYPE _Type);
	void RemoveScript(CScript* _Script);

	// LevelSaveLoad에서 사용
	void AddChild(CGameObject* _ChildObject, int _childLayerIdx);

    CComponent* GetComponent(COMPONENT_TYPE _Type) { return m_arrCom[(UINT)_Type]; }
    CRenderComponent* GetRenderComponent() { return m_RenderCom; }
    CGameObject* GetParent() { return m_Parent; }
    const vector<CGameObject*>& GetChildren() { return m_vecChildren; }
    int GetLayerIdx() { return m_LayerIdx; }
    const vector<CScript*> GetScriptVec() { return m_vecScript; }
    template<typename T>
    T* GetScript();

    bool IsAncestor(CGameObject* _Object);
    bool IsDead() { return m_Dead; }
    void DisconnectWithLayer();

    // Unregister : 등록을 취소하다, 등록하는 행위를 취소하다
    // Deregister : 등록된것을 취소하다
    void DeregisterThisChildFromParent();

    class CTransform* Transform() { return (CTransform*)GetComponent(COMPONENT_TYPE::TRANSFORM); }
    class CMeshRender* MeshRender() { return (CMeshRender*)GetComponent(COMPONENT_TYPE::MESHRENDER); }
    class CCamera* Camera() { return (CCamera*)GetComponent(COMPONENT_TYPE::CAMERA); }
    class CCollider2D* Collider2D() { return (CCollider2D*)GetComponent(COMPONENT_TYPE::COLLIDER2D); }
    class CFlipBookComponent* FlipBookComponent() { return (CFlipBookComponent*)GetComponent(COMPONENT_TYPE::FLIPBOOK_COMPONENT); }
    class CTileMap* TileMap() { return (CTileMap*)GetComponent(COMPONENT_TYPE::TILEMAP); }
    class CLight2D* Light2D() { return (CLight2D*)GetComponent(COMPONENT_TYPE::LIGHT2D); }
    class CParticleSystem* ParticleSystem() { return (CParticleSystem*)GetComponent(COMPONENT_TYPE::PARTICLE_SYSTEM); }


    // Prefab Save 전용!
	void SetLayerIdxForPrefabSave(int _Idx) { m_LayerIdx = _Idx; }

public:
    void Begin();
    void Tick();
    virtual void FinalTick();
    void Render();
};

// m_vecScript에서 특정 스크립트를 찾아서 반환하는 함수
template <typename T>
T* CGameObject::GetScript()
{
	for (size_t i = 0; i < m_vecScript.size(); ++i)
	{
		T* pScript = dynamic_cast<T*>(m_vecScript[i]);
		if (pScript)
			return pScript;
	}

	return nullptr;
}
