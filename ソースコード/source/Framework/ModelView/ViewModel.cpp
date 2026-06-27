#include "ViewModel.h"
#include "../Shadow/ShadowType.h"
#include "../Renderer/Renderer.h"
#include "../Input/InputInterface.h"
#include "../Model/StaticMesh.h"
#include "../Model/SkeletalMesh.h"
#include "../Camera/Camera.h"
#include "../System/SystemContext.h"

void CViewModel::Init()
{
	m_IsNoCullingCheck = true;
	m_SkeletalMesh = new CSkeletalMesh(ESkeletalMeshType::PLAYER);
	m_Position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_Rotation = XMFLOAT3(0.0f, 180.0f, 0.0f);
	m_Scale = XMFLOAT3(10.0f, 10.0f, 10.0f);
	m_IsSkeletalMeshDraw = true;

	if (CCamera* camera = GetCamera())
	{
		camera->SetPosition(XMFLOAT3(0.0f, 5.5f, -8.0f));
	}
	m_FileName = "asset/player.fbx";
}

void CViewModel::UnInit()
{
	if (m_StaticMesh)
	{
		delete m_StaticMesh;
	}
	if (m_SkeletalMesh)
	{
		delete m_SkeletalMesh;
	}
}

void CViewModel::Update(float DeltaTime)
{
	CSystemContext* system = CSystemContext::GetInstance();
	if (!system)
	{
		return;
	}
	CInputInterface* instance = CInputInterface::GetInstance();
	if (!instance)
	{
		return;
	}

	//マウスによる回転
	if (instance->IsViewModelScreenClickTrigger())
	{
		RECT range = system->GetWindowRange();
		//マウスの座標を調整する
		GetCursorPos(&m_Pt);
		m_Pt.x = m_Pt.x - range.left - (range.right - range.left) / 2;
		m_Pt.y = (m_Pt.y - (range.top) - (range.bottom - (range.top)) / 2) * (-1);
	}

	if (instance->IsViewModelCameraRotationPress())
	{
		POINT point;
		RECT range = system->GetWindowRange();
		//マウスの座標を調整する
		GetCursorPos(&point);
		point.x = point.x - range.left - (range.right - range.left) / 2;
		float a = (float)(point.x - m_Pt.x);
		m_Rotation.y += a * -0.5f;

		m_Pt = point;
	}
	if (m_IsSkeletalMeshDraw)
	{
		if (m_SkeletalMesh)
		{
			m_SkeletalMesh->Update(DeltaTime);
		}
	}
	m_World = XMMatrixScaling(m_Scale.x, m_Scale.y, m_Scale.z);
	m_World *= XMMatrixRotationRollPitchYaw(XMConvertToRadians(m_Rotation.x), XMConvertToRadians(m_Rotation.y), XMConvertToRadians(m_Rotation.z));
	m_World *= XMMatrixTranslation(m_Position.x, m_Position.y + 0.2f, m_Position.z);
}

void CViewModel::Draw()
{
	// マトリクス設定
	if (!m_IsSkeletalMeshDraw)
	{
		if (m_StaticMesh)
		{
			m_StaticMesh->ShadowOnModelDraw(EShadowType::NO_DOWN_SHADOW, m_World);
		}
	}
	else
	{
		if (m_SkeletalMesh)
		{
			m_SkeletalMesh->Draw(m_World);
		}
	}
}

void CViewModel::ShadowDraw()
{
	if (!m_IsSkeletalMeshDraw)
	{
		if (m_StaticMesh)
		{
			m_StaticMesh->ShadowDraw(EShadowType::NO_DOWN_SHADOW, m_World);
		}
	}
	else
	{
		if (m_SkeletalMesh)
		{
			m_SkeletalMesh->ShadowDraw(EShadowType::NO_DOWN_SHADOW, m_World);
		}
	}
}

void CViewModel::LoadModel(const std::string& FileName)
{
	m_AnimationNumber = 0;

	Assimp::Importer importer;
	const aiScene* scene = aiImportFile(FileName.c_str(), aiProcess_Triangulate
		| aiProcess_MakeLeftHanded
		| aiProcess_FlipUVs
		| aiProcess_FlipWindingOrder
		| aiProcess_JoinIdenticalVertices
		| aiProcess_OptimizeMeshes
		| aiProcess_LimitBoneWeights
		| aiProcess_RemoveRedundantMaterials);

	//現在読み込んでいるモデルを解放
	if (m_SkeletalMesh)
	{
		delete m_SkeletalMesh;
		m_SkeletalMesh = nullptr;
	}
	if (m_StaticMesh)
	{
		delete m_StaticMesh;
		m_StaticMesh = nullptr;
	}

	if (!scene || !scene->mMeshes[0])
	{
		return;
	}

	//ボーンの数を見て通常モデルかスキンメッシュか判断
	if (scene->mMeshes[0]->mNumBones == 0)
	{//通常モデル
		m_IsSkeletalMeshDraw = false;
		m_StaticMesh = new CStaticMesh(FileName);
	}
	else
	{
		m_IsSkeletalMeshDraw = true;
		m_SkeletalMesh = new CSkeletalMesh(FileName);
	}
	aiReleaseImport(scene);
	m_FileName = FileName;
}

float CViewModel::GetAnimationSpeed() const
{ 
	if (!m_SkeletalMesh)
	{
		return 0.0f;
	}
	return m_SkeletalMesh->GetAnimationSpeed(); 
}

aiAnimation* CViewModel::GetAiAnimation() const
{
	if (!m_SkeletalMesh)
	{
		return nullptr;
	}
	return m_SkeletalMesh->GetAiAnimation(); 
}

const aiScene* CViewModel::GetAiScene() const 
{ 
	if (!m_SkeletalMesh)
	{
		return nullptr;
	}
	return m_SkeletalMesh->GetAiScene(); 
}

float CViewModel::GetAnimeFrame() const
{
	if (!m_SkeletalMesh)
	{
		return 0.0f;
	}
	return m_SkeletalMesh->GetFrame();
}

void CViewModel::SetAnimeSpeed(const float Speed)
{
	if (!m_SkeletalMesh)
	{
		return;
	}
	m_SkeletalMesh->SetAnimationSpeed(Speed);
}

void CViewModel::SetAnimationNumber(const UINT Number)
{
	const aiScene* aiScene = GetAiScene();
	if (!aiScene)
	{
		return;
	}
	//アニメーションがない番号だったらリターン
	if (Number >= aiScene->mNumAnimations || Number < 0)
	{
		return;
	}
	m_AnimationNumber = Number;
	m_SkeletalMesh->SetAnimationChange(Number);
}

void CViewModel::SetAnimeLoop(const bool IsLoop)
{
	if (!m_SkeletalMesh)
	{
		return;
	}
	m_SkeletalMesh->SetLoop(IsLoop);
}

void CViewModel::ResetAnimeFrame()
{ 
	if (!m_SkeletalMesh)
	{
		return;
	}
	m_SkeletalMesh->ResetAnimeFrame();
}