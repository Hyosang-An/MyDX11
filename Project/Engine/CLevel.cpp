#include "pch.h"
#include "CLevel.h"

#include "CLayer.h"
#include "CGameObject.h"

CLevel::CLevel() :
	m_Layer{}
{
	for (int i = 0; i < MAX_LAYER; ++i)
	{
		m_Layer[i] = new CLayer(i);
	}
}

CLevel::~CLevel()
{
	Delete_Array(m_Layer);
}

void CLevel::Begin()
{
	for (int i = 0; i < MAX_LAYER; ++i)
	{
		m_Layer[i]->Begin();
	}
}

void CLevel::Tick()
{
	for (int i = 0; i < MAX_LAYER; ++i)
	{
		m_Layer[i]->Tick();
	}
}

void CLevel::FinalTick()
{
	for (int i = 0; i < MAX_LAYER; ++i)
	{
		m_Layer[i]->FinalTick();
	}
}

void CLevel::AddObject(int LayerIdx, CGameObject* _Object, bool _bMoveChild)
{
	m_Layer[LayerIdx]->AddObject(_Object, _bMoveChild);
}

CGameObject* CLevel::FindObjectByName(const wstring& _Name)
{
	for (UINT layerIndex = 0; layerIndex < MAX_LAYER; ++layerIndex)
	{
		const vector<CGameObject*>& vecParent = m_Layer[layerIndex]->GetParentObjects();

		for (size_t parentIndex = 0; parentIndex < vecParent.size(); ++parentIndex)
		{
			// BFS Ž��
			list<CGameObject*> objectList;
			objectList.push_back(vecParent[parentIndex]);

			while (!objectList.empty())
			{
				CGameObject* pObject = objectList.front();
				objectList.pop_front();

				// ���� ���� ��ü�� �̸��� Ȯ��
				if (_Name == pObject->GetName())
				{
					return pObject;
				}

				// �ڽ� ��ü���� ����Ʈ�� �߰�
				const vector<CGameObject*>& vecChild = pObject->GetChildren();
				for (size_t childIndex = 0; childIndex < vecChild.size(); ++childIndex)
				{
					objectList.push_back(vecChild[childIndex]);
				}
			}
		}
	}

	return nullptr;
}

void CLevel::ClearObject()
{
	for (UINT i = 0; i < MAX_LAYER; ++i)
	{
		m_Layer[i]->ClearObject();
	}
}

void CLevel::ChangeState(LEVEL_STATE _NextState)
{
	if (m_State == _NextState)
		return;

	// Stop -> Pause (X)
	if (STOP == m_State && PAUSE == _NextState)
		return;

	// Stop -> Play (���� ������ ������ ���۵Ǹ�, ������ �ִ� ��ü���� Begin �� ȣ��Ǿ�� �Ѵ�.)
	if (STOP == m_State && PLAY == _NextState)
	{
		Begin();
	}

	m_State = _NextState;

	// Play -> Stop (���� ������ ���۵Ǵ� �������� ������ �����ؾ� �Ѵ�.)
}