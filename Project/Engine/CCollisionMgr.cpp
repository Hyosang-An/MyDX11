#include "pch.h"
#include "CCollisionMgr.h"

#include "CLevelMgr.h"
#include "CLevel.h"
#include "CLayer.h"
#include "CGameObject.h"
#include "CCollider2D.h"

#include "CAssetMgr.h"

CCollisionMgr::CCollisionMgr()
	: m_Matrix{}
{

}

CCollisionMgr::~CCollisionMgr()
{

}


void CCollisionMgr::Init()
{
	CollisionCheck(LAYER::PLAYER, LAYER::WALL_OR_GROUND);
	CollisionCheck(LAYER::PLAYER, LAYER::SPIKE);
	CollisionCheck(LAYER::PLAYER, LAYER::MONSTER);

	CollisionCheck(LAYER::ROOM, LAYER::ROOM);
}

void CCollisionMgr::Tick()
{
	for (int Row = 0; Row < MAX_LAYER; ++Row)
	{
		for (int Col = Row; Col < MAX_LAYER; ++Col)
		{
			if (m_Matrix[Row] & (1 << Col))
			{
				CollisionBtwLayer(Row, Col);
			}
		}
	}
}



void CCollisionMgr::CollisionCheck(UINT Layer1, UINT Layer2)
{
	UINT Row = Layer1;
	UINT Col = Layer2;

	if (Row > Col)
	{
		Row = (UINT)Layer2;
		Col = (UINT)Layer1;
	}

	if (m_Matrix[Row] & (1 << Col))
	{
		m_Matrix[Row] &= ~(1 << Col);
	}
	else
	{
		m_Matrix[Row] |= (1 << Col);
	}
}

void CCollisionMgr::CollisionCheck(LAYER _layer1, LAYER _Layer2)
{
	CollisionCheck((UINT)_layer1, (UINT)_Layer2);
}


void CCollisionMgr::CollisionBtwLayer(UINT _Left, UINT _Right)
{
	// ���� ���� �����´�.
	CLevel* pCurLevel = CLevelMgr::GetInst()->GetCurrentLevel();
	if (nullptr == pCurLevel)
		return;

	// �� ���̾ ���� ��� ������Ʈ���� �����´�.
	const vector<CGameObject*>& vecLeft = pCurLevel->GetLayer(_Left)->GetObjects();
	const vector<CGameObject*>& vecRight = pCurLevel->GetLayer(_Right)->GetObjects();

	for (size_t i = 0; i < vecLeft.size(); ++i)
	{
		CCollider2D* pLeftCol = vecLeft[i]->Collider2D();
		if (nullptr == pLeftCol)
			continue;

		size_t startJ = (vecLeft == vecRight) ? i + 1 : 0;

		for (size_t j = startJ; j < vecRight.size(); ++j)
		{
			if (vecLeft[i] == vecRight[j])
				continue;

			CCollider2D* pRightCol = vecRight[j]->Collider2D();
			if (nullptr == pRightCol)
				continue;



			COLLIDER_ID id = {};

			id.LeftID = pLeftCol->GetID();
			id.RightID = pRightCol->GetID();

			map<ULONGLONG, bool>::iterator iter = m_mapCollisionInfo.find(id.ID);

			// ��ϵ����� ������ ��Ͻ�Ų��.
			if (iter == m_mapCollisionInfo.end())
			{
				m_mapCollisionInfo.insert(make_pair(id.ID, false));
				iter = m_mapCollisionInfo.find(id.ID);
			}

			bool bDead = vecLeft[i]->IsDead() || vecRight[j]->IsDead();
			//bool bDeactive = !vecLeft[i]->IsActive() || !vecRight[j]->IsActive();

			// �� �浹ü�� ���� �浹���̴�.
			if (IsCollision(pLeftCol, pRightCol))
			{
				// �������� �浹���̾���.
				if (iter->second)
				{
					// �� �浹ü�� �ϳ��� Dead ���°ų� ��Ȱ��ȭ ���¶��
					if (bDead) // || bDeactive)
					{
						pLeftCol->EndOverlap(pRightCol);
						pRightCol->EndOverlap(pLeftCol);
						iter->second = false;
					}
					else
					{
						pLeftCol->Overlap(pRightCol);
						pRightCol->Overlap(pLeftCol);
					}
				}

				// �������� �浹���� �ƴϾ���.
				else
				{
					// �� �浹ü�� �ϳ��� Dead ���°ų� ��Ȱ��ȭ ���¶��
					if (bDead) // || bDeactive)
					{
						iter->second = false;
					}
					else
					{
						pLeftCol->BeginOverlap(pRightCol);
						pRightCol->BeginOverlap(pLeftCol);
						iter->second = true;
					}
				}

				// �� �浹ü�� �ϳ��� Dead ���°ų� ��Ȱ��ȭ ���¶��
				// �߰��� �浹 ������ ȣ������ش�.
				/*if (bDead || bDeactive)
				{
					vecLeft[i]->EndOverlap(vecRight[j]);
					vecRight[j]->EndOverlap(vecLeft[i]);
					iter->second = false;
				}*/
			}

			// �� �浹ü�� ���� �浹���� �ƴϴ�.
			else
			{
				// �������� �浹���̾���.
				if (iter->second)
				{
					pLeftCol->EndOverlap(pRightCol);
					pRightCol->EndOverlap(pLeftCol);
				}

				iter->second = false;
			}
		}
	}
}

