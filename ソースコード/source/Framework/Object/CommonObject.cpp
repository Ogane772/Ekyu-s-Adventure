#include "CommonObject.h"
#include "../Shadow/ShadowType.h"
#include "../Collision/CollisionTag.h"
#include "../Collision/Collision.h"
#include "../Scene/SceneManager.h"
#include "../Scene/Scene.h"
#include "../Model/StaticMesh.h"
#include "../Renderer/Renderer.h"
#include "../../Game/Character/Enemy/Enemy.h"
#include "../../Game/Character/Player/Player.h"

#ifdef _DEBUG
#ifndef DBG_NEW
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#define new DBG_NEW
#endif
#endif

void CCommonObject::Init(const EStaticMeshType Type, const Vector3& SetPos, const Vector3& SetScale, const Vector3& SetRot)
{
	m_Position = SetPos;
	m_Scale = SetScale;
	m_Rotation = SetRot;
	m_CullingRadius = 15.0f;
	UpdateWorldMatrix();
	m_Model = std::make_unique<CStaticMesh>(Type);
	if (m_Model)
	{
		m_Model->CreateCollisionMesh();
		m_Model->SetAlpha(1.0f);
		m_Model->Update(m_World);
		m_MeshWorld = m_Model->GetWorld();
	}
}

void CCommonObject::CreateCullingSphere(const float Radius, const Vector3& SetPos)
{
	m_CullingSphere = std::make_unique<CSphere>(m_Position, Radius);
	if (m_CullingSphere)
	{
		m_CullingSphere->SetAddSpherePosition(SetPos);
		m_CullingRadius = m_CullingSphere->GetRadius();
	}
	m_IsOverrideCullingPosition = true;
}

void CCommonObject::CreateAutoCullingSphere()
{
	m_CullingSphere = std::make_unique<CSphere>();
	if (m_CullingSphere)
	{
		m_CullingSphere->AutoSphereCreate(m_Position, m_Scale, m_Model->GetMeshTriangleList());
		m_CullingSphere->Update(m_Position);
		m_CullingRadius = m_CullingSphere->GetRadius();
	}
	m_IsOverrideCullingPosition = true;
}

void CCommonObject::Update(float DeltaTime)
{
	if (m_CullingSphere)
	{
		m_CullingSphere->Update(m_Position);
		m_OverrideCullingPosition = m_Position + m_CullingSphere->GetAddPosition();
		m_CullingRadius = m_CullingSphere->GetRadius();
	}

	UpdateWorldMatrix();
	if (m_Model)
	{
		m_Model->Update(m_World);
		m_MeshWorld = m_Model->GetWorld();
	}
}

void CCommonObject::Draw()
{
	if (!m_Model)
	{
		return;
	}

	if (m_IsEditorOnlyDraw && !IsEditMode())
	{
		return;
	}

	if (m_IsDisableDepth)
	{
		CRenderer::SetDepthEnable(false);
	}

	if (!m_IsDisableShadow && !m_IsEditorDisableShadow)
	{
		m_Model->ShadowOnModelDraw(EShadowType::NO_DOWN_SHADOW, m_World);
	}
	else
	{
		m_Model->Draw(m_World);
	}
	if (m_IsDisableDepth)
	{
		CRenderer::SetDepthEnable(true);
	}

	if (m_CullingSphere)
	{
		m_CullingSphere->Draw();
	}
}

void CCommonObject::SingleCollisonDraw()
{
	if (m_CullingSphere)
	{
		m_CullingSphere->SingleDraw();
	}
}

void CCommonObject::ShadowDraw()
{
	if (!m_Model)
	{
		return;
	}
	if (m_IsEditorOnlyDraw && !IsEditMode())
	{
		return;
	}
	if (m_IsDisableShadow || m_IsEditorDisableShadow)
	{
		return;
	}
	m_Model->ShadowDraw(EShadowType::NO_DOWN_SHADOW, m_World);
}

