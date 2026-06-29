#include "SaveAndLoad.h"
#include <fstream>
#include "../Object/CommonObject.h"
#include "../Scene/SceneManager.h"
#include "../Scene/Scene.h"
#include "../Collision/Box.h"
#include "../Collision/Sphere.h"
#include "../Collision/CollisionTag.h"
#include "../Object/NoCliffBox.h"
#include "../Object/CameraHeightLockBox.h"
#include "../Object/TutorialBox.h"
#include "../Mouse/MouseCursor.h"
#include "../Renderer/Renderer.h"
#include "../Model/StaticMeshType.h"
#include "../ImGui/ImGuiObjectIcon.h"
#include "../../Game/Character/Enemy/Enemy.h"
#include "../../Game/Character/Enemy/Mob/MobEnemy.h"
#include "../../Game/Object/Door/StarDoor.h"
#include "../../Game/Object/PlayerStart/PlayerStart.h"
#include "../../Game/Object/ItemObject/Acorn.h"
#include "../../Game/Object/ItemObject/Heart.h"
#include "../../Game/Object/ItemObject/Star.h"

#ifdef _DEBUG
#ifndef DBG_NEW
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#define new DBG_NEW
#endif
#endif

void CSaveAndLoad::Save(const std::string& FileName)
{
	CSceneManager* sceneManager = CSceneManager::GetInstance();
	if (!sceneManager)
	{
		return;
	}
	std::weak_ptr<CScene> scene = sceneManager->GetScene();
	if (scene.expired())
	{
		return;
	}

	FILE *pFile;
	errno_t error;
	std::string name;
	name = "asset/save/";
	name = name + FileName;
	error = fopen_s(&pFile, name.c_str(), "wb");
	if (error != 0)
	{
		return;
	}
	
	std::vector<CAcorn*> acorns;
	scene.lock()->GetGameObjects<CAcorn>(ESceneObjectType::BILLBOARD, acorns);
	for (CAcorn* acorn : acorns)
	{
		if (!acorn)
		{
			continue;
		}
		m_SaveData.ObjectType = EEditorSelectObjectType::ACORN;
		m_SaveData.Position = acorn->GetPosition();
		fprintf(pFile, "%d,", m_SaveData.ObjectType);
		fprintf(pFile, "%f,%f,%f\n", m_SaveData.Position.x, m_SaveData.Position.y, m_SaveData.Position.z);
	}
	std::vector<CHeart*> hearts;
	scene.lock()->GetGameObjects<CHeart>(ESceneObjectType::NOMESH_OBJECT, hearts);
	for (CHeart* heart : hearts)
	{
		if (!heart)
		{
			continue;
		}
		m_SaveData.ObjectType = EEditorSelectObjectType::HEART;
		m_SaveData.Position = heart->GetPosition();
		m_SaveData.Material = *heart->GetMaterial();
		m_SaveData.EnableLight = *heart->IsEnableLight();
		fprintf(pFile, "%d,", m_SaveData.ObjectType);
		fprintf(pFile, "%f,%f,%f,", m_SaveData.Position.x, m_SaveData.Position.y, m_SaveData.Position.z);
		fprintf(pFile, "%f,%f,%f,%f,", m_SaveData.Material.Diffuse.r, m_SaveData.Material.Diffuse.g, m_SaveData.Material.Diffuse.b, m_SaveData.Material.Diffuse.a);
		fprintf(pFile, "%f,%f,%f,%f,", m_SaveData.Material.Ambient.r, m_SaveData.Material.Ambient.g, m_SaveData.Material.Ambient.b, m_SaveData.Material.Ambient.a);
		fprintf(pFile, "%f,%f,%f,%f,%d\n", m_SaveData.Material.Specular.r, m_SaveData.Material.Specular.g, m_SaveData.Material.Specular.b, m_SaveData.Material.Specular.a, m_SaveData.EnableLight);
	}
	std::vector<CStar*> stars;
	scene.lock()->GetGameObjects<CStar>(ESceneObjectType::NOMESH_OBJECT, stars);
	for (CStar* star : stars)
	{
		if (!star)
		{
			continue;
		}
		m_SaveData.ObjectType = EEditorSelectObjectType::STAR;
		m_SaveData.Position = star->GetPosition();
		m_SaveData.Material = *star->GetMaterial();
		m_SaveData.EnableLight = *star->IsEnableLight();
		fprintf(pFile, "%d,", m_SaveData.ObjectType);
		fprintf(pFile, "%f,%f,%f,", m_SaveData.Position.x, m_SaveData.Position.y, m_SaveData.Position.z);
		fprintf(pFile, "%f,%f,%f,%f,", m_SaveData.Material.Diffuse.r, m_SaveData.Material.Diffuse.g, m_SaveData.Material.Diffuse.b, m_SaveData.Material.Diffuse.a);
		fprintf(pFile, "%f,%f,%f,%f,", m_SaveData.Material.Ambient.r, m_SaveData.Material.Ambient.g, m_SaveData.Material.Ambient.b, m_SaveData.Material.Ambient.a);
		fprintf(pFile, "%f,%f,%f,%f,%d\n", m_SaveData.Material.Specular.r, m_SaveData.Material.Specular.g, m_SaveData.Material.Specular.b, m_SaveData.Material.Specular.a, m_SaveData.EnableLight);
	}
	std::vector<CNoCliffBox*> nocliffs;
	scene.lock()->GetGameObjects<CNoCliffBox>(ESceneObjectType::NOMESH_OBJECT, nocliffs);
	for (CNoCliffBox* nocliff : nocliffs)
	{
		if (!nocliff)
		{
			continue;
		}
		m_SaveData.ObjectType = EEditorSelectObjectType::NO_CLIFFBOX;
		m_SaveData.Position = nocliff->GetPosition();
		SatBoxData(nocliff->GetCollisionBox());
		fprintf(pFile, "%d,", m_SaveData.ObjectType);
		fprintf(pFile, "%f,%f,%f,", m_SaveData.Position.x, m_SaveData.Position.y, m_SaveData.Position.z);
		fprintf(pFile, "%f,%f,%f,", m_SaveData.BoxMax.x, m_SaveData.BoxMax.y, m_SaveData.BoxMax.z);
		fprintf(pFile, "%f,%f,%f,", m_SaveData.BoxMin.x, m_SaveData.BoxMin.y, m_SaveData.BoxMin.z);
		fprintf(pFile, "%f,%f,%f\n", m_SaveData.BoxPosition.x, m_SaveData.BoxPosition.y, m_SaveData.BoxPosition.z);
	}
	std::vector<CCameraHeightLockBox*> cameraboxs;
	scene.lock()->GetGameObjects<CCameraHeightLockBox>(ESceneObjectType::NOMESH_OBJECT, cameraboxs);
	for (CCameraHeightLockBox* camerabox : cameraboxs)
	{
		if (!camerabox)
		{
			continue;
		}
		m_SaveData.ObjectType = EEditorSelectObjectType::CAMERA_HEIGHTLOCK_BOX;
		m_SaveData.Position = camerabox->GetPosition();
		SatBoxData(camerabox->GetCollisionBox());
		m_SaveData.HeightLockPosition = *camerabox->GetHeightLockPosition();
		fprintf(pFile, "%d,", m_SaveData.ObjectType);
		fprintf(pFile, "%f,%f,%f,", m_SaveData.Position.x, m_SaveData.Position.y, m_SaveData.Position.z);
		fprintf(pFile, "%f,%f,%f,", m_SaveData.BoxMax.x, m_SaveData.BoxMax.y, m_SaveData.BoxMax.z);
		fprintf(pFile, "%f,%f,%f,", m_SaveData.BoxMin.x, m_SaveData.BoxMin.y, m_SaveData.BoxMin.z);
		fprintf(pFile, "%f,%f,%f,%f\n", m_SaveData.BoxPosition.x, m_SaveData.BoxPosition.y, m_SaveData.BoxPosition.z, m_SaveData.HeightLockPosition);
	}
	std::vector<CTutorialBox*> tutorials;
	scene.lock()->GetGameObjects<CTutorialBox>(ESceneObjectType::NOMESH_OBJECT, tutorials);
	for (CTutorialBox* tutorial : tutorials)
	{
		if (!tutorial)
		{
			continue;
		}
		m_SaveData.ObjectType = EEditorSelectObjectType::TUTORIAL_BOX;
		m_SaveData.Position = tutorial->GetPosition();
		SatBoxData(tutorial->GetCollisionBox());
		m_SaveData.TutorialNumber = *tutorial->GetTutorialNumber();
		fprintf(pFile, "%d,", m_SaveData.ObjectType);
		fprintf(pFile, "%f,%f,%f,", m_SaveData.Position.x, m_SaveData.Position.y, m_SaveData.Position.z);
		fprintf(pFile, "%f,%f,%f,", m_SaveData.BoxMax.x, m_SaveData.BoxMax.y, m_SaveData.BoxMax.z);
		fprintf(pFile, "%f,%f,%f,", m_SaveData.BoxMin.x, m_SaveData.BoxMin.y, m_SaveData.BoxMin.z);
		fprintf(pFile, "%f,%f,%f,%d\n", m_SaveData.BoxPosition.x, m_SaveData.BoxPosition.y, m_SaveData.BoxPosition.z, m_SaveData.TutorialNumber);
	}
	std::vector<CPlayerStart*> starts;
	scene.lock()->GetGameObjects<CPlayerStart>(ESceneObjectType::NOMESH_OBJECT, starts);
	for (CPlayerStart* start : starts)
	{
		if (!start)
		{
			continue;
		}
		m_SaveData.ObjectType = EEditorSelectObjectType::PLAYER_START;
		m_SaveData.Position = start->GetPosition();
		m_SaveData.Rotation = start->GetRotation();
		fprintf(pFile, "%d,", m_SaveData.ObjectType);
		fprintf(pFile, "%f,%f,%f,", m_SaveData.Position.x, m_SaveData.Position.y, m_SaveData.Position.z);
		fprintf(pFile, "%f,%f,%f\n", m_SaveData.Rotation.x, m_SaveData.Rotation.y, m_SaveData.Rotation.z);
	}
	std::vector<CCommonObject*> commons;
	scene.lock()->GetGameObjects<CCommonObject>(ESceneObjectType::MESH_OBJECT, commons);
	for (CCommonObject* common : commons)
	{
		if (!common)
		{
			continue;
		}
		if (common->GetObjectReference())
		{
			continue;
		}
		m_SaveData.ObjectType = EEditorSelectObjectType::COMMON;
		m_SaveData.Position = common->GetPosition();
		m_SaveData.Material = *common->GetMaterial();
		if (CSphere* sphere = common->GetCullingSphere())
		{
			SatSphereData(sphere);
		}
		else
		{
			m_SaveData.SphereRadius = 0.0f;
			m_SaveData.SpherePosition = Vector3::Zero();
		}
		m_SaveData.StaticMeshType = common->GetMeshType();
		m_SaveData.Scale = common->GetScale();
		m_SaveData.Rotation = common->GetRotation();
		m_SaveData.PlayerCollisionTag = common->GetPlayerCollisionTag();
		m_SaveData.ObjectCollisionTag = common->GetObjectCollisionTag();
		m_SaveData.CameraCollisionTag = common->GetCameraCollisionTag();
		m_SaveData.StepsCollisionTag = common->GetStepsCollisionTag();
		m_SaveData.DisableShadow = *common->IsDisableShadow();
		m_SaveData.NormalPower = common->GetNormalMapPower()->NormalPower.x;
		m_SaveData.NormalSpecular = common->GetNormalMapPower()->SpecularPower.x;
		m_SaveData.NoCullingCheck = *common->IsNoCullingCheck();
		m_SaveData.EditorOnlyDraw = *common->IsEditorOnlyDraw();
		m_SaveData.EnableNormalMap = *common->IsEnableNormalMap();
		m_SaveData.EnableLight = *common->IsEnableLight();
		m_SaveData.DisableDepth = *common->IsDisableDepth();
		m_SaveData.DisableSelfShadow = *common->IsDisableSelfShadow();
		fprintf(pFile, "%d,", m_SaveData.ObjectType);
		fprintf(pFile, "%f,%f,%f,", m_SaveData.Position.x, m_SaveData.Position.y, m_SaveData.Position.z);
		fprintf(pFile, "%f,%f,%f,%f,", m_SaveData.Material.Diffuse.r, m_SaveData.Material.Diffuse.g, m_SaveData.Material.Diffuse.b, m_SaveData.Material.Diffuse.a);
		fprintf(pFile, "%f,%f,%f,%f,", m_SaveData.Material.Ambient.r, m_SaveData.Material.Ambient.g, m_SaveData.Material.Ambient.b, m_SaveData.Material.Ambient.a);
		fprintf(pFile, "%f,%f,%f,%f,", m_SaveData.Material.Specular.r, m_SaveData.Material.Specular.g, m_SaveData.Material.Specular.b, m_SaveData.Material.Specular.a);
		fprintf(pFile, "%d,", m_SaveData.StaticMeshType);
		fprintf(pFile, "%f,%f,%f,", m_SaveData.Scale.x, m_SaveData.Scale.y, m_SaveData.Scale.z);
		fprintf(pFile, "%f,%f,%f,", m_SaveData.Rotation.x, m_SaveData.Rotation.y, m_SaveData.Rotation.z);
		fprintf(pFile, "%d,%d,%d,", m_SaveData.PlayerCollisionTag, m_SaveData.ObjectCollisionTag, m_SaveData.DisableShadow);
		fprintf(pFile, "%f,%f,", m_SaveData.NormalPower, m_SaveData.NormalSpecular);
		fprintf(pFile, "%f,%f,%f,%f,%d,%d,%d,%d,%d,%d,%d,%d\n", m_SaveData.SphereRadius, m_SaveData.SpherePosition.x, m_SaveData.SpherePosition.y, m_SaveData.SpherePosition.z, m_SaveData.NoCullingCheck, m_SaveData.EditorOnlyDraw, m_SaveData.CameraCollisionTag, m_SaveData.EnableNormalMap, m_SaveData.EnableLight, m_SaveData.DisableDepth, m_SaveData.StepsCollisionTag, m_SaveData.DisableSelfShadow);
	}
	std::vector<CStarDoor*> stardoors;
	scene.lock()->GetGameObjects<CStarDoor>(ESceneObjectType::MESH_OBJECT, stardoors);
	for (CStarDoor* stardoor : stardoors)
	{
		if (!stardoor)
		{
			continue;
		}
		m_SaveData.ObjectType = EEditorSelectObjectType::STAR_DOOR;
		m_SaveData.Position = stardoor->GetPosition();
		m_SaveData.Rotation = stardoor->GetRotation();
		m_SaveData.OpenStar = *stardoor->GetOpenStar();
		fprintf(pFile, "%d,", m_SaveData.ObjectType);
		fprintf(pFile, "%f,%f,%f,", m_SaveData.Position.x, m_SaveData.Position.y, m_SaveData.Position.z);
		fprintf(pFile, "%f,%f,%f,%d\n", m_SaveData.Rotation.x, m_SaveData.Rotation.y, m_SaveData.Rotation.z, m_SaveData.OpenStar);
	}
	std::vector<CMobEnemy*> enemys;
	scene.lock()->GetGameMobEnemyObjects(ESceneObjectType::ENEMY, enemys);
	for (CMobEnemy* enemy : enemys)
	{
		if (!enemy)
		{
			continue;
		}
		switch (enemy->GetEnemyType())
		{
		case EEnemyType::SLIME:
			m_SaveData.ObjectType = EEditorSelectObjectType::SLIME;
			m_SaveData.Position = enemy->GetFirstPosition();
			m_SaveData.Rotation = enemy->GetFirstRotation();
			m_SaveData.Scale = enemy->GetFirstScale();
			SatSphereData(enemy->GetRangeSphere());
			SatBodySphereData(enemy->GetSphere());
			m_SaveData.TargetPoint = *enemy->GetTargetPoint();
			m_SaveData.TargetPointMax = (int)m_SaveData.TargetPoint.size();
			m_SaveData.IsRespawn = *enemy->IsRespawn();
			m_SaveData.ItemDrop = *enemy->GetItemDropData();
			SatBoxData(enemy->GetRangeBox());
			m_SaveData.IsBodyCollisionCheck = *enemy->IsBodySphereCollisionCheck();
			m_SaveData.IsDownLineCollisionCheck = *enemy->IsDownLineCollisionCheck();
			if (m_SaveData.IsRespawn)
			{
				m_SaveData.RespawnTime = *enemy->GetRespawnTime();
			}
			else
			{
				m_SaveData.RespawnTime = 0;
			}
			fprintf(pFile, "%d,", m_SaveData.ObjectType);
			//2番目にターゲットポイントの数にすること
			fprintf(pFile, "%d,", m_SaveData.TargetPointMax);
			fprintf(pFile, "%f,%f,%f,", m_SaveData.Position.x, m_SaveData.Position.y, m_SaveData.Position.z);
			fprintf(pFile, "%f,%f,%f,", m_SaveData.Scale.x, m_SaveData.Scale.y, m_SaveData.Scale.z);
			fprintf(pFile, "%f,%f,%f,", m_SaveData.Rotation.x, m_SaveData.Rotation.y, m_SaveData.Rotation.z);
			fprintf(pFile, "%f,%f,%f,%f,", m_SaveData.SphereRadius, m_SaveData.SpherePosition.x, m_SaveData.SpherePosition.y, m_SaveData.SpherePosition.z);
			fprintf(pFile, "%f,%f,%f,%f,", m_SaveData.BodySphereRadius, m_SaveData.BodySpherePosition.x, m_SaveData.BodySpherePosition.y, m_SaveData.BodySpherePosition.z);
			fprintf(pFile, "%d,%d,", m_SaveData.IsRespawn, m_SaveData.RespawnTime);
			fprintf(pFile, "%d,%d,", m_SaveData.ItemDrop.ItemDropType, m_SaveData.ItemDrop.Percent);
			fprintf(pFile, "%f,%f,%f,", m_SaveData.BoxMax.x, m_SaveData.BoxMax.y, m_SaveData.BoxMax.z);
			fprintf(pFile, "%f,%f,%f,", m_SaveData.BoxMin.x, m_SaveData.BoxMin.y, m_SaveData.BoxMin.z);
			fprintf(pFile, "%f,%f,%f,", m_SaveData.BoxPosition.x, m_SaveData.BoxPosition.y, m_SaveData.BoxPosition.z);
			fprintf(pFile, "%d,%d", m_SaveData.IsBodyCollisionCheck, m_SaveData.IsDownLineCollisionCheck);
			//ターゲットポイントは最後にセーブすること
			for (int i = 0; i < m_SaveData.TargetPointMax; i++)
			{
				//認識文字Tを代入
				fprintf(pFile, ",T,");
				fprintf(pFile, "%f,%f,%f", m_SaveData.TargetPoint[i].x, m_SaveData.TargetPoint[i].y, m_SaveData.TargetPoint[i].z);
			}
			fprintf(pFile, "\n");
			break;
		case EEnemyType::MONKEY:
			m_SaveData.ObjectType = EEditorSelectObjectType::MONKEY;
			m_SaveData.Position = enemy->GetFirstPosition();
			m_SaveData.Rotation = enemy->GetFirstRotation();
			m_SaveData.Scale = enemy->GetFirstScale();
			SatSphereData(enemy->GetRangeSphere());
			m_SaveData.IsRespawn = *enemy->IsRespawn();
			m_SaveData.ItemDrop = *enemy->GetItemDropData();
			if (m_SaveData.IsRespawn)
			{
				m_SaveData.RespawnTime = *enemy->GetRespawnTime();
			}
			else
			{
				m_SaveData.RespawnTime = 0;
			}
			fprintf(pFile, "%d,", m_SaveData.ObjectType);
			//2番目にターゲットポイントの数にすること
			fprintf(pFile, "%f,%f,%f,", m_SaveData.Position.x, m_SaveData.Position.y, m_SaveData.Position.z);
			fprintf(pFile, "%f,%f,%f,", m_SaveData.Scale.x, m_SaveData.Scale.y, m_SaveData.Scale.z);
			fprintf(pFile, "%f,%f,%f,", m_SaveData.Rotation.x, m_SaveData.Rotation.y, m_SaveData.Rotation.z);
			fprintf(pFile, "%f,%f,%f,%f,", m_SaveData.SphereRadius, m_SaveData.SpherePosition.x, m_SaveData.SpherePosition.y, m_SaveData.SpherePosition.z);
			fprintf(pFile, "%d,%d,", m_SaveData.IsRespawn, m_SaveData.RespawnTime);
			fprintf(pFile, "%d,%d", m_SaveData.ItemDrop.ItemDropType, m_SaveData.ItemDrop.Percent);
			fprintf(pFile, "\n");
			break;
		case EEnemyType::THORN_MAN:
			m_SaveData.ObjectType = EEditorSelectObjectType::THORNMAN;
			m_SaveData.Position = enemy->GetFirstPosition();
			m_SaveData.Rotation = enemy->GetFirstRotation();
			m_SaveData.Scale = enemy->GetFirstScale();
			SatBodySphereData(enemy->GetSphere());
			m_SaveData.TargetPoint = *enemy->GetTargetPoint();
			m_SaveData.TargetPointMax = (int)m_SaveData.TargetPoint.size();
			m_SaveData.IsRespawn = *enemy->IsRespawn();
			m_SaveData.ItemDrop = *enemy->GetItemDropData();
			if (m_SaveData.IsRespawn)
			{
				m_SaveData.RespawnTime = *enemy->GetRespawnTime();
			}
			else
			{
				m_SaveData.RespawnTime = 0;
			}
			fprintf(pFile, "%d,", m_SaveData.ObjectType);
			//2番目にターゲットポイントの数にすること
			fprintf(pFile, "%d,", m_SaveData.TargetPointMax);
			fprintf(pFile, "%f,%f,%f,", m_SaveData.Position.x, m_SaveData.Position.y, m_SaveData.Position.z);
			fprintf(pFile, "%f,%f,%f,", m_SaveData.Scale.x, m_SaveData.Scale.y, m_SaveData.Scale.z);
			fprintf(pFile, "%f,%f,%f,", m_SaveData.Rotation.x, m_SaveData.Rotation.y, m_SaveData.Rotation.z);
			fprintf(pFile, "%f,%f,%f,%f,", m_SaveData.BodySphereRadius, m_SaveData.BodySpherePosition.x, m_SaveData.BodySpherePosition.y, m_SaveData.BodySpherePosition.z);
			fprintf(pFile, "%d,%d,", m_SaveData.IsRespawn, m_SaveData.RespawnTime);
			fprintf(pFile, "%d,%d", m_SaveData.ItemDrop.ItemDropType, m_SaveData.ItemDrop.Percent);
			//ターゲットポイントは最後にセーブすること
			for (int i = 0; i < m_SaveData.TargetPointMax; i++)
			{
				//認識文字Tを代入
				fprintf(pFile, ",T,");
				fprintf(pFile, "%f,%f,%f", m_SaveData.TargetPoint[i].x, m_SaveData.TargetPoint[i].y, m_SaveData.TargetPoint[i].z);
			}
			fprintf(pFile, "\n");
			break;
		}
	}
	//フォグセーブ
	//float3つのみセーブなのでPositionに格納する
	m_SaveData.ObjectType = EEditorSelectObjectType::FOG;
	const FOG_CONSTANT& fog = CRenderer::GetFogConstant();
	m_SaveData.Position = Vector3(fog.FogParam.x, fog.FogParam.y, fog.FogParam.z);
	fprintf(pFile, "%d,", m_SaveData.ObjectType);
	fprintf(pFile, "%f,%f,%f,", m_SaveData.Position.x, m_SaveData.Position.y, m_SaveData.Position.z);
	fclose(pFile);
}

