#include "pch.h"
#include "CTransform.h"

#include "CDevice.h"
#include "CConstBuffer.h"
#include "CEngine.h"

CTransform::CTransform()
	: CComponent(COMPONENT_TYPE::TRANSFORM)
{
}

CTransform::~CTransform()
{
}

void CTransform::FinalTick()
{
	// ������Ʈ�� ������� ���
	// ũ�����
	Matrix matScale = XMMatrixScaling(m_RelativeScale.x, m_RelativeScale.y, m_RelativeScale.z);

	// �̵� ���
	Matrix matTranslation = XMMatrixTranslation(m_RelativePos.x, m_RelativePos.y, m_RelativePos.z);

	// ȸ�� ��� (���� ��ǥ���� X -> Y -> Z �� ������ ������Ʈ�� ȸ����Ŵ)
	Matrix matRot = XMMatrixRotationX(m_RelativeRotation.x)
				  * XMMatrixRotationY(m_RelativeRotation.y)
				  * XMMatrixRotationZ(m_RelativeRotation.z);

	m_matWorld = matScale * matRot * matTranslation;

	// ���⺤�� ���
	static Vec3 vDefaultAxis[3] =
	{
		Vec3(1.f, 0.f, 0.f),
		Vec3(0.f, 1.f, 0.f),
		Vec3(0.f, 0.f, 1.f)
	};

	for (int i = 0; i < 3; ++i)
	{
		m_RelativeDir[i] = XMVector3TransformNormal(vDefaultAxis[i], matRot);
		m_RelativeDir[i].Normalize();
	}


	// �θ� ������Ʈ�� �ִ��� Ȯ��
	if (GetOwner()->GetParent())
	{
		// �θ��� ��������� ���ؼ� ���� ��������� �����
		const Matrix& matParentWorldMat = GetOwner()->GetParent()->Transform()->GetWorldMat();
		
		if (m_IndependentScale)
		{
			Vec3 vParentScale = GetOwner()->GetParent()->Transform()->GetWorldScale();
			Matrix matParentScale = XMMatrixScaling(vParentScale.x, vParentScale.y, vParentScale.z);
			Matrix matParentScaleInv = XMMatrixInverse(nullptr, matParentScale);

			// �θ� Scaling ���
			m_matWorld = m_matWorld * matParentScaleInv * matParentWorldMat;
		}
		else
		{
			m_matWorld *= matParentWorldMat;
		}

		// ���� ������� ������Ʈ�� ���⺤�͸� ����
		for (int i = 0; i < 3; ++i)
		{
			m_WorldDir[i] = XMVector3TransformNormal(vDefaultAxis[i], m_matWorld);
		}
	}

	// �θ� ������, RelativeDir �� �� WorldDir
	else
	{
		for (int i = 0; i < 3; ++i)
		{
			m_WorldDir[i] = m_RelativeDir[i];
		}
	}

}

void CTransform::Binding()
{
	g_Trans.matWorld = m_matWorld;
	g_Trans.matWV = g_Trans.matWorld * g_Trans.matView;
	g_Trans.matWVP = g_Trans.matWV * g_Trans.matProj;

	for (int i = 0; i < 3; ++i)
	{
		g_Trans.worldDir[i] = m_WorldDir[i];
	}

	CConstBuffer* pTransformCB = CDevice::GetInst()->GetConstBuffer(CB_TYPE::TRANSFORM);
	pTransformCB->SetData(&g_Trans);
	pTransformCB->Binding();



	//// NDC ��ǥ �����
	//Vec4 worldpos(m_matWorld._41, m_matWorld._42, m_matWorld._43, 1.0f);
	//Vec4 viewPosition = XMVector3Transform(worldpos, g_Trans.matView);
	//Vec4 clipPosition = XMVector3Transform(viewPosition, g_Trans.matProj);

	//XMFLOAT4 clippos;
	//XMStoreFloat4(&clippos, clipPosition);

	//Vec3 NDC = Vec3(clippos.x, clippos.y, clippos.z) / clippos.w;

	//wstring strNDC ;
	//if (GetOwner()->GetName() == L"Player")
	//{
	//	strNDC = std::to_wstring(NDC.x) + L" ," + std::to_wstring(NDC.y) + L" ," + std::to_wstring(NDC.z) + L" ,";
	//	int a = 1;
	//}
}

