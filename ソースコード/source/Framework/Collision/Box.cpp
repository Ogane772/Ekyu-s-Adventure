#include "Box.h"
#include "../Scene/SceneManager.h"
#include "../Renderer/Renderer.h"
#include "../System/SystemContext.h"

CBox::CBox(const Vector3& SetPos, const Vector3& Max, const Vector3& Min)
{
	m_Model = std::make_unique<CStaticMesh>(EStaticMeshType::BOX);
	m_Position = SetPos;
	m_BoxData.Max = Max;
	m_BoxData.Min = Min;
	//軸ベクトル 軸の長さ（この場合ボックスの各半径）を初期化する
	m_BoxData.Length = (m_BoxData.Max - m_BoxData.Min) * 0.5f;

	UpdateAxis();

	m_Rotation = Vector3(0.0f, 0.0f, 0.0f);
	m_Scale = m_BoxData.Length;
}

void CBox::CommonAutoBoxCreate(const bool IsAddBoxSize, const Vector3& Position, const Vector3& AddBoxSize, const Vector3& SetRot, const Vector3& SetScale, const std::vector<std::vector<Vector3>>* Vertex)
{
	if (!Vertex)
	{
		return;
	}
	//最大値、最小値の初期値設定
	Vector3 max = Vector3(-10000.0f, -10000.0f, -10000.0f);
	Vector3 min = Vector3(10000.0f, 10000.0f, 10000.0f);

	int j, i;
	const int sizeA = (int)Vertex->size();

	for (j = 0; j < sizeA; j++)
	{
		const std::vector<Vector3>& list = (*Vertex)[j];
		const int sizeB = (int)list.size();
		//最大値、最小値取得ループ
		for (i = 0; i < sizeB; i++)
		{
			Vector3 pos = list[i];
			if (pos.x < min.x)min.x = pos.x;
			if (pos.x > max.x)max.x = pos.x;
			if (pos.y < min.y)min.y = pos.y;
			if (pos.y > max.y)max.y = pos.y;
			if (pos.z < min.z)min.z = pos.z;
			if (pos.z > max.z)max.z = pos.z;
		}
	}
	//方向ベクトル取得
	m_Rotation = SetRot;
	m_BoxData.Rot = XMMatrixRotationRollPitchYaw(XMConvertToRadians(m_Rotation.x), XMConvertToRadians(m_Rotation.y), XMConvertToRadians(m_Rotation.z));
	UpdateAxis();
	//長さ取得
	m_BoxData.Length.x = (fabsf(max.x - min.x) * 0.5f) * SetScale.x + AddBoxSize.x;
	m_BoxData.Length.y = (fabsf(max.y - min.y) * 0.5f) * SetScale.y + AddBoxSize.y;
	m_BoxData.Length.z = (fabsf(max.z - min.z) * 0.5f) * SetScale.z + AddBoxSize.z;
	if (IsAddBoxSize)
	{
		m_Position = ((min + max) * 0.5f) + Position;
	}
	else
	{
		m_Position = Position;
	}
	m_Scale = m_BoxData.Length;
	if (!m_Model)
	{
		m_Model = std::make_unique<CStaticMesh>(EStaticMeshType::BOX);
	}
	m_BoxData.Max = max * SetScale;
	m_BoxData.Max.x += 1.0f;
	m_BoxData.Max.y += 1.0f;
	m_BoxData.Max.z += 1.0f;
	m_BoxData.Min = min * SetScale;
	m_BoxData.Min.x -= 1.0f;
	m_BoxData.Min.y -= 1.0f;
	m_BoxData.Min.z -= 1.0f;
}

void CBox::AutoBoxCreate(const Vector3& SetPos, const Vector3& SetRot, const Vector3& SetScale, const std::vector<std::vector<Vector3>>* Vertex)
{
	CommonAutoBoxCreate(false, SetPos, Vector3::One(), SetRot, SetScale, Vertex);
}

void CBox::AutoBoxCreate(const Vector3& AddBoxSize, const Vector3& SetPos, const Vector3& SetRot, const Vector3& SetScale, const std::vector<std::vector<Vector3>>* Vertex)
{
	CommonAutoBoxCreate(true, SetPos, AddBoxSize, SetRot, SetScale, Vertex);
}

void CBox::Update(const Vector3& Position, const Vector3& Rotation)
{
	m_Position = Position + m_BoxData.AddPosition;
	m_Rotation = Rotation;
	m_BoxData.Rot = XMMatrixRotationRollPitchYaw(XMConvertToRadians(m_Rotation.x), XMConvertToRadians(m_Rotation.y), XMConvertToRadians(m_Rotation.z));

	UpdateAxis();
}

void CBox::CollisionDraw()
{
	UpdateWorldMatrix();
	CRenderer::SetShader();
	CRenderer::SetWireframeMode();
	if (m_Model)
	{
		m_Model->Draw(m_World);
	}
	CRenderer::SetSolidMode();
}

void CBox::Draw()
{
	CSystemContext* system = CSystemContext::GetInstance();
	if (!system)
	{
		return;
	}
	if (!system->IsDebugCollisionRender())
	{
		return;
	}
	
	CollisionDraw();
}

void CBox::SingleDraw()
{
	CSystemContext* system = CSystemContext::GetInstance();
	if (!system)
	{
		return;
	}
	if (!system->IsDebugCollisionRender() && system->IsEditMode())
	{
		CollisionDraw();
	}
}

void CBox::AddBoxPosition(const Vector3& AddPos)
{
	m_BoxData.AddPosition = AddPos;
}

void CBox::SetPosition(const Vector3& SetPos)
{
	m_Position =  SetPos + m_BoxData.AddPosition;
}

void CBox::AddPosition(const Vector3& AddPos)
{
	m_Position += AddPos;
}

void CBox::SetBoxSize(const Vector3& Max, const Vector3& Min)
{
	m_BoxData.Max = Max;
	m_BoxData.Min = Min;

	m_BoxData.Length = (m_BoxData.Max - m_BoxData.Min) / 2;

	UpdateAxis();
	m_Scale = m_BoxData.Length;
}

void CBox::UpdateAxis()
{
	//それぞれのローカル基底軸ベクトルに、それぞれの回転を反映させる
	m_BoxData.AxisX = Vector3::Right();
	m_BoxData.AxisY = Vector3::Up();
	m_BoxData.AxisZ = Vector3::Forward();

	m_BoxData.AxisX = m_BoxData.Rot * m_BoxData.AxisX;
	m_BoxData.AxisY = m_BoxData.Rot * m_BoxData.AxisY;
	m_BoxData.AxisZ = m_BoxData.Rot * m_BoxData.AxisZ;
}