void CSaveAndLoad::Load(const std::string& FileName)
{
	CSceneManager* sceneManager = CSceneManager::GetInstance();
	if (!sceneManager)
	{
		return;
	}
	CMouseCursor* mouse = sceneManager->GetMouseCursor();
	if (mouse)
	{
		mouse->SetChoiceObjectNumber(EEditorSelectObjectType::NONE);
	}
	std::weak_ptr<CScene> scene = sceneManager->GetScene();
	if (scene.expired())
	{
		return;
	}
	//マップにある読み込みオブジェクトを全てリリースする
	std::vector<CAcorn*> acorns;
	scene.lock()->GetGameObjects<CAcorn>(ESceneObjectType::BILLBOARD, acorns);
	for (CAcorn* acorn : acorns)
	{
		if (!acorn)
		{
			continue;
		}
		acorn->SetDestroyEdit();
	}
	std::vector<CHeart*> hearts;
	scene.lock()->GetGameObjects<CHeart>(ESceneObjectType::NOMESH_OBJECT, hearts);
	for (CHeart* heart : hearts)
	{
		if (!heart)
		{
			continue;
		}
		heart->SetDestroyEdit();
	}
	std::vector<CStar*> stars;
	scene.lock()->GetGameObjects<CStar>(ESceneObjectType::NOMESH_OBJECT, stars);
	for (CStar* star : stars)
	{
		if (!star)
		{
			continue;
		}
		star->SetDestroyEdit();
	}
	std::vector<CStarDoor*> stardoors;
	scene.lock()->GetGameObjects<CStarDoor>(ESceneObjectType::MESH_OBJECT, stardoors);
	for (CStarDoor* stardoor : stardoors)
	{
		if (!stardoor)
		{
			continue;
		}
		stardoor->SetDestroyEdit();
	}
	std::vector<CNoCliffBox*> nocliffs;
	scene.lock()->GetGameObjects<CNoCliffBox>(ESceneObjectType::NOMESH_OBJECT, nocliffs);
	for (CNoCliffBox* nocliff : nocliffs)
	{
		if (!nocliff)
		{
			continue;
		}
		nocliff->SetDestroyEdit();
	}
	std::vector<CCameraHeightLockBox*> cameraboxs;
	scene.lock()->GetGameObjects<CCameraHeightLockBox>(ESceneObjectType::NOMESH_OBJECT, cameraboxs);
	for (CCameraHeightLockBox* camerabox : cameraboxs)
	{
		if (!camerabox)
		{
			continue;
		}
		camerabox->SetDestroyEdit();
	}
	std::vector<CTutorialBox*> tutorials;
	scene.lock()->GetGameObjects<CTutorialBox>(ESceneObjectType::NOMESH_OBJECT, tutorials);
	for (CTutorialBox* tutorial : tutorials)
	{
		if (!tutorial)
		{
			continue;
		}
		tutorial->SetDestroyEdit();
	}
	std::vector<CPlayerStart*> starts;
	scene.lock()->GetGameObjects<CPlayerStart>(ESceneObjectType::NOMESH_OBJECT, starts);
	for (CPlayerStart* start : starts)
	{
		if (!start)
		{
			continue;
		}
		start->SetDestroyEdit();
	}
	std::vector<CCommonObject*> commons;
	scene.lock()->GetGameObjects<CCommonObject>(ESceneObjectType::MESH_OBJECT, commons);
	for (CCommonObject* common : commons)
	{
		if (!common)
		{
			continue;
		}
		common->SetDestroyEdit();
	}
	std::vector<CEnemy*> enemys;
	scene.lock()->GetGameEnemyObjects(ESceneObjectType::ENEMY, enemys);
	for (CEnemy* enemy : enemys)
	{
		if (!enemy)
		{
			continue;
		}
		enemy->SetDestroyEdit();
	}
	CommonLoad(FileName);
}

