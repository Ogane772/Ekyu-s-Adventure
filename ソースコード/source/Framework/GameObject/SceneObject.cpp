#include "SceneObject.h"
#include "../Scene/SceneManager.h"
#include "../Sound/SoundManager.h"
#include "../Sound/SoundTypes.h"
#include "../Scene/Scene.h"
#include "../TextureLoader/TextureLoader.h"
#include "../System/SystemContext.h"

bool CSceneObject::Destroy()
{
	if (m_IsDestroy)
	{
		UnInit();
		delete this;
		return true;
	}
	else
	{
		return false;
	}
}

UINT CSceneObject::PlaySoundIndex(const ESoundIndex SoundIndex)
{
	CSoundManager* instance = CSoundManager::GetInstance();
	if (instance)
	{
		return instance->Play(SoundIndex);
	}
	return -1;
}

void CSceneObject::PlaySoundID(const UINT Id)
{
	CSoundManager* instance = CSoundManager::GetInstance();
	if (instance)
	{
		instance->Play(Id);
	}
}

void CSceneObject::StopSoundIndex(const ESoundIndex SoundIndex)
{
	CSoundManager* instance = CSoundManager::GetInstance();
	if (instance)
	{
		instance->Stop(SoundIndex);
	}
}

void CSceneObject::StopSoundID(const UINT Id)
{
	CSoundManager* instance = CSoundManager::GetInstance();
	if (instance)
	{
		instance->Stop(Id);
	}
}

void CSceneObject::DeleteSoundIndex(const ESoundIndex SoundIndex)
{
	CSoundManager* instance = CSoundManager::GetInstance();
	if (instance)
	{
		instance->DeleteSoundIndex(SoundIndex);
	}
}

void CSceneObject::DeleteSoundID(const UINT Id)
{
	CSoundManager* instance = CSoundManager::GetInstance();
	if (instance)
	{
		instance->DeleteSoundID(Id);
	}
}

void CSceneObject::ReplayBGM()
{
	CSoundManager* instance = CSoundManager::GetInstance();
	if (instance)
	{
		instance->ReplayBGM();
	}
}

void CSceneObject::StopBGM()
{
	CSoundManager* instance = CSoundManager::GetInstance();
	if (instance)
	{
		instance->StopBGM();
	}
}

void CSceneObject::DeleteBGM()
{
	CSoundManager* instance = CSoundManager::GetInstance();
	if (instance)
	{
		instance->DeleteBGM();
	}
}

void CSceneObject::AllSoundStop()
{
	CSoundManager* instance = CSoundManager::GetInstance();
	if (instance)
	{
		instance->AllStop();
	}
}

void CSceneObject::AllSoundReplay()
{
	CSoundManager* instance = CSoundManager::GetInstance();
	if (instance)
	{
		instance->AllReplay();
	}
}

std::weak_ptr<ID3D11ShaderResourceView> CSceneObject::LoadTexture(const ETextureType TextureType)
{
	CTextureLoader* instance = CTextureLoader::GetInstance();
	if (!instance)
	{
		return std::weak_ptr<ID3D11ShaderResourceView>();
	}
	return instance->Load(TextureType);
}

std::weak_ptr<ID3D11ShaderResourceView> CSceneObject::LoadTexture(const std::string& FileName)
{
	CTextureLoader* instance = CTextureLoader::GetInstance();
	if (!instance)
	{
		return std::weak_ptr<ID3D11ShaderResourceView>();
	}
	return instance->Load(FileName);
}

void CSceneObject::ReleaseTexture(std::weak_ptr<ID3D11ShaderResourceView> Texture)
{
	CTextureLoader* instance = CTextureLoader::GetInstance();
	if (instance)
	{
		instance->Release(Texture);
	}
}

CPlayer* CSceneObject::GetPlayer() const
{
	CSceneManager* manager = CSceneManager::GetInstance();
	if (!manager)
	{
		return nullptr;
	}
	return manager->GetPlayer();
}

CCamera* CSceneObject::GetCamera() const
{
	CSceneManager* manager = CSceneManager::GetInstance();
	if (!manager)
	{
		return nullptr;
	}
	return manager->GetCamera();
}

CMouseCursor* CSceneObject::GetMouseCursor() const
{
	CSceneManager* manager = CSceneManager::GetInstance();
	if (!manager)
	{
		return nullptr;
	}
	return manager->GetMouseCursor();
}

std::weak_ptr<CScene> CSceneObject::GetScene() const
{
	CSceneManager* manager = CSceneManager::GetInstance();
	if (!manager)
	{
		return std::weak_ptr<CScene>();
	}
	return manager->GetScene();
}

bool CSceneObject::IsEditMode() const
{
	CSystemContext* system = CSystemContext::GetInstance();
	if (!system)
	{
		return false;
	}
	return system->IsEditMode();
}

void CSceneObject::SetEvent(const bool Type)
{
	CSystemContext* system = CSystemContext::GetInstance();
	if (!system)
	{
		return;
	}
	return system->SetEvent(Type);
}

bool CSceneObject::IsEvent() const
{
	CSystemContext* system = CSystemContext::GetInstance();
	if (!system)
	{
		return false;
	}
	return system->IsEvent();
}