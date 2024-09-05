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
	// 현재 레벨 가져온다.
	CLevel* pCurLevel = CLevelMgr::GetInst()->GetCurrentLevel();
	if (nullptr == pCurLevel)
		return;

	// 각 레이어에 속한 모든 오브젝트들을 가져온다.
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

			// 등록된적이 없으면 등록시킨다.
			if (iter == m_mapCollisionInfo.end())
			{
				m_mapCollisionInfo.insert(make_pair(id.ID, false));
				iter = m_mapCollisionInfo.find(id.ID);
			}

			bool bDead = vecLeft[i]->IsDead() || vecRight[j]->IsDead();
			//bool bDeactive = !vecLeft[i]->IsActive() || !vecRight[j]->IsActive();

			// 두 충돌체가 지금 충돌중이다.
			if (IsCollision(pLeftCol, pRightCol))
			{
				// 이전에도 충돌중이었다.
				if (iter->second)
				{
					// 두 충돌체중 하나라도 Dead 상태거나 비활성화 상태라면
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

				// 이전에는 충돌중이 아니었다.
				else
				{
					// 두 충돌체중 하나라도 Dead 상태거나 비활성화 상태라면
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

				// 두 충돌체중 하나라도 Dead 상태거나 비활성화 상태라면
				// 추가로 충돌 해제를 호출시켜준다.
				/*if (bDead || bDeactive)
				{
					vecLeft[i]->EndOverlap(vecRight[j]);
					vecRight[j]->EndOverlap(vecLeft[i]);
					iter->second = false;
				}*/
			}

			// 두 충돌체가 지금 충돌중이 아니다.
			else
			{
				// 이전에는 충돌중이었다.
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
	//// BOD 알고리즘
	//
	//// 충돌체의 기본 원형 도형을 가져온다.
	//Ptr<CMesh> pRectMesh = CAssetMgr::GetInst()->FindAsset<CMesh>(L"RectMesh");
	//Vtx* pVtx = (Vtx*)pRectMesh->GetVtxSysMem();

	//// 각 충돌체의 월드 행렬을 가져온다.
	//const Matrix& matLeft = _Left->GetWorldMat();
	//const Matrix& matRight = _Right->GetWorldMat();

	//// 기본 도형(Rect) 를 각 충돌체의 월드행렬을 곱해서, 충돌체의 각 모서리 위치로 옮긴 후,
	//// 좌표끼리 위치값을 빼서 충돌체의 월드상에서의 위치에서 도형의 표면 방향벡터를 구한다.
	//// 이 벡터는 충돌체들을 투영시킬 축이 될 예정
	//Vec3 vProjAxis[4] = {};

	//vProjAxis[0] = XMVector3TransformCoord(pVtx[3].vPos, matLeft) - XMVector3TransformCoord(pVtx[0].vPos, matLeft);
	//vProjAxis[1] = XMVector3TransformCoord(pVtx[1].vPos, matLeft) - XMVector3TransformCoord(pVtx[0].vPos, matLeft);

	//vProjAxis[2] = XMVector3TransformCoord(pVtx[3].vPos, matRight) - XMVector3TransformCoord(pVtx[0].vPos, matRight);
	//vProjAxis[3] = XMVector3TransformCoord(pVtx[1].vPos, matRight) - XMVector3TransformCoord(pVtx[0].vPos, matRight);

	//// 충돌체의 중심을 잇는 벡터
	//Vec3 vCenter = XMVector3TransformCoord(Vec3(0.f, 0.f, 0.f), matLeft) - XMVector3TransformCoord(Vec3(0.f, 0.f, 0.f), matRight);

	//// 투영
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



	// Overlap 계산 가능한 코드
	// BOD 알고리즘

	// 충돌체의 기본 원형 도형을 가져온다.
	//Ptr<CMesh> pRectMesh = CAssetMgr::GetInst()->FindAsset<CMesh>(L"RectMesh");
	//Vtx* pVtx = (Vtx*)pRectMesh->GetVtxSysMem();

	//// 각 충돌체의 월드 행렬을 가져온다.
	//const Matrix& matLeft = _Left->GetWorldMat();
	//const Matrix& matRight = _Right->GetWorldMat();

	//// 기본 도형(Rect) 를 각 충돌체의 월드행렬을 곱해서, 충돌체의 각 모서리 위치로 옮긴 후,
	//// 좌표끼리 위치값을 빼서 충돌체의 월드상에서의 위치에서 도형의 표면 방향벡터를 구한다.
	//// 이 벡터는 충돌체들을 투영시킬 축이 될 예정
	//Vec3 vProjAxis[4] = {};

	//vProjAxis[0] = XMVector3TransformCoord(pVtx[3].vPos, matLeft) - XMVector3TransformCoord(pVtx[0].vPos, matLeft);
	//vProjAxis[1] = XMVector3TransformCoord(pVtx[1].vPos, matLeft) - XMVector3TransformCoord(pVtx[0].vPos, matLeft);

	//vProjAxis[2] = XMVector3TransformCoord(pVtx[3].vPos, matRight) - XMVector3TransformCoord(pVtx[0].vPos, matRight);
	//vProjAxis[3] = XMVector3TransformCoord(pVtx[1].vPos, matRight) - XMVector3TransformCoord(pVtx[0].vPos, matRight);

	//// 충돌체의 중심을 잇는 벡터
	//Vec3 vCenter = XMVector3TransformCoord(Vec3(0.f, 0.f, 0.f), matLeft) - XMVector3TransformCoord(Vec3(0.f, 0.f, 0.f), matRight);

	//// 오버랩 거리 초기화
	//float minOverlap = FLT_MAX; // 최소 오버랩을 매우 큰 값으로 초기화
	//Vec3 smallestOverlapAxis; // 최소 오버랩이 발생한 축

	//// 투영
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

	//	float overlap = dot - fCenter; // 오버랩 계산

	//	if (overlap <= 0) {
	//		return false; // 충돌이 발생하지 않음
	//	}

	//	// 최소 오버랩 거리 및 축 업데이트
	//	if (overlap < minOverlap) {
	//		minOverlap = overlap;
	//		smallestOverlapAxis = vProj;
	//	}
	//}

	//// 최소 오버랩 축과 거리를 반환
	//m_vOverlap = smallestOverlapAxis * minOverlap;
	//return true;

	// ========================================================================================================
	// Overlap 계산 가능한 코드


	//// 충돌체의 기본 원형 도형을 가져온다.
	//Ptr<CMesh> pRectMesh = CAssetMgr::GetInst()->FindAsset<CMesh>(L"RectMesh");
	//Vtx* pVtx = (Vtx*)pRectMesh->GetVtxSysMem();

	//// 각 충돌체의 월드 행렬을 가져온다.
	//const Matrix& matLeft = _Left->GetWorldMat();
	//const Matrix& matRight = _Right->GetWorldMat();

	//// 기본 도형(Rect) 를 각 충돌체의 월드행렬을 곱해서, 충돌체의 각 모서리 위치로 옮긴 후,
	//// 좌표끼리 위치값을 빼서 충돌체의 월드상에서의 위치에서 도형의 표면 방향벡터를 구한다.
	//// 이 벡터는 충돌체들을 투영시킬 축이 될 예정
	//Vec3 vProjAxis[4] = {};

	//vProjAxis[0] = XMVector3TransformCoord(pVtx[3].vPos, matLeft) - XMVector3TransformCoord(pVtx[0].vPos, matLeft);
	//vProjAxis[1] = XMVector3TransformCoord(pVtx[1].vPos, matLeft) - XMVector3TransformCoord(pVtx[0].vPos, matLeft);

	//vProjAxis[2] = XMVector3TransformCoord(pVtx[3].vPos, matRight) - XMVector3TransformCoord(pVtx[0].vPos, matRight);
	//vProjAxis[3] = XMVector3TransformCoord(pVtx[1].vPos, matRight) - XMVector3TransformCoord(pVtx[0].vPos, matRight);

	//// 충돌체의 중심을 잇는 벡터
	//Vec3 vCenter = XMVector3TransformCoord(Vec3(0.f, 0.f, 0.f), matLeft) - XMVector3TransformCoord(Vec3(0.f, 0.f, 0.f), matRight);

	//// 오버랩 거리 초기화
	//int minOverlap = INT_MAX; // 최소 오버랩을 매우 큰 값으로 초기화
	//Vec3 smallestOverlapAxis; // 최소 오버랩이 발생한 축

	//// 투영
	//for (int i = 0; i < 4; ++i)
	//{
	//	Vec3 vProj = vProjAxis[i];
	//	vProj.Normalize();

	//	// float 결과를 int로 변환
	//	int dot = static_cast<int>(fabs(vProjAxis[0].Dot(vProj)));
	//	dot += static_cast<int>(fabs(vProjAxis[1].Dot(vProj)));
	//	dot += static_cast<int>(fabs(vProjAxis[2].Dot(vProj)));
	//	dot += static_cast<int>(fabs(vProjAxis[3].Dot(vProj)));
	//	dot /= 2;

	//	int fCenter = static_cast<int>(fabs(vCenter.Dot(vProj)));

	//	int overlap = dot - fCenter; // 오버랩 계산

	//	if (overlap <= 0) {
	//		return false; // 충돌이 발생하지 않음
	//	}

	//	// 최소 오버랩 거리 및 축 업데이트
	//	if (overlap < minOverlap) {
	//		minOverlap = overlap;
	//		smallestOverlapAxis = vProj;
	//	}
	//}

	//// 최소 오버랩 축과 거리를 반환
	//m_vOverlap = smallestOverlapAxis * static_cast<float>(minOverlap);
	//return true;



	// ========================================================================================================
	// int 타입 기본 Rect 충돌

	Vec3 vLeftWorldPos = _Left->GetWorldPos();
	Vec3 vLeftWorldScale = _Left->GetWorldScale();
	Vec3 vRightWorldPos = _Right->GetWorldPos();
	Vec3 vRightWorldScale = _Right->GetWorldScale();

	if (abs(vRightWorldPos.x - vLeftWorldPos.x) <= (abs(vLeftWorldScale.x + vRightWorldScale.x) * 0.5f) &&
		abs(vRightWorldPos.y - vLeftWorldPos.y) <= (abs(vLeftWorldScale.y + vRightWorldScale.y) * 0.5f))
	{
		// Overlap 영역 계산
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

		// 충돌 영역 계산
		float fOverlapWidth = fOverlap_Right - fOverlap_Left;
		float fOverlapHeight = fOverlap_Top - fOverlap_Bottom;

		m_OverlapArea = Vec2(fOverlapWidth, fOverlapHeight);

		return true;
	}
	return false;

}