void CSaveAndLoad::AddLoad(const std::string& FileName)
{
	CommonLoad(FileName);
}

void CSaveAndLoad::SetMyStarAndAcorn(const ESceneType Type, const int Star, const int Acorn)
{
	const PlayerSaveData data = PlayerSaveData(Star, Acorn);
	//すでに要素が存在すれば要素が多い方を採用。なければ要素作成
	if (m_PlayerSaveData.contains(Type))
	{
		PlayerSaveData& currentData = m_PlayerSaveData[Type];
		currentData.StageStarCount = (std::max)(currentData.StageStarCount, data.StageStarCount);
		currentData.StageAcornCount = (std::max)(currentData.StageAcornCount, data.StageAcornCount);
		return;
	}
	m_PlayerSaveData.emplace(Type, data);
}

int CSaveAndLoad::GetStarCount(const ESceneType Type)
{
	if (m_PlayerSaveData.contains(Type))
	{
		return m_PlayerSaveData[Type].StageStarCount;
	}
	return 0;
}

int CSaveAndLoad::GetAcornCount(const ESceneType Type)
{
	if (m_PlayerSaveData.contains(Type))
	{
		return m_PlayerSaveData[Type].StageAcornCount;
	}
	return 0;
}

void CSaveAndLoad::CommonLoad(const std::string& FileName)
{
	std::string name;
	name = "asset/save/";
	name = name + FileName;
	std::string data;
	
	std::ifstream ifs(name.c_str());
	if (!ifs)
	{
		std::cout << "ファイルが開けませんでした。" << std::endl;
		std::cin.get();
	}

	std::string lineData;
	while (!ifs.eof())
	{//1行ずつ読み込む
		std::getline(ifs, lineData);
		if (lineData.size() == 0)
		{
			break;
		}

		ObjectLoad(lineData);
	}
	ifs.close();
}

