#include "Scene.h"
#include "../Sound/SoundManager.h"
#include "../Scene/SceneTypes.h"
#include "../Shadow/ShadowMap.h"
#include "../GameObject/SceneObject.h"
#include "../../Game/Object/HitBlock/HitBlock.h"
#include "../../Game/Character/Enemy/Mob/Slime/Slime.h"
#include "../../Game/Character/Enemy/Mob/Monkey/Monkey.h"
#include "../../Game/Character/Enemy/Mob/ThornMan/ThornMan.h"
#include "../../Game/Character/Enemy/Boss/Gorilla.h"

void CScene::PlayBGM(const ESoundIndex SoundIndex)
{
	if (CSoundManager* instance = CSoundManager::GetInstance())
	{
		instance->Play(SoundIndex);
	}
}

ESceneType CScene::Update(float DeltaTime)
{
	const int max = static_cast<int>(ESceneObjectType::MAX);
	for (int i = 0; i < max; i++)
	{
		for (CSceneObject* object : m_SceneObject[i])
		{
			object->PreUpdate(DeltaTime);
		}
		//削除フラグが立っているオブジェクトを削除かつ要素を取り除く
		m_SceneObject[i].erase(std::remove_if(m_SceneObject[i].begin(), m_SceneObject[i].end(), [](CSceneObject* object)
		{
			const bool isDestroy = object->Destroy();
			if (isDestroy)
			{
				object = nullptr;
			}
			return isDestroy;
		}),
			m_SceneObject[i].end()
		);
	}
	return ESceneType::NONE;
}

void CScene::UnInit()
{
	const int max = static_cast<int>(ESceneObjectType::MAX);
	for (int i = 0; i < max; i++)
	{
		for (CSceneObject* object : m_SceneObject[i])
		{
			object->UnInit();
			delete object;
		}
		m_SceneObject[i].clear();
	}
	if (CSoundManager* sound = CSoundManager::GetInstance())
	{
		sound->DeleteBGM();
	}
}

void CScene::Draw()
{
	//通常描画用ビューポート設定
	CShadowMap::SetNormalViewPort();
	const int max = static_cast<int>(ESceneObjectType::MAX);
	for (int i = 0; i < max; i++)
	{
		for (CSceneObject* object : m_SceneObject[i])
		{
			object->PreDraw();
		}
	}
}

void CScene::ShadowDraw()
{
	//デプス用ビューポート設定
	CShadowMap::SetDepthViewPort();
	const int max = static_cast<int>(ESceneObjectType::MAX);
	for (int i = 0; i < max; i++)
	{
		for (CSceneObject* object : m_SceneObject[i])
		{
			//影用ビューポート設定							
			object->ShadowDraw();
		}
	}
}

void CScene::GetGameEnemyObjects(const ESceneObjectType Type, std::vector<CEnemy*>& OutList)
{
	for (CSceneObject* object : m_SceneObject[static_cast<int>(Type)])
	{
		if (typeid(*object) == typeid(CSlime))
		{
			OutList.emplace_back((CEnemy*)object);
		}
		else if (typeid(*object) == typeid(CMonkey))
		{
			OutList.emplace_back((CEnemy*)object);
		}
		else if (typeid(*object) == typeid(CThornMan))
		{
			OutList.emplace_back((CEnemy*)object);
		}
		else if (typeid(*object) == typeid(CGorilla))
		{
			OutList.emplace_back((CEnemy*)object);
		}
	}
}

void CScene::GetGameMobEnemyObjects(const ESceneObjectType Type, std::vector<CMobEnemy*>& OutList)
{
	for (CSceneObject* object : m_SceneObject[static_cast<int>(Type)])
	{
		if (typeid(*object) == typeid(CSlime))
		{
			OutList.emplace_back((CMobEnemy*)object);
		}
		else if (typeid(*object) == typeid(CMonkey))
		{
			OutList.emplace_back((CMobEnemy*)object);
		}
		else if (typeid(*object) == typeid(CThornMan))
		{
			OutList.emplace_back((CMobEnemy*)object);
		}
	}
}

void CScene::GetHitBlockObjects(const ESceneObjectType Type, std::vector<CHitBlock*>& OutList)
{
	for (CSceneObject* object : m_SceneObject[static_cast<int>(Type)])
	{
		if (typeid(*object) == typeid(CHitBlock))
		{
			OutList.emplace_back((CHitBlock*)object);
		}
	}
}