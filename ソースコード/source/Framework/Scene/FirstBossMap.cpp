#include "FirstBossMap.h"
#include "../Shadow/ShadowMap.h"
#include "../Renderer/Renderer.h"
#include "../Input/InputInterface.h"
#include "../Fade/Fade.h"
#include "../GameObject/GameObject.h"
#include "../ImGui/DebugImGui.h"
#include "../Camera/Camera.h"
#include "../SaveAndLoad/SaveAndLoad.h"
#include "../Model/StaticMesh.h"
#include "../Mouse/MouseCursor.h"
#include "../Object/CommonObject.h"
#include "../Collision/CollisionTag.h"
#include "../Sound/SoundTypes.h"
#include "../Scene/SceneTypes.h"
#include "../../Game/UserInterface/MainGameUI.h"
#include "../../Game/UserInterface/PauseUI.h"
#include "../../Game/Effect/EffectManager.h"
#include "../../Game/Character/Player/Player.h"
#include "../../Game/Object/PlayerStart/PlayerStart.h"
#include "../../Game/Object/HitBlock/HitBlock.h"
#include "../../Game/Object/ItemObject/Heart.h"
#include "../../Game/Character/Enemy/Enemy.h"

void CFirstBossMap::Init(const std::string& LoadSaveFile, const ESoundIndex PlayBGMIndex)
{
	m_Player = CreateAddGameObject<CPlayer>(ESceneObjectType::PLAYER);
	if (!m_Player)
	{
		return;
	}
	m_Player->SetPosition(Vector3(0.0f, 3.1f, -34.0f));
	m_Player->SetDefaultPosition(Vector3(0.0f, 3.1f, -34.0f));
	if (CSaveAndLoad* save = CSaveAndLoad::GetInstance())
	{
		m_Player->SetMyStarCount(save->GetTemporaryStarCount());
		m_Player->SetMyAcornCount(save->GetTemporaryAcornCount());
	}

	m_Camera = CreateAddGameObject<CCamera>(ESceneObjectType::CAMERA);
	if (m_Camera)
	{
		m_Camera->SetTarget(m_Player);
	}

	m_EffectManager = CreateAddGameObject<CEffectManager>(ESceneObjectType::BILLBOARD);
	CShadowMap::SetLightRotation(Vector3(79.385f, -9.231f, 31.385f));
	{
		CCommonObject *common = new CCommonObject();
		common->Init(EStaticMeshType::SKYDOME , XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f));
		common->SetNoCullingCheck(true);
		common->SetDisableShadow(true);
		AddGameObject(ESceneObjectType::NOMESH_OBJECT, common);
		common->SetOriginalNumber(-1);
	}
	{
		CCommonObject *common = new CCommonObject();
		common->Init(EStaticMeshType::FIRST_BOSS_FIELD, XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(3.0f, 3.0f, 3.0f), XMFLOAT3(0.0f, 0.0f, 0.0f));
		common->SetPlayerCollisionTag(ECollisionTagName::ALL_COLISION);
		common->SetObjectCollisionTag(ECollisionTagName::OBJECT_COLISION);
		common->SetNoCullingCheck(true);
		common->SetCameraCollisionTag(ECollisionTagCameraName::BLOCK);
		AddGameObject(ESceneObjectType::MESH_OBJECT, common);
		common->SetOriginalNumber(-1);
	}
	{
		CCommonObject *common = new CCommonObject();
		common->Init(EStaticMeshType::FIRST_BOSS_WALL, XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(3.0f, 3.0f, 3.0f), XMFLOAT3(0.0f, 0.0f, 0.0f));
		common->SetPlayerCollisionTag(ECollisionTagName::ALL_COLISION);
		common->SetNoCullingCheck(true);
		common->SetEnableLight(true);
		common->SetCameraCollisionTag(ECollisionTagCameraName::BLOCK);
		AddGameObject(ESceneObjectType::MESH_OBJECT, common);
		common->SetOriginalNumber(-1);
	}
	{
		CHitBlock* hit = new CHitBlock(Vector3(0.0f, 3.1f, 25.0f), Vector3(0.0f, 10.0f, 0.0f), Vector3(13.0f, 18.0f, 13.0f), Vector3(0, 6.5f, 0), Vector3(6.9f, 38.0f, 6.6f), Vector3(0.0f, 0.0f, 0.0f));;
		AddGameObject(ESceneObjectType::MESH_OBJECT, hit);
		hit->SetOriginalNumber(-1);
	}
	{
		CHitBlock* hit = new CHitBlock(Vector3(22.0f, 3.1f, 0.0f), Vector3(0.0f, 0.0f, 0.0f), Vector3(18.0f, 17.0f, 15.0f), Vector3(0, 6.5f, 0), Vector3(6.9f, 38.0f, 6.6f), Vector3(0.0f, 0.0f, 0.0f));
		AddGameObject(ESceneObjectType::MESH_OBJECT, hit);
		hit->SetOriginalNumber(-1);
	}
	{
		CHitBlock* hit = new CHitBlock(Vector3(-18.0f, 3.1f, -21.0f), Vector3(0.0f, 30.0f, 0.0f), Vector3(12.0f, 19.0f, 15.0f), Vector3(0, 6.5f, 0), Vector3(6.5f, 48.0f, 6.7f), Vector3(0.0f, 0.0f, 0.0f));
		AddGameObject(ESceneObjectType::MESH_OBJECT, hit);
		hit->SetOriginalNumber(-1);
	}
	{
		CEnemy* enemy = nullptr;
		enemy = CEnemy::Create(EEnemyType::GORILLA, Vector3(0.0f, 3.0f, 0.0f));
		AddGameObject(ESceneObjectType::ENEMY, enemy);
	}
	{
		CHeart* heart = new CHeart(Vector3(25.82f, 3.1f, -26.0f), false);
		AddGameObject(ESceneObjectType::NOMESH_OBJECT, heart);
		heart->SetOriginalNumber(-1);
	}
	{
		CHeart* heart = new CHeart(Vector3(-23.52f, 3.1f, 25.0f), false);
		AddGameObject(ESceneObjectType::NOMESH_OBJECT, heart);
		heart->SetOriginalNumber(-1);
	}
	m_MouseCursor = CreateAddGameObject<CMouseCursor>(ESceneObjectType::UI);

	CreateAddGameObject<CMainGameUI>(ESceneObjectType::UI);
	CreateAddGameObject<CPauseUI>(ESceneObjectType::UI);

	m_ImGui = CreateAddGameObject<CImGui>(ESceneObjectType::UI);
	if (m_ImGui)
	{
		m_ImGui->SetSaveFile(ESceneType::BOSS);
	}
	if (CFade* fade = CFade::GetInstance())
	{
		fade->Start(false, 1.4f, 1.4f);
	}
	PlayBGM(ESoundIndex::BOSS_BGM);
}

void CFirstBossMap::UnInit()
{
	CScene::UnInit();
	if (!m_IsRetry)
	{
		CStaticMesh::AllLoadModelDelete();
	}
}

ESceneType CFirstBossMap::Update(float DeltaTime)
{
	CScene::Update(DeltaTime);
	
	CShadowMap::Update();

	if (m_Player)
	{
		if (!m_IsStageClearChangeScene && m_Player->IsSceneChangeGetStageClearStar())
		{
			m_IsStageClearChangeScene = true;
			if (CSaveAndLoad* save = CSaveAndLoad::GetInstance())
			{
				save->SetMyStarAndAcorn(save->GetStartMainGameScene(), m_Player->GetMyStarCount(), m_Player->GetMyAcornCount());
				save->PlayerSave();
			}
			return ESceneType::STAGE_SELECT;
		}
	}

	return ESceneType::BOSS;
}

void CFirstBossMap::Draw()
{
	CScene::ShadowDraw();
	CScene::Draw();
	CRenderer::SetShader();
}