void CSaveAndLoad::ObjectLoad(std::string& LineData)
{
	CSceneManager* sceneManager = CSceneManager::GetInstance();
	if (!sceneManager)
	{
		return;
	}
	std::weak_ptr<CScene> scene = sceneManager->GetScene();
	if (scene.expired())
	{
		return;
	}
	const char* lineDataChar = LineData.c_str();
	if (!lineDataChar)
	{
		return;
	}
	CMouseCursor* mouse = sceneManager->GetMouseCursor();
	sscanf_s(lineDataChar, "%d", &m_SaveData.ObjectType);
	switch (m_SaveData.ObjectType)
	{
		case EEditorSelectObjectType::ACORN:
		{
			sscanf_s(lineDataChar, "%d,%f,%f,%f", &m_SaveData.ObjectType, &m_SaveData.Position.x, &m_SaveData.Position.y, &m_SaveData.Position.z);
			CAcorn* acorn = new CAcorn(Vector3(m_SaveData.Position));
			if (!acorn)
			{
				return;;
			}
			scene.lock()->AddGameObject<CAcorn>(ESceneObjectType::BILLBOARD, acorn);
			if (mouse)
			{
				mouse->PushMapObject(acorn, "Acorn", EStaticMeshType::SPHERE, EEditorSelectObjectType::ACORN);
			}
			break;
		}
		case EEditorSelectObjectType::HEART:
		{
			sscanf_s(lineDataChar, "%d,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%d", &m_SaveData.ObjectType, &m_SaveData.Position.x, &m_SaveData.Position.y, &m_SaveData.Position.z,
				&m_SaveData.Material.Diffuse.r, &m_SaveData.Material.Diffuse.g, &m_SaveData.Material.Diffuse.b, &m_SaveData.Material.Diffuse.a,
				&m_SaveData.Material.Ambient.r, &m_SaveData.Material.Ambient.g, &m_SaveData.Material.Ambient.b, &m_SaveData.Material.Ambient.a,
				&m_SaveData.Material.Specular.r, &m_SaveData.Material.Specular.g, &m_SaveData.Material.Specular.b, &m_SaveData.Material.Specular.a,&m_SaveData.EnableLight);
			CHeart* heart = new CHeart(Vector3(m_SaveData.Position), false);
			if (!heart)
			{
				return;;
			}
			scene.lock()->AddGameObject<CHeart>(ESceneObjectType::NOMESH_OBJECT, heart);
			if (mouse)
			{
				mouse->PushMapObject(heart, "Heart", EStaticMeshType::HEART, EEditorSelectObjectType::HEART);
			}
			break;
		}
		case EEditorSelectObjectType::STAR:
		{
			sscanf_s(lineDataChar, "%d,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%d", &m_SaveData.ObjectType, &m_SaveData.Position.x, &m_SaveData.Position.y, &m_SaveData.Position.z,
				&m_SaveData.Material.Diffuse.r, &m_SaveData.Material.Diffuse.g, &m_SaveData.Material.Diffuse.b, &m_SaveData.Material.Diffuse.a,
				&m_SaveData.Material.Ambient.r, &m_SaveData.Material.Ambient.g, &m_SaveData.Material.Ambient.b, &m_SaveData.Material.Ambient.a,
				&m_SaveData.Material.Specular.r, &m_SaveData.Material.Specular.g, &m_SaveData.Material.Specular.b, &m_SaveData.Material.Specular.a, &m_SaveData.EnableLight);
			CStar* star = new CStar(Vector3(m_SaveData.Position));
			if (!star)
			{
				return;;
			}
			scene.lock()->AddGameObject<CStar>(ESceneObjectType::NOMESH_OBJECT, star);
			if (mouse)
			{
				mouse->PushMapObject(star, "Star", EStaticMeshType::STAR, EEditorSelectObjectType::STAR);
			}
			break;
		}
		case EEditorSelectObjectType::STAR_DOOR:
		{
			sscanf_s(lineDataChar, "%d,%f,%f,%f,%f,%f,%f,%d", &m_SaveData.ObjectType, &m_SaveData.Position.x, &m_SaveData.Position.y, &m_SaveData.Position.z,
				&m_SaveData.Rotation.x, &m_SaveData.Rotation.y, &m_SaveData.Rotation.z,
				&m_SaveData.OpenStar);
				
			CStarDoor *stardoor = new CStarDoor(Vector3(m_SaveData.Position), Vector3(m_SaveData.Rotation), m_SaveData.OpenStar);
			if (!stardoor)
			{
				return;;
			}
			stardoor->SetStarTexture();
			scene.lock()->AddGameObject<CStarDoor>(ESceneObjectType::MESH_OBJECT, stardoor);
			if (mouse)
			{
				mouse->PushMapObject(stardoor, "StarDoor", EStaticMeshType::STAR_DOOR, EEditorSelectObjectType::STAR_DOOR);
			}
			break;
		}
		case EEditorSelectObjectType::COMMON:
		{
			sscanf_s(lineDataChar, "%d,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%d,%f,%f,%f,%f,%f,%f,%d,%d,%d,%f,%f,%f,%f,%f,%f,%d,%d,%d,%d,%d,%d,%d,%d", &m_SaveData.ObjectType, &m_SaveData.Position.x, &m_SaveData.Position.y, &m_SaveData.Position.z,
				&m_SaveData.Material.Diffuse.r, &m_SaveData.Material.Diffuse.g, &m_SaveData.Material.Diffuse.b, &m_SaveData.Material.Diffuse.a,
				&m_SaveData.Material.Ambient.r, &m_SaveData.Material.Ambient.g, &m_SaveData.Material.Ambient.b, &m_SaveData.Material.Ambient.a,
				&m_SaveData.Material.Specular.r, &m_SaveData.Material.Specular.g, &m_SaveData.Material.Specular.b, &m_SaveData.Material.Specular.a,
				&m_SaveData.StaticMeshType,
				&m_SaveData.Scale.x, &m_SaveData.Scale.y, &m_SaveData.Scale.z,
				&m_SaveData.Rotation.x, &m_SaveData.Rotation.y, &m_SaveData.Rotation.z,
				&m_SaveData.PlayerCollisionTag, &m_SaveData.ObjectCollisionTag, &m_SaveData.DisableShadow,
				&m_SaveData.NormalPower, &m_SaveData.NormalSpecular,
				&m_SaveData.SphereRadius, &m_SaveData.SpherePosition.x, &m_SaveData.SpherePosition.y, &m_SaveData.SpherePosition.z, &m_SaveData.NoCullingCheck, &m_SaveData.EditorOnlyDraw, &m_SaveData.CameraCollisionTag
				, &m_SaveData.EnableNormalMap, &m_SaveData.EnableLight, &m_SaveData.DisableDepth,&m_SaveData.StepsCollisionTag,&m_SaveData.DisableSelfShadow);
			CCommonObject* common = new CCommonObject();
			if (!common)
			{
				return;
			}
			common->Init(m_SaveData.StaticMeshType, m_SaveData.Position, Vector3(m_SaveData.Scale), Vector3(m_SaveData.Rotation));
			common->SetPlayerCollisionTag(m_SaveData.PlayerCollisionTag);
			common->SetObjectCollisionTag(m_SaveData.ObjectCollisionTag);
			common->SetCameraCollisionTag(m_SaveData.CameraCollisionTag);
			common->SetStepsCollisionTag(m_SaveData.StepsCollisionTag);
			common->CreateCullingSphere(m_SaveData.SphereRadius, m_SaveData.SpherePosition);
			common->SetDisableShadow(m_SaveData.DisableShadow != 0);
			common->SetNoCullingCheck(m_SaveData.NoCullingCheck != 0);
			common->SetEditorOnlyDraw(m_SaveData.EditorOnlyDraw != 0);
			common->SetEnableNormalMap(m_SaveData.EnableNormalMap != 0);
			common->SetEnableLight(m_SaveData.EnableLight != 0);
			common->SetDisableDepth(m_SaveData.DisableDepth != 0);
			common->SetDisableSelfShadow(m_SaveData.DisableSelfShadow != 0);
			ShaderNormalMapData cons;
			cons.NormalPower.x = m_SaveData.NormalPower;
			cons.SpecularPower.x = m_SaveData.NormalSpecular;
			common->SetNormalMapPower(cons);
			if (MATERIAL* material = common->GetMaterial())
			{
				m_SaveData.Material.Alpha.r = 1.0f;
				*material = m_SaveData.Material;
			}
			scene.lock()->AddGameObject<CCommonObject>(ESceneObjectType::MESH_OBJECT, common);
			if (mouse)
			{
				mouse->PushMapObject(common, CStaticMesh::GetModelFileName(m_SaveData.StaticMeshType), m_SaveData.StaticMeshType, EEditorSelectObjectType::COMMON);
			}
			break;
		}
		case EEditorSelectObjectType::NO_CLIFFBOX:
		{
			sscanf_s(lineDataChar, "%d,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f", &m_SaveData.ObjectType, &m_SaveData.Position.x, &m_SaveData.Position.y, &m_SaveData.Position.z,
				&m_SaveData.BoxMax.x, &m_SaveData.BoxMax.y, &m_SaveData.BoxMax.z,
				&m_SaveData.BoxMin.x, &m_SaveData.BoxMin.y, &m_SaveData.BoxMin.z,
				&m_SaveData.BoxPosition.x, &m_SaveData.BoxPosition.y, &m_SaveData.BoxPosition.z);
			CNoCliffBox* cliffbox = new CNoCliffBox(Vector3(m_SaveData.Position), Vector3(m_SaveData.BoxMax), Vector3(m_SaveData.BoxMin));
			if (!cliffbox)
			{
				return;;
			}
			cliffbox->GetCollisionBox()->AddBoxPosition(m_SaveData.BoxPosition);
			scene.lock()->AddGameObject<CNoCliffBox>(ESceneObjectType::NOMESH_OBJECT, cliffbox);
			if (mouse)
			{
				mouse->PushMapObject(cliffbox, "NoCliffBox", EStaticMeshType::BOX, EEditorSelectObjectType::NO_CLIFFBOX);
			}
			break;
		}
		case EEditorSelectObjectType::CAMERA_HEIGHTLOCK_BOX:
		{
			sscanf_s(lineDataChar, "%d,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f", &m_SaveData.ObjectType, &m_SaveData.Position.x, &m_SaveData.Position.y, &m_SaveData.Position.z,
				&m_SaveData.BoxMax.x, &m_SaveData.BoxMax.y, &m_SaveData.BoxMax.z,
				&m_SaveData.BoxMin.x, &m_SaveData.BoxMin.y, &m_SaveData.BoxMin.z,
				&m_SaveData.BoxPosition.x, &m_SaveData.BoxPosition.y, &m_SaveData.BoxPosition.z,&m_SaveData.HeightLockPosition);
			CCameraHeightLockBox* camerabox = new CCameraHeightLockBox(Vector3(m_SaveData.Position), Vector3(m_SaveData.BoxMax), Vector3(m_SaveData.BoxMin));
			if (!camerabox)
			{
				return;;
			}
			camerabox->GetCollisionBox()->AddBoxPosition(m_SaveData.BoxPosition);
			camerabox->SetHeightLockPosition(m_SaveData.HeightLockPosition);
			scene.lock()->AddGameObject<CCameraHeightLockBox>(ESceneObjectType::NOMESH_OBJECT, camerabox);
			if (mouse)
			{
				mouse->PushMapObject(camerabox, "CameraBox", EStaticMeshType::BOX, EEditorSelectObjectType::CAMERA_HEIGHTLOCK_BOX);
			}
			break;
		}
		case EEditorSelectObjectType::TUTORIAL_BOX:
		{
			sscanf_s(lineDataChar, "%d,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%d", &m_SaveData.ObjectType, &m_SaveData.Position.x, &m_SaveData.Position.y, &m_SaveData.Position.z,
				&m_SaveData.BoxMax.x, &m_SaveData.BoxMax.y, &m_SaveData.BoxMax.z,
				&m_SaveData.BoxMin.x, &m_SaveData.BoxMin.y, &m_SaveData.BoxMin.z,
				&m_SaveData.BoxPosition.x, &m_SaveData.BoxPosition.y, &m_SaveData.BoxPosition.z, &m_SaveData.TutorialNumber);
			CTutorialBox* tutorialbox = new CTutorialBox(Vector3(m_SaveData.Position), Vector3(m_SaveData.BoxMax), Vector3(m_SaveData.BoxMin));
			if (!tutorialbox)
			{
				return;;
			}
			tutorialbox->GetCollisionBox()->AddBoxPosition(m_SaveData.BoxPosition);
			tutorialbox->SetTutorialNumber(m_SaveData.TutorialNumber);
			scene.lock()->AddGameObject<CTutorialBox>(ESceneObjectType::NOMESH_OBJECT, tutorialbox);
			if (mouse)
			{
				mouse->PushMapObject(tutorialbox, "TutorialBox", EStaticMeshType::BOX, EEditorSelectObjectType::TUTORIAL_BOX);
			}
			break;
		}
		case EEditorSelectObjectType::PLAYER_START:
		{
			sscanf_s(lineDataChar, "%d,%f,%f,%f,%f,%f,%f", &m_SaveData.ObjectType, &m_SaveData.Position.x, &m_SaveData.Position.y, &m_SaveData.Position.z, &m_SaveData.Rotation.x, &m_SaveData.Rotation.y, &m_SaveData.Rotation.z);
			CPlayerStart* start = new CPlayerStart(Vector3(m_SaveData.Position), Vector3(m_SaveData.Rotation));
			if (!start)
			{
				return;;
			}
			scene.lock()->AddGameObject<CPlayerStart>(ESceneObjectType::NOMESH_OBJECT, start);
			if (mouse)
			{
				mouse->PushMapObject(start, "PlayerStart", EStaticMeshType::PLAYER_START, EEditorSelectObjectType::PLAYER_START);
			}
			break;
		}
		case EEditorSelectObjectType::SLIME:
		{
			sscanf_s(lineDataChar, "%d,%d,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%d,%d,%d,%d,%f,%f,%f,%f,%f,%f,%f,%f,%f,%d,%d", &m_SaveData.ObjectType, &m_SaveData.TargetPointMax, &m_SaveData.Position.x, &m_SaveData.Position.y, &m_SaveData.Position.z,
				&m_SaveData.Scale.x, &m_SaveData.Scale.y, &m_SaveData.Scale.z,
				&m_SaveData.Rotation.x, &m_SaveData.Rotation.y, &m_SaveData.Rotation.z,
				&m_SaveData.SphereRadius, &m_SaveData.SpherePosition.x, &m_SaveData.SpherePosition.y, &m_SaveData.SpherePosition.z,
				&m_SaveData.BodySphereRadius, &m_SaveData.BodySpherePosition.x, &m_SaveData.BodySpherePosition.y, &m_SaveData.BodySpherePosition.z,
				&m_SaveData.IsRespawn, &m_SaveData.RespawnTime, &m_SaveData.ItemDrop.ItemDropType, &m_SaveData.ItemDrop.Percent,
				&m_SaveData.BoxMax.x, &m_SaveData.BoxMax.y, &m_SaveData.BoxMax.z,
				&m_SaveData.BoxMin.x, &m_SaveData.BoxMin.y, &m_SaveData.BoxMin.z,
				&m_SaveData.BoxPosition.x, &m_SaveData.BoxPosition.y, &m_SaveData.BoxPosition.z,
				&m_SaveData.IsBodyCollisionCheck, &m_SaveData.IsDownLineCollisionCheck);
			if (m_SaveData.TargetPointMax <= 0)
			{
				return;
			}

			m_SaveData.TargetPoint.resize(m_SaveData.TargetPointMax);
			CMobEnemy* enemy = nullptr;
			enemy = dynamic_cast<CMobEnemy*>(CEnemy::Create(EEnemyType::SLIME, Vector3(m_SaveData.Position)));
			if (!enemy)
			{
				return;
			}
			//ターゲットポイントは最後にロードすること
			SetTargetPositionData(LineData, enemy);
			enemy->SetRotation(m_SaveData.Rotation);
			enemy->SetDefaultRotation(m_SaveData.Rotation);
			enemy->SetScale(m_SaveData.Scale);
			enemy->SetRespawn(m_SaveData.IsRespawn != 0, m_SaveData.RespawnTime);
			enemy->SetRangeSphere(m_SaveData.SpherePosition, m_SaveData.SphereRadius);
			enemy->SetBodySphere(m_SaveData.BodySpherePosition, m_SaveData.BodySphereRadius);
			enemy->SetBodySphereCollisionCheck(m_SaveData.IsBodyCollisionCheck != 0);
			enemy->SetDownLineCollisionCheck(m_SaveData.IsDownLineCollisionCheck != 0);
			CBox* box = enemy->GetRangeBox();
			box->AddBoxPosition(m_SaveData.BoxPosition);
			box->SetBoxSize(m_SaveData.BoxMax, m_SaveData.BoxMin);
			ItemDropData item;
			item.ItemDropType = m_SaveData.ItemDrop.ItemDropType;
			item.Percent = m_SaveData.ItemDrop.Percent;
			enemy->SetItemDropData(item);
			scene.lock()->AddGameObject<CEnemy>(ESceneObjectType::ENEMY, enemy);
			if (mouse)
			{
				mouse->PushMapObject(enemy, "Slime", EStaticMeshType::SLIME, EEditorSelectObjectType::SLIME);
			}
			break;
		}
		case EEditorSelectObjectType::MONKEY:
		{
			sscanf_s(lineDataChar, "%d,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%d,%d,%d,%d,%d", &m_SaveData.ObjectType, &m_SaveData.Position.x, &m_SaveData.Position.y, &m_SaveData.Position.z,
				&m_SaveData.Scale.x, &m_SaveData.Scale.y, &m_SaveData.Scale.z,
				&m_SaveData.Rotation.x, &m_SaveData.Rotation.y, &m_SaveData.Rotation.z,
				&m_SaveData.SphereRadius, &m_SaveData.SpherePosition.x, &m_SaveData.SpherePosition.y, &m_SaveData.SpherePosition.z,
				&m_SaveData.IsRespawn, &m_SaveData.RespawnTime, &m_SaveData.ItemDrop.ItemDropType, &m_SaveData.ItemDrop.Percent,&m_SaveData.AttackFrame);

			CMobEnemy* enemy = nullptr;
			enemy = dynamic_cast<CMobEnemy*>(CEnemy::Create(EEnemyType::MONKEY, Vector3(m_SaveData.Position)));
			if (!enemy)
			{
				return;
			}
			enemy->SetRotation(m_SaveData.Rotation);
			enemy->SetDefaultRotation(m_SaveData.Rotation);
			enemy->SetScale(m_SaveData.Scale);
			enemy->SetRespawn(m_SaveData.IsRespawn != 0, m_SaveData.RespawnTime);
			enemy->SetRangeSphere(m_SaveData.SpherePosition, m_SaveData.SphereRadius);
			ItemDropData item;
			item.ItemDropType = m_SaveData.ItemDrop.ItemDropType;
			item.Percent = m_SaveData.ItemDrop.Percent;
			enemy->SetItemDropData(item);
			scene.lock()->AddGameObject<CEnemy>(ESceneObjectType::ENEMY, enemy);
			if (mouse)
			{
				mouse->PushMapObject(enemy, "Monkey", EStaticMeshType::MONKEY, EEditorSelectObjectType::MONKEY);
			}
			break;
		}
		case EEditorSelectObjectType::THORNMAN:
		{
			sscanf_s(lineDataChar, "%d,%d,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%d,%d,%d,%d", &m_SaveData.ObjectType, &m_SaveData.TargetPointMax, &m_SaveData.Position.x, &m_SaveData.Position.y, &m_SaveData.Position.z,
				&m_SaveData.Scale.x, &m_SaveData.Scale.y, &m_SaveData.Scale.z,
				&m_SaveData.Rotation.x, &m_SaveData.Rotation.y, &m_SaveData.Rotation.z,
				&m_SaveData.BodySphereRadius, &m_SaveData.BodySpherePosition.x, &m_SaveData.BodySpherePosition.y, &m_SaveData.BodySpherePosition.z,
				&m_SaveData.IsRespawn, &m_SaveData.RespawnTime, &m_SaveData.ItemDrop.ItemDropType, &m_SaveData.ItemDrop.Percent);
			if (m_SaveData.TargetPointMax == 0)
			{
				return;
			}
			m_SaveData.TargetPoint.resize(m_SaveData.TargetPointMax);
			CMobEnemy* enemy = nullptr;
			enemy = dynamic_cast<CMobEnemy*>(CEnemy::Create(EEnemyType::THORN_MAN, Vector3(m_SaveData.Position)));
			if (!enemy)
			{
				return;
			}
			//ターゲットポイントは最後にロードすること
			SetTargetPositionData(LineData, enemy);
			enemy->SetRotation(m_SaveData.Rotation);
			enemy->SetDefaultRotation(m_SaveData.Rotation);
			enemy->SetScale(m_SaveData.Scale);
			enemy->SetRespawn(m_SaveData.IsRespawn != 0, m_SaveData.RespawnTime);
			enemy->SetBodySphere(m_SaveData.BodySpherePosition, m_SaveData.BodySphereRadius);
			ItemDropData item;
			item.ItemDropType = m_SaveData.ItemDrop.ItemDropType;
			item.Percent = m_SaveData.ItemDrop.Percent;
			enemy->SetItemDropData(item);
			scene.lock()->AddGameObject<CEnemy>(ESceneObjectType::ENEMY, enemy);
			if (mouse)
			{
				mouse->PushMapObject(enemy, "ThornMan", EStaticMeshType::THORNMAN, EEditorSelectObjectType::THORNMAN);
			}
			break;
		}
		case EEditorSelectObjectType::FOG:
		{
			sscanf_s(lineDataChar, "%d,%f,%f,%f", &m_SaveData.ObjectType, &m_SaveData.Position.x, &m_SaveData.Position.y, &m_SaveData.Position.z);
			FOG_CONSTANT& fog = CRenderer::GetFogConstant();
			fog.FogParam.x = m_SaveData.Position.x;
			fog.FogParam.y = m_SaveData.Position.y;
			fog.FogParam.z = m_SaveData.Position.z;
			break;
		}
	}
	if (mouse)
	{
		mouse->SetCreateNumber(-1);
	}
}