void CCommonObject::SetPosition(const Vector3& SetPos)
{
	m_Position = SetPos;
	if (m_CullingSphere)
	{
		m_CullingSphere->SetPosition(m_Position);
	}
}

void CCommonObject::SetRotation(const Vector3& SetRot)
{
	m_Rotation = SetRot;
}

bool CCommonObject::IsLoadNormalMap() const
{
	if (!m_Model)
	{
		return false;
	}
	return m_Model->IsLoadNormalMap();
}

ShaderNormalMapData* CCommonObject::GetNormalMapPower() const
{
	if (!m_Model)
	{
		return nullptr;
	}
	return m_Model->GetNormalMapPower();
}

void CCommonObject::SetNormalMapPower(ShaderNormalMapData& NormalMapData)
{
	if (m_Model)
	{
		m_Model->SetNormalMapPower(NormalMapData);
	}
}

std::vector<CollisionMesh>* CCommonObject::GetNormalModelMeshCollision()
{
	if (!m_Model)
	{
		return nullptr;
	}
	return m_Model->GetNormalModelMeshCollision();
}

MATERIAL* CCommonObject::GetMaterial() const
{
	if (!m_Model)
	{
		return nullptr;
	}
	return m_Model->GetMaterial();
}

void CCommonObject::SetNoCullingCheck(const bool Type)
{
	m_IsNoCullingCheck = Type;
	if (Type)
	{
		m_IsCulling = true;
	}
}

void CCommonObject::ChangeTexture(const ETextureType Type, const int Index)
{
	if (m_Model)
	{
		m_Model->ChangeTexture(Type, Index);
	}
}

void CCommonObject::ChangeSphereTexture(const ETextureType Type)
{
	if (m_CullingSphere)
	{
		m_CullingSphere->ChangeSphereTexture(Type);
	}
}

void CCommonObject::SetAlpha(const float Alpha)
{
	if (m_Model)
	{
		m_Model->SetAlpha(Alpha);
	}
}

bool* CCommonObject::IsEnableLight()
{
	if (!m_Model)
	{
		return nullptr;
	}
	return m_Model->IsEnableLight();
}

void CCommonObject::SetEnableLight(const bool Enable)
{
	if (m_Model)
	{
		m_Model->SetEnableLight(Enable);
	}
}

bool* CCommonObject::IsDisableSelfShadow() const
{
	if (!m_Model)
	{
		return nullptr;
	}
	return m_Model->IsDisableSelfShadow();
}

void CCommonObject::SetDisableSelfShadow(const bool Disable)
{
	if (!m_Model)
	{
		return;
	}
	m_Model->SetDisableSelfShadow(Disable);
}

bool* CCommonObject::IsEnableNormalMap() const
{
	if (!m_Model)
	{
		return nullptr;
	}
	return m_Model->IsEnableNormalMap();
}

void CCommonObject::SetEnableNormalMap(const bool Enable)
{
	if (m_Model)
	{
		m_Model->SetEnableNormalMap(Enable);
	}
}

void CCommonObject::SetMaterial(const MATERIAL& Material)
{
	if (m_Model)
	{
		m_Model->SetMaterial(Material);
	}
}

void CCommonObject::UpdateAutoCullingSphere()
{
	if (m_CullingSphere)
	{
		m_CullingSphere.reset();
	}
	m_CullingSphere = std::make_unique<CSphere>();
	if (m_CullingSphere)
	{
		m_CullingSphere->AutoSphereCreate(m_Position, m_Scale, m_Model->GetMeshTriangleList());
		m_CullingSphere->Update(m_Position);
		m_CullingRadius = m_CullingSphere->GetRadius();
	}
	m_IsOverrideCullingPosition = true;
}

void CCommonObject::SetCullingRadius(const float Radius)
{
	m_CullingRadius = Radius;
	if (m_CullingSphere)
	{
		m_CullingSphere->SetRadius(Radius);
	}
}

EStaticMeshType CCommonObject::GetMeshType() const
{
	if (!m_Model)
	{
		return EStaticMeshType();
	}
	return m_Model->GetMeshType();
}