bool CCollisionMgr::IsCollision(CCollider2D* _Left, CCollider2D* _Right)
{
	//// BOD �˰���
	//
	//// �浹ü�� �⺻ ���� ������ �����´�.
	//Ptr<CMesh> pRectMesh = CAssetMgr::GetInst()->FindAsset<CMesh>(L"RectMesh");
	//Vtx* pVtx = (Vtx*)pRectMesh->GetVtxSysMem();

	//// �� �浹ü�� ���� ����� �����´�.
	//const Matrix& matLeft = _Left->GetWorldMat();
	//const Matrix& matRight = _Right->GetWorldMat();

	//// �⺻ ����(Rect) �� �� �浹ü�� ��������� ���ؼ�, �浹ü�� �� �𼭸� ��ġ�� �ű� ��,
	//// ��ǥ���� ��ġ���� ���� �浹ü�� ����󿡼��� ��ġ���� ������ ǥ�� ���⺤�͸� ���Ѵ�.
	//// �� ���ʹ� �浹ü���� ������ų ���� �� ����
	//Vec3 vProjAxis[4] = {};

	//vProjAxis[0] = XMVector3TransformCoord(pVtx[3].vPos, matLeft) - XMVector3TransformCoord(pVtx[0].vPos, matLeft);
	//vProjAxis[1] = XMVector3TransformCoord(pVtx[1].vPos, matLeft) - XMVector3TransformCoord(pVtx[0].vPos, matLeft);

	//vProjAxis[2] = XMVector3TransformCoord(pVtx[3].vPos, matRight) - XMVector3TransformCoord(pVtx[0].vPos, matRight);
	//vProjAxis[3] = XMVector3TransformCoord(pVtx[1].vPos, matRight) - XMVector3TransformCoord(pVtx[0].vPos, matRight);

	//// �浹ü�� �߽��� �մ� ����
	//Vec3 vCenter = XMVector3TransformCoord(Vec3(0.f, 0.f, 0.f), matLeft) - XMVector3TransformCoord(Vec3(0.f, 0.f, 0.f), matRight);

	//// ����
	//for (int i = 0; i < 4; ++i)
	//{
	//	Vec3 vProj = vProjAxis[i];
	//	vProj.Normalize();

	//	float dot = fabs(vProjAxis[0].Dot(vProj));
	//	dot += fabs(vProjAxis[1].Dot(vProj));
	//	dot += fabs(vProjAxis[2].Dot(vProj));
	//	dot += fabs(vProjAxis[3].Dot(vProj));
	//	dot /= 2.f;

	//	float fCenter = fabs(vCenter.Dot(vProj));

	//	if (dot <= fCenter)
	//		return false;
	//}

	//return true;



	// Overlap ��� ������ �ڵ�
	// BOD �˰���

	// �浹ü�� �⺻ ���� ������ �����´�.
	//Ptr<CMesh> pRectMesh = CAssetMgr::GetInst()->FindAsset<CMesh>(L"RectMesh");
	//Vtx* pVtx = (Vtx*)pRectMesh->GetVtxSysMem();

	//// �� �浹ü�� ���� ����� �����´�.
	//const Matrix& matLeft = _Left->GetWorldMat();
	//const Matrix& matRight = _Right->GetWorldMat();

	//// �⺻ ����(Rect) �� �� �浹ü�� ��������� ���ؼ�, �浹ü�� �� �𼭸� ��ġ�� �ű� ��,
	//// ��ǥ���� ��ġ���� ���� �浹ü�� ����󿡼��� ��ġ���� ������ ǥ�� ���⺤�͸� ���Ѵ�.
	//// �� ���ʹ� �浹ü���� ������ų ���� �� ����
	//Vec3 vProjAxis[4] = {};

	//vProjAxis[0] = XMVector3TransformCoord(pVtx[3].vPos, matLeft) - XMVector3TransformCoord(pVtx[0].vPos, matLeft);
	//vProjAxis[1] = XMVector3TransformCoord(pVtx[1].vPos, matLeft) - XMVector3TransformCoord(pVtx[0].vPos, matLeft);

	//vProjAxis[2] = XMVector3TransformCoord(pVtx[3].vPos, matRight) - XMVector3TransformCoord(pVtx[0].vPos, matRight);
	//vProjAxis[3] = XMVector3TransformCoord(pVtx[1].vPos, matRight) - XMVector3TransformCoord(pVtx[0].vPos, matRight);

	//// �浹ü�� �߽��� �մ� ����
	//Vec3 vCenter = XMVector3TransformCoord(Vec3(0.f, 0.f, 0.f), matLeft) - XMVector3TransformCoord(Vec3(0.f, 0.f, 0.f), matRight);

	//// ������ �Ÿ� �ʱ�ȭ
	//float minOverlap = FLT_MAX; // �ּ� �������� �ſ� ū ������ �ʱ�ȭ
	//Vec3 smallestOverlapAxis; // �ּ� �������� �߻��� ��

	//// ����
	//for (int i = 0; i < 4; ++i)
	//{
	//	Vec3 vProj = vProjAxis[i];
	//	vProj.Normalize();

	//	float dot = fabs(vProjAxis[0].Dot(vProj));
	//	dot += fabs(vProjAxis[1].Dot(vProj));
	//	dot += fabs(vProjAxis[2].Dot(vProj));
	//	dot += fabs(vProjAxis[3].Dot(vProj));
	//	dot /= 2.f;

	//	float fCenter = fabs(vCenter.Dot(vProj));

	//	float overlap = dot - fCenter; // ������ ���

	//	if (overlap <= 0) {
	//		return false; // �浹�� �߻����� ����
	//	}

	//	// �ּ� ������ �Ÿ� �� �� ������Ʈ
	//	if (overlap < minOverlap) {
	//		minOverlap = overlap;
	//		smallestOverlapAxis = vProj;
	//	}
	//}

	//// �ּ� ������ ��� �Ÿ��� ��ȯ
	//m_vOverlap = smallestOverlapAxis * minOverlap;
	//return true;

	// ========================================================================================================
	// Overlap ��� ������ �ڵ�


	//// �浹ü�� �⺻ ���� ������ �����´�.
	//Ptr<CMesh> pRectMesh = CAssetMgr::GetInst()->FindAsset<CMesh>(L"RectMesh");
	//Vtx* pVtx = (Vtx*)pRectMesh->GetVtxSysMem();

	//// �� �浹ü�� ���� ����� �����´�.
	//const Matrix& matLeft = _Left->GetWorldMat();
	//const Matrix& matRight = _Right->GetWorldMat();

	//// �⺻ ����(Rect) �� �� �浹ü�� ��������� ���ؼ�, �浹ü�� �� �𼭸� ��ġ�� �ű� ��,
	//// ��ǥ���� ��ġ���� ���� �浹ü�� ����󿡼��� ��ġ���� ������ ǥ�� ���⺤�͸� ���Ѵ�.
	//// �� ���ʹ� �浹ü���� ������ų ���� �� ����
	//Vec3 vProjAxis[4] = {};

	//vProjAxis[0] = XMVector3TransformCoord(pVtx[3].vPos, matLeft) - XMVector3TransformCoord(pVtx[0].vPos, matLeft);
	//vProjAxis[1] = XMVector3TransformCoord(pVtx[1].vPos, matLeft) - XMVector3TransformCoord(pVtx[0].vPos, matLeft);

	//vProjAxis[2] = XMVector3TransformCoord(pVtx[3].vPos, matRight) - XMVector3TransformCoord(pVtx[0].vPos, matRight);
	//vProjAxis[3] = XMVector3TransformCoord(pVtx[1].vPos, matRight) - XMVector3TransformCoord(pVtx[0].vPos, matRight);

	//// �浹ü�� �߽��� �մ� ����
	//Vec3 vCenter = XMVector3TransformCoord(Vec3(0.f, 0.f, 0.f), matLeft) - XMVector3TransformCoord(Vec3(0.f, 0.f, 0.f), matRight);

	//// ������ �Ÿ� �ʱ�ȭ
	//int minOverlap = INT_MAX; // �ּ� �������� �ſ� ū ������ �ʱ�ȭ
	//Vec3 smallestOverlapAxis; // �ּ� �������� �߻��� ��

	//// ����
	//for (int i = 0; i < 4; ++i)
	//{
	//	Vec3 vProj = vProjAxis[i];
	//	vProj.Normalize();

	//	// float ����� int�� ��ȯ
	//	int dot = static_cast<int>(fabs(vProjAxis[0].Dot(vProj)));
	//	dot += static_cast<int>(fabs(vProjAxis[1].Dot(vProj)));
	//	dot += static_cast<int>(fabs(vProjAxis[2].Dot(vProj)));
	//	dot += static_cast<int>(fabs(vProjAxis[3].Dot(vProj)));
	//	dot /= 2;

	//	int fCenter = static_cast<int>(fabs(vCenter.Dot(vProj)));

	//	int overlap = dot - fCenter; // ������ ���

	//	if (overlap <= 0) {
	//		return false; // �浹�� �߻����� ����
	//	}

	//	// �ּ� ������ �Ÿ� �� �� ������Ʈ
	//	if (overlap < minOverlap) {
	//		minOverlap = overlap;
	//		smallestOverlapAxis = vProj;
	//	}
	//}

	//// �ּ� ������ ��� �Ÿ��� ��ȯ
	//m_vOverlap = smallestOverlapAxis * static_cast<float>(minOverlap);
	//return true;



	// ========================================================================================================
	// int Ÿ�� �⺻ Rect �浹

	Vec3 vLeftWorldPos = _Left->GetWorldPos();
	Vec3 vLeftWorldScale = _Left->GetWorldScale();
	Vec3 vRightWorldPos = _Right->GetWorldPos();
	Vec3 vRightWorldScale = _Right->GetWorldScale();

	if (abs(vRightWorldPos.x - vLeftWorldPos.x) <= (abs(vLeftWorldScale.x + vRightWorldScale.x) * 0.5f) &&
		abs(vRightWorldPos.y - vLeftWorldPos.y) <= (abs(vLeftWorldScale.y + vRightWorldScale.y) * 0.5f))
	{
		// Overlap ���� ���
		float fLeft_LeftBorder = vLeftWorldPos.x - (vLeftWorldScale.x * 0.5f);
		float fLeft_RightBorder = vLeftWorldPos.x + (vLeftWorldScale.x * 0.5f);
		float fLeft_TopBorder = vLeftWorldPos.y + (vLeftWorldScale.y * 0.5f);
		float fLeft_BottomBorder = vLeftWorldPos.y - (vLeftWorldScale.y * 0.5f);

		float fRight_LeftBorder = vRightWorldPos.x - (vRightWorldScale.x * 0.5f);
		float fRight_RightBorder = vRightWorldPos.x + (vRightWorldScale.x * 0.5f);
		float fRight_TopBorder = vRightWorldPos.y + (vRightWorldScale.y * 0.5f);
		float fRight_BottomBorder = vRightWorldPos.y - (vRightWorldScale.y * 0.5f);

		float fOverlap_Left = max(fLeft_LeftBorder, fRight_LeftBorder);
		float fOverlap_Right = min(fLeft_RightBorder, fRight_RightBorder);
		float fOverlap_Top = min(fLeft_TopBorder, fRight_TopBorder);
		float fOverlap_Bottom = max(fLeft_BottomBorder, fRight_BottomBorder);

		// �浹 ���� ���
		float fOverlapWidth = fOverlap_Right - fOverlap_Left;
		float fOverlapHeight = fOverlap_Top - fOverlap_Bottom;

		m_OverlapArea = Vec2(fOverlapWidth, fOverlapHeight);

		return true;
	}
	return false;

}