void CSaveAndLoad::SatBoxData(CBox* Box)
{
	if (!Box)
	{
		return;
	}
	m_SaveData.BoxMax = Box->GetMax();
	m_SaveData.BoxMin = Box->GetMin();
	m_SaveData.BoxPosition = Box->GetAddPosition();
}

void CSaveAndLoad::SatSphereData(CSphere* Sphere)
{
	if (!Sphere)
	{
		return;
	}
	m_SaveData.SphereRadius = Sphere->GetRadius();
	m_SaveData.SpherePosition = Sphere->GetAddPosition();
}

void CSaveAndLoad::SatBodySphereData(CSphere* Sphere)
{
	if (!Sphere)
	{
		return;
	}
	m_SaveData.BodySphereRadius = Sphere->GetRadius();
	m_SaveData.BodySpherePosition = Sphere->GetAddPosition();
}

void CSaveAndLoad::SetTargetPositionData(std::string& LineData, CEnemy* Enemy)
{
	if (!Enemy)
	{
		return;
	}
	const char* lineDataChar = LineData.c_str();
	//ターゲットポイント認識文字位置を検索
	int pos = static_cast<int>(LineData.find("T"));
	//その文字まで文字列を削除する
	LineData.erase(0, pos + 2);
	for (int i = 0; i < m_SaveData.TargetPointMax; i++)
	{//ターゲットポイント位置を代入
		sscanf_s(lineDataChar, "%f,%f,%f", &m_SaveData.TargetPoint[i].x, &m_SaveData.TargetPoint[i].y, &m_SaveData.TargetPoint[i].z);
		if (i == m_SaveData.TargetPointMax - 1)
		{
			Enemy->AddTargetPosition(m_SaveData.TargetPoint[i] - m_SaveData.Position);
			break;
		}
		//再び計算しターゲットポイント全て終わるまで続ける
		pos = static_cast<int>(LineData.find("T"));
		LineData.erase(0, pos + 2);
		Enemy->AddTargetPosition(m_SaveData.TargetPoint[i] - m_SaveData.Position);
	}
}