void CTransform::SetWorldPos(Vec3 _Pos)
{
	// �ڽ� ������Ʈ�� ���
	if (GetOwner()->GetParent())
	{
		// ������ WorldPos
		Vec3 curWorldPos = GetWorldPos();

		// ������ ��ġ�� ���̰� ���� ������ �������� ����
		if (abs(curWorldPos.x - _Pos.x) < 0.001 && abs(curWorldPos.y - _Pos.y) < 0.001 && abs(curWorldPos.z - _Pos.z) < 0.001)
			return;			
	}

	Matrix matTargetRelativeTranslation;

	// ũ�����
	Matrix matRelativeScale = XMMatrixScaling(m_RelativeScale.x, m_RelativeScale.y, m_RelativeScale.z);

	// ũ�Ⱑ �ϳ��� 0�̸� �������� ����
	if (m_RelativeScale.x * m_RelativeScale.y * m_RelativeScale.z == 0)
		return;

	// ȸ�� ��� (���� ��ǥ���� X -> Y -> Z �� ������ ������Ʈ�� ȸ����Ŵ)
	Matrix matRelativeRot = XMMatrixRotationX(m_RelativeRotation.x)
		* XMMatrixRotationY(m_RelativeRotation.y)
		* XMMatrixRotationZ(m_RelativeRotation.z);

	Matrix matParentWorld; // Identity�� �ʱ�ȭ

	// �θ� ������Ʈ�� �ִ��� Ȯ��
	if (GetOwner()->GetParent())
	{
		// �θ��� ��������� ���ؼ� ���� ��������� �����
		if (m_IndependentScale)
		{
			matParentWorld = GetOwner()->GetParent()->Transform()->GetWorldMat();

			Vec3 vParentScale = GetOwner()->GetParent()->Transform()->GetWorldScale();
			Matrix matParentScale = XMMatrixScaling(vParentScale.x, vParentScale.y, vParentScale.z);

			// �θ� Scaling ���
			matParentWorld = matParentScale.Invert() * matParentWorld ;
		}
		else
		{
			matParentWorld = GetOwner()->GetParent()->Transform()->GetWorldMat();
		}
	}

	Matrix matTargetWorld = m_matWorld;
	matTargetWorld._41 = _Pos.x;
	matTargetWorld._42 = _Pos.y;
	matTargetWorld._43 = _Pos.z;
	
	matTargetRelativeTranslation =  matRelativeRot.Invert() * matRelativeScale.Invert() * matTargetWorld * matParentWorld.Invert();

	// �̰� �츮�� ���ϴ� WorldPos�� ���� RelativePos
	Vec3 TargetRelativePos = matTargetRelativeTranslation.Translation();


	// ���� RelativePos�� TargetRelativePos���� ���̰� �ſ� ���� ���� �������� ���� (�ֻ��� ������Ʈ�϶���)
	if (GetOwner()->GetParent() == nullptr)
	{
		if (abs(TargetRelativePos.x - m_RelativePos.x) > 0.001)
			m_RelativePos.x = TargetRelativePos.x;
		if (abs(TargetRelativePos.y - m_RelativePos.y) > 0.001)
			m_RelativePos.y = TargetRelativePos.y;
		if (abs(TargetRelativePos.z - m_RelativePos.z) > 0.001)
			m_RelativePos.z = TargetRelativePos.z;
	}
	else
	{
		m_RelativePos = TargetRelativePos;
	}



	// m_matWorld�� ���� �ٸ� ������� ������Ʈ
	FinalTick();
}

void CTransform::SetWorldScale(Vec3 _Scale)
{
	Matrix matTargetRelativeScaling;

	Matrix matTargetWorldScaling =  Matrix::CreateScale(_Scale);

	// �θ� ������Ʈ�� �ִ��� Ȯ��
	if (GetOwner()->GetParent())
	{
		// �θ��� ��������� ���ؼ� ���� ��������� �����
		if (m_IndependentScale)
		{
			m_RelativeScale = _Scale;
		}
		else
		{
			Matrix matParentWorldScaling = Matrix::CreateScale(GetOwner()->GetParent()->Transform()->GetWorldScale());
			matTargetRelativeScaling = matTargetWorldScaling * matParentWorldScaling.Invert();

			// �̰� �츮�� ���ϴ� WorldScale�� ���� RelativeScale
			m_RelativeScale = Vec3(matTargetRelativeScaling._11, matTargetRelativeScaling._22, matTargetRelativeScaling._33);
		}
	}
	else
		m_RelativeScale = _Scale;

	// m_matWorld�� ���� �ٸ� ������� ������Ʈ
	FinalTick();
}

