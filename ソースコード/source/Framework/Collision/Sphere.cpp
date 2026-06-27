#include "Sphere.h"
#include "../Scene/SceneManager.h"
#include "../Renderer/Renderer.h"
#include "../System/SystemContext.h"

CSphere::CSphere(const Vector3& SetPos, const float Radius)
{
	m_Model = std::make_unique<CStaticMesh>(EStaticMeshType::SPHERE);
	m_Position = SetPos;
	m_SphereData.Radius = Radius;
	m_Rotation = Vector3(0.0f, 0.0f, 0.0f);
	m_Scale = Vector3(Radius, Radius, Radius);
}

void CSphere::AutoSphereCreate(const Vector3& SetPos, const Vector3& SetScale, const std::vector<std::vector<Vector3>>* Vertex)
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
		const std::vector<Vector3 >& list = (*Vertex)[j];
		const int sizeB = (int)list.size();
		//最大値、最小値取得ループ
		for (i = 0; i < sizeB; i++)
		{
			const Vector3 pos = list[i];
			if (pos.x < min.x)min.x = pos.x;
			if (pos.x > max.x)max.x = pos.x;
			if (pos.y < min.y)min.y = pos.y;
			if (pos.y > max.y)max.y = pos.y;
			if (pos.z < min.z)min.z = pos.z;
			if (pos.z > max.z)max.z = pos.z;
		}
	}
	max = max * SetScale;
	min = min * SetScale;
	//中心位置を求める
	m_Position = ((min + max) * 0.5f);

	m_SphereData.AddPosition = m_Position;
	const Vector3 length = m_Position - max;
	//1.1fはカリング用に幅を持たせる
	m_SphereData.Radius = length.Length() * 1.1f;

	if (!m_Model)
	{
		m_Model = std::make_unique<CStaticMesh>(EStaticMeshType::SPHERE);
	}

	m_Rotation = Vector3(0.0f, 0.0f, 0.0f);
	m_Scale = Vector3(m_SphereData.Radius, m_SphereData.Radius, m_SphereData.Radius);
}

void CSphere::Update(const Vector3 Position)
{
	m_Position = Position + m_SphereData.AddPosition;
}

void CSphere::CollisionDraw()
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

void CSphere::Draw()
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

void CSphere::SingleDraw()
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

void CSphere::SetAddSpherePosition(const Vector3& AddPos)
{
	m_SphereData.AddPosition = AddPos;
}

void CSphere::SetRadius(const float Radius)
{
	m_SphereData.Radius = Radius;
	m_Scale = Vector3(Radius, Radius, Radius);
}

void CSphere::ChangeSphereTexture(const ETextureType Type)
{
	if (m_Model)
	{
		m_Model->ChangeTexture(Type, 0);
	}
}