void CSaveAndLoad::PlayerSave()
{
	FILE *pFile;
	errno_t error;
	std::string name;
	name = "asset/save/player.txt";
	error = fopen_s(&pFile, name.c_str(), "wb");
	if (error != 0)
	{
		return;
	}
	
	for (const auto& data : m_PlayerSaveData)
	{
		fprintf(pFile, "%d,%d,%d\n", data.first, data.second.StageStarCount, data.second.StageAcornCount);
	}
	fclose(pFile);
}

void CSaveAndLoad::PlayerLoad()
{
	std::string name;
	name = "asset/save/player.txt";
	std::string data;
	
	std::ifstream ifs(name.c_str());
	if (!ifs)
	{
		std::cout << "ファイルが開けませんでした。" << std::endl;
		std::cin.get();
	}

	std::string lineData;
	while (!ifs.eof())
	{//1行ずつ読み込む
		std::getline(ifs, lineData);
		if (lineData.size() == 0)
		{
			break;
		}
		int sceneType = 0;
		int starCount = 0;
		int acornCount = 0;
		std::istringstream iss(lineData);
		sscanf_s(lineData.c_str(), "%d,%d,%d", &sceneType, &starCount, &acornCount);
		m_PlayerSaveData.emplace(static_cast<ESceneType>(sceneType), PlayerSaveData(starCount, acornCount));
	}
	ifs.close();
}