void CTransform::SetWorldRotation(Vec3 _Rot)
{
	// ����� �۵��Ǵ��� Ȯ�� �ȵ�!

	Matrix matTargetRot;

	// ũ�����
	Matrix matRelativeScale = XMMatrixScaling(m_RelativeScale.x, m_RelativeScale.y, m_RelativeScale.z);

	// �̵� ���
	Matrix matRelativeTranslation = XMMatrixTranslation(m_RelativePos.x, m_RelativePos.y, m_RelativePos.z);

	Matrix matParentWorld; // Identity�� �ʱ�ȭ

	// �θ� ������Ʈ�� �ִ��� Ȯ��
	if (GetOwner()->GetParent())
	{
		// �θ��� ��������� ���ؼ� ���� ��������� �����
		if (m_IndependentScale)
		{
			matParentWorld = GetOwner()->GetParent()->Transform()->GetWorldMat();

			Vec3 vParentScale = GetOwner()->GetParent()->Transform()->GetWorldScale();
			Matrix matParentScale = XMMatrixScaling(vParentScale.x, vParentScale.y, vParentScale.z);

			// �θ� Scaling ���
			matParentWorld = matParentScale.Invert() * matParentWorld;
		}
		else
		{
			matParentWorld = GetOwner()->GetParent()->Transform()->GetWorldMat();
		}
	}

	// WorldScale
	Matrix matWorldScaling = XMMatrixScaling(GetWorldScale().x, GetWorldScale().y, GetWorldScale().z);

	// WorldTranslation
	Matrix matWorldTranslation = XMMatrixTranslation(GetWorldPos().x, GetWorldPos().y, GetWorldPos().z);

	// WorldRotation
	Matrix matWorldRotation = XMMatrixRotationX(_Rot.x) * XMMatrixRotationY(_Rot.y) * XMMatrixRotationZ(_Rot.z);


	matTargetRot = matRelativeScale.Invert() * (matWorldScaling * matWorldRotation * matWorldTranslation) * (matRelativeTranslation * matParentWorld).Invert();

	// matRot ��ķκ��� ���Ϸ� ������ ����
	float sy = -matTargetRot.m[2][0];
	float cy = sqrt(1 - sy * sy);

	float x, y, z;

	if (cy > 1e-6) {
		x = atan2(matTargetRot.m[2][1], matTargetRot.m[2][2]);
		y = atan2(-matTargetRot.m[2][0], cy);
		z = atan2(matTargetRot.m[1][0], matTargetRot.m[0][0]);
	}
	else {
		x = atan2(-matTargetRot.m[1][2], matTargetRot.m[1][1]);
		y = atan2(-matTargetRot.m[2][0], cy);
		z = 0; // Z�� �� �� ����
	}

	// ����� ���Ϸ� ������ Vec3�� ����
	m_RelativeRotation = Vec3(x, y, z);

	FinalTick();
}

Vec3 CTransform::GetWorldScale()
{
	Vec3 vWorldScale = Vec3(1.f, 1.f, 1.f);

	CGameObject* pObject = GetOwner();

	while (pObject)
	{
		vWorldScale *= pObject->Transform()->GetRelativeScale();

		if (pObject->Transform()->m_IndependentScale)
			break;

		pObject = pObject->GetParent();
	}

	return vWorldScale;
}

void CTransform::SaveToFile(FILE* _File)
{
	fwrite(&m_RelativePos, sizeof(Vec3), 1, _File);
	fwrite(&m_RelativeScale, sizeof(Vec3), 1, _File);
	fwrite(&m_RelativeRotation, sizeof(Vec3), 1, _File);
	fwrite(&m_IndependentScale, sizeof(bool), 1, _File);
}

void CTransform::LoadFromFile(FILE* _File)
{
	fread(&m_RelativePos, sizeof(Vec3), 1, _File);
	fread(&m_RelativeScale, sizeof(Vec3), 1, _File);
	fread(&m_RelativeRotation, sizeof(Vec3), 1, _File);
	fread(&m_IndependentScale, sizeof(bool), 1, _File);
}