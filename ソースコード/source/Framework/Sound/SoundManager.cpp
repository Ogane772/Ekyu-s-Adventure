#include "SoundManager.h"
#include "Sound.h"
#include "SoundTypes.h"

constexpr int RESERVE_SOUND = 30;
#define BGM_PREFIX "asset/sound/bgm/"
#define SE_PREFIX "asset/sound/se/"
#define WAV_PREFIX ".wav"
#define ADD_BGM_PREFIX(str) BGM_PREFIX str WAV_PREFIX
#define ADD_SE_PREFIX(str) SE_PREFIX str WAV_PREFIX

CSoundManager::CSoundManager()
{
	m_SoundDataList =
	{
		{ ESoundIndex::FIRST_STAGE_BGM, SoundData(ADD_BGM_PREFIX("first_stage"), ESoundType::BGM, 0.6f, true, true) },
		{ ESoundIndex::BOSS_BGM, SoundData(ADD_BGM_PREFIX("boss"), ESoundType::BGM, 0.4f, true, true) },
		{ ESoundIndex::TUTORIAL_BGM, SoundData(ADD_BGM_PREFIX("tutorial"), ESoundType::BGM, 0.7f, true, true) },
		{ ESoundIndex::STAGESELECT_BGM, SoundData(ADD_BGM_PREFIX("stageselect"), ESoundType::BGM, 0.7f, true, true) },
		{ ESoundIndex::BOM_SE, SoundData(ADD_SE_PREFIX("bigbom"), ESoundType::SE, 1.0f, false) },
		{ ESoundIndex::BOM_START, SoundData(ADD_SE_PREFIX("bom_start"), ESoundType::SE, 0.9f, false) },
		{ ESoundIndex::BOM_THROW, SoundData(ADD_SE_PREFIX("bom_throw"), ESoundType::SE, 0.6f, false) },
		{ ESoundIndex::DAMAGE, SoundData(ADD_SE_PREFIX("damage"), ESoundType::SE, 1.0f, false) },
		{ ESoundIndex::DIVE, SoundData(ADD_SE_PREFIX("dive"), ESoundType::SE, 1.0f, false) },
		{ ESoundIndex::ENEMY_DEATH, SoundData(ADD_SE_PREFIX("enemy_death"), ESoundType::SE, 1.0f, false) },
		{ ESoundIndex::ItemDropData, SoundData(ADD_SE_PREFIX("item_drop"), ESoundType::SE, 1.3f, false) },
		{ ESoundIndex::JUMP_END, SoundData(ADD_SE_PREFIX("jump_end"), ESoundType::SE, 0.7f, false) },
		{ ESoundIndex::STUN, SoundData(ADD_SE_PREFIX("stun"), ESoundType::SE, 0.7f, true) },
		{ ESoundIndex::PLAYER_DAMAGE, SoundData(ADD_SE_PREFIX("player_damage"), ESoundType::SE, 1.0f, false) },
		{ ESoundIndex::TAIL_HIT, SoundData(ADD_SE_PREFIX("tail_hit"), ESoundType::SE, 1.0f, false) },
		{ ESoundIndex::WALL_HIT, SoundData(ADD_SE_PREFIX("wall_hit"), ESoundType::SE, 1.0f, false) },
		{ ESoundIndex::SWING, SoundData(ADD_SE_PREFIX("swing"), ESoundType::SE, 0.9f, true) },
		{ ESoundIndex::JUMP_HIT, SoundData(ADD_SE_PREFIX("jump_hit"), ESoundType::SE, 1.6f, false) },
		{ ESoundIndex::BOSS_JUMP_HIT, SoundData(ADD_SE_PREFIX("jump_hit2"), ESoundType::SE, 1.6f, false) },
		{ ESoundIndex::FINAL_JUMP, SoundData(ADD_SE_PREFIX("final_jump"), ESoundType::SE, 1.0f, true) },
		{ ESoundIndex::DIVE_HIT, SoundData(ADD_SE_PREFIX("dive_hit"), ESoundType::SE, 1.5f, false) },
		{ ESoundIndex::SPIN_JUMP, SoundData(ADD_SE_PREFIX("spin_jump"), ESoundType::SE, 1.0f, false) },
		{ ESoundIndex::TAIL_ATTACK, SoundData(ADD_SE_PREFIX("tail_attack"), ESoundType::SE, 1.6f, false) },
		{ ESoundIndex::HIPDROP_START, SoundData(ADD_SE_PREFIX("hipdrop_start"), ESoundType::SE, 1.5f, false) },
		{ ESoundIndex::HIPDROP_END, SoundData(ADD_SE_PREFIX("hipdrop_end"), ESoundType::SE, 1.6f, false) },
		{ ESoundIndex::WALL_SLIP, SoundData(ADD_SE_PREFIX("wall_slip"), ESoundType::SE, 0.2f, true) },
		{ ESoundIndex::MOVE_CHANGE_JUMP, SoundData(ADD_SE_PREFIX("MoveChangeJump"), ESoundType::SE, 1.0f, false) },
		{ ESoundIndex::CROUCHING_JUMP, SoundData(ADD_SE_PREFIX("crouching_jump"), ESoundType::SE, 1.0f, false) },
		{ ESoundIndex::DASH_JUMP, SoundData(ADD_SE_PREFIX("dash_jump"), ESoundType::SE, 1.0f, false) },
		{ ESoundIndex::FIRST_JUMP, SoundData(ADD_SE_PREFIX("first_jump"), ESoundType::SE, 1.0f, false) },
		{ ESoundIndex::SECOND_JUMP, SoundData(ADD_SE_PREFIX("second_jump"), ESoundType::SE, 1.0f, false) },
		{ ESoundIndex::DOWN, SoundData(ADD_SE_PREFIX("down"), ESoundType::SE, 1.9f, false) },
		{ ESoundIndex::DOWN2, SoundData(ADD_SE_PREFIX("down2"), ESoundType::SE, 1.2f, false) },
		{ ESoundIndex::DEATH_MOVE, SoundData(ADD_SE_PREFIX("death_move"), ESoundType::SE, 1.7f, false) },
		{ ESoundIndex::ACORN_HIT, SoundData(ADD_SE_PREFIX("acorn_hit"), ESoundType::SE, 1.7f, false) },
		{ ESoundIndex::HEAL, SoundData(ADD_SE_PREFIX("heal"), ESoundType::SE, 1.7f, false) },
		{ ESoundIndex::CLIFF_HIT, SoundData(ADD_SE_PREFIX("cliff_hit"), ESoundType::SE, 1.3f, false) },
		{ ESoundIndex::CLIFF_UP, SoundData(ADD_SE_PREFIX("cliff_up"), ESoundType::SE, 0.8f, false) },
		{ ESoundIndex::STAR_MOVE, SoundData(ADD_SE_PREFIX("star_move"), ESoundType::SE, 1.1f, false) },
		{ ESoundIndex::DOOR_OPEN, SoundData(ADD_SE_PREFIX("door_open"), ESoundType::SE, 1.5f, true) },
		{ ESoundIndex::GETSTAR, SoundData(ADD_SE_PREFIX("getstar"), ESoundType::SE, 0.7f, false) },
		{ ESoundIndex::SLIME_DISCOVERY, SoundData(ADD_SE_PREFIX("slime_discovery"), ESoundType::SE, 1.3f, false) },
		{ ESoundIndex::CLEAR_DOOR, SoundData(ADD_SE_PREFIX("clear_door"), ESoundType::SE, 1.3f, false) },
		{ ESoundIndex::SLIME_WALK, SoundData(ADD_SE_PREFIX("slime_walk"), ESoundType::SE, 3.3f, true) },
		{ ESoundIndex::SLIME_DASH, SoundData(ADD_SE_PREFIX("slime_dash"), ESoundType::SE, 3.3f, true) },
		{ ESoundIndex::RESPAWN, SoundData(ADD_SE_PREFIX("respawn"), ESoundType::SE, 2.0f, false) },
		{ ESoundIndex::DORAMING, SoundData(ADD_SE_PREFIX("doraming"), ESoundType::SE, 1.0f, false) },
		{ ESoundIndex::GORILLA_JUMP, SoundData(ADD_SE_PREFIX("goriilla_jump"), ESoundType::SE, 1.0f, false) },
		{ ESoundIndex::GORILLA_ATTACK_STANDBY, SoundData(ADD_SE_PREFIX("gorilla_attack_standby"), ESoundType::SE, 1.0f, false) },
		{ ESoundIndex::GORILLA_WALL_HIT, SoundData(ADD_SE_PREFIX("gorilla_wallhit"), ESoundType::SE, 1.0f, false) },
		{ ESoundIndex::GORILLA_ATTACK_HEAL, SoundData(ADD_SE_PREFIX("gorilla_attack_heal"), ESoundType::SE, 1.0f, false) },
		{ ESoundIndex::GORILLA_JUMP_ATTACK, SoundData(ADD_SE_PREFIX("gorilla_jump_attack"), ESoundType::SE, 1.2f, false) },
		{ ESoundIndex::GORILLA_JUMP_LOOP, SoundData(ADD_SE_PREFIX("gorilla_jump_loop"), ESoundType::SE, 0.7f, true) },
		{ ESoundIndex::HIT_END, SoundData(ADD_SE_PREFIX("hit_end"), ESoundType::SE, 1.0f, false) },
		{ ESoundIndex::BOSS_DEATH, SoundData(ADD_SE_PREFIX("boss_death"), ESoundType::SE, 1.0f, false) },
		{ ESoundIndex::SELECT_STAGE, SoundData(ADD_SE_PREFIX("select_stage"), ESoundType::SE, 1.0f, false) },
		{ ESoundIndex::SELECT_FRAME, SoundData(ADD_SE_PREFIX("select_frame"), ESoundType::SE, 1.0f, false) },
		{ ESoundIndex::FADEOUT, SoundData(ADD_SE_PREFIX("FadeOut"), ESoundType::SE, 0.8f, false) },
		{ ESoundIndex::HIPDROP_JUMP, SoundData(ADD_SE_PREFIX("hipdrop_jump"), ESoundType::SE, 1.3f, false) },
		{ ESoundIndex::DEATH, SoundData(ADD_SE_PREFIX("death"), ESoundType::SE, 1.3f, false) },
		{ ESoundIndex::WALL_JUMP, SoundData(ADD_SE_PREFIX("wall_jump"), ESoundType::SE, 1.0f, false) },
		{ ESoundIndex::FOOTSTEPS_NORMAL, SoundData(ADD_SE_PREFIX("footsteps_normal"), ESoundType::SE, 0.21f, false, true) },
		{ ESoundIndex::FOOTSTEPS_BRIDGE, SoundData(ADD_SE_PREFIX("footsteps_bridge"), ESoundType::SE, 0.45f, false, true) },
		{ ESoundIndex::FOOTSTEPS_ROCK, SoundData(ADD_SE_PREFIX("footsteps_rock"), ESoundType::SE, 0.45f, false, true) },
		{ ESoundIndex::THORNMAN_DEATH, SoundData(ADD_SE_PREFIX("thornman_death"), ESoundType::SE, 1.0f, false) },
		{ ESoundIndex::THORNMAN_DISCOVERY, SoundData(ADD_SE_PREFIX("thornman_discovery"), ESoundType::SE, 1.3f, false) },
		{ ESoundIndex::IN_PAUSE, SoundData(ADD_SE_PREFIX("in_pause"), ESoundType::SE, 0.7f, false) },
		{ ESoundIndex::OUT_PAUSE, SoundData(ADD_SE_PREFIX("out_pause"), ESoundType::SE, 0.7f, false) },
		{ ESoundIndex::PAUSE_CURSOR_MOVE, SoundData(ADD_SE_PREFIX("pause_cursor_move"), ESoundType::SE, 0.3f, false) },
		{ ESoundIndex::PAUSE_MENU_OK, SoundData(ADD_SE_PREFIX("pause_menu_ok"), ESoundType::SE, 0.7f, false) },
		{ ESoundIndex::PAUSE_MENU_BACK, SoundData(ADD_SE_PREFIX("pause_menu_back"), ESoundType::SE, 0.7f, false) },
		{ ESoundIndex::PAUSE_OPTION_CHANGE, SoundData(ADD_SE_PREFIX("PauseOptionChange"), ESoundType::SE, 0.7f, false) },
		{ ESoundIndex::GROUND_SLIP, SoundData(ADD_SE_PREFIX("ground_slip"), ESoundType::SE, 0.05f, true) },
	};

	//サウンドのセットアップ
	HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
	if (hr == S_FALSE)
	{
		return;
	}

	XAudio2Create(&m_XAudio);
	if (!m_XAudio)
	{
		return;
	}

	m_XAudio->CreateMasteringVoice(&m_MasteringVoice);
	if (!m_MasteringVoice)
	{
		return;
	}

	DWORD channelMask;
	m_MasteringVoice->GetChannelMask(&channelMask);

	X3DAudioInitialize(channelMask, X3DAUDIO_SPEED_OF_SOUND, m_X3DAudioHandle);

	m_PlaySoundList.reserve(RESERVE_SOUND);
}

CSoundManager::~CSoundManager()
{
	m_PlaySoundList.clear();
	m_PlaySoundList.shrink_to_fit();

	if (m_MasteringVoice)
	{
		m_MasteringVoice->DestroyVoice();
		m_MasteringVoice = nullptr;
	}

	if (m_XAudio)
	{
		m_XAudio->Release();
		m_XAudio = nullptr;
	}

	CoUninitialize();
}

void CSoundManager::Update()
{
	for (std::unique_ptr<CSound>& sound : m_PlaySoundList)
	{
		if (!sound)
		{
			continue;
		}
		sound->Update();
	}

	m_PlaySoundList.erase(std::remove_if(m_PlaySoundList.begin(), m_PlaySoundList.end(),[](const std::unique_ptr<CSound>& Sound) 
	{
		if (!Sound)
		{
			return false;
		}
		return Sound->IsSoundEnd();
	}),
		m_PlaySoundList.end()
	);
}

UINT CSoundManager::Play(const ESoundIndex SoundIndex)
{
	if (!m_SoundDataList.contains(SoundIndex))
	{
		return UINT_MAX;
	}
	UINT id = UINT_MAX;
	const SoundData& soundData = m_SoundDataList[SoundIndex];
	id = PlayCache(SoundIndex, soundData);
	if (id != UINT_MAX)
	{
		return id;
	}

	std::unique_ptr<CSound> sound = std::make_unique<CSound>(m_XAudio, m_MasteringVoice);
	if (!sound)
	{
		return UINT_MAX;
	}
	id = sound->GetID();
	sound->Load(SoundIndex, soundData);
	sound->Play();
	m_PlaySoundList.emplace_back(std::move(sound));
	return id;
}

void CSoundManager::Play(const UINT Id)
{
	for (std::unique_ptr<CSound>& sound : m_PlaySoundList)
	{
		if (!sound)
		{
			continue;
		}
		if (Id != sound->GetID())
		{
			continue;
		}
		sound->Replay();
		break;
	}
}

UINT CSoundManager::Play3DSound(const ESoundIndex SoundIndex, const Vector3& Position)
{
	const UINT id = Play(SoundIndex);
	if (id == UINT_MAX)
	{
		return id;
	}
	if (CSound* sound = m_PlaySoundList.back().get())
	{
		sound->Play3DSound(m_X3DAudioHandle, Position);
	}
	return id;
}

void CSoundManager::Stop(const ESoundIndex SoundIndex)
{
	if (!m_SoundDataList.contains(SoundIndex))
	{
		return;
	}

	for (std::unique_ptr<CSound>& sound : m_PlaySoundList)
	{
		if (!sound)
		{
			continue;
		}
		if (SoundIndex != sound->GetSoundIndex())
		{
			continue;
		}
		sound->Stop();
	}
}

void CSoundManager::Stop(const UINT Id)
{
	for (std::unique_ptr<CSound>& sound : m_PlaySoundList)
	{
		if (!sound)
		{
			continue;
		}
		if (Id != sound->GetID())
		{
			continue;
		}
		sound->Stop();
		break;
	}
}

void CSoundManager::DeleteSoundIndex(const ESoundIndex SoundIndex)
{
	if (!m_SoundDataList.contains(SoundIndex))
	{
		return;
	}

	for (std::unique_ptr<CSound>& sound : m_PlaySoundList)
	{
		if (!sound)
		{
			continue;
		}
		if (SoundIndex != sound->GetSoundIndex())
		{
			continue;
		}
		sound->EnableSoundEnd();
	}
}

void CSoundManager::DeleteSoundID(const UINT Id)
{
	for (std::unique_ptr<CSound>& sound : m_PlaySoundList)
	{
		if (!sound)
		{
			continue;
		}
		if (Id != sound->GetID())
		{
			continue;
		}
		sound->EnableSoundEnd();
		break;
	}
}

void CSoundManager::ReplayBGM()
{
	for (std::unique_ptr<CSound>& sound : m_PlaySoundList)
	{
		if (!sound)
		{
			continue;
		}
		if (sound->GetSoundType() != ESoundType::BGM)
		{
			continue;
		}
		sound->Replay();
		break;
	}
}

void CSoundManager::StopBGM()
{
	for (std::unique_ptr<CSound>& sound : m_PlaySoundList)
	{
		if (!sound)
		{
			continue;
		}
		if (sound->GetSoundType() != ESoundType::BGM)
		{
			continue;
		}
		sound->Stop();
		break;
	}
}

void CSoundManager::DeleteBGM()
{
	for (std::unique_ptr<CSound>& sound : m_PlaySoundList)
	{
		if (!sound)
		{
			continue;
		}
		if (sound->GetSoundType() != ESoundType::BGM)
		{
			continue;
		}
		sound->EnableSoundEnd();
		break;
	}
}
void CSoundManager::Update3DSoundPosition(const UINT Id, const Vector3& Position)
{
	for (std::unique_ptr<CSound>& sound : m_PlaySoundList)
	{
		if (!sound)
		{
			continue;
		}
		if (Id != sound->GetID())
		{
			continue;
		}
		sound->Update3DSoundPosition(Position);
		break;
	}
}

void CSoundManager::AllClear()
{
	m_PlaySoundList.clear();
	m_PlaySoundList.shrink_to_fit();
	m_PlaySoundList.reserve(RESERVE_SOUND);
}

void CSoundManager::AllStop()
{
	for (std::unique_ptr<CSound>& sound : m_PlaySoundList)
	{
		if (!sound)
		{
			continue;
		}
		sound->Stop();
	}
}

void CSoundManager::AllReplay()
{
	for (std::unique_ptr<CSound>& sound : m_PlaySoundList)
	{
		if (!sound)
		{
			continue;
		}
		sound->Replay();
	}
}

UINT CSoundManager::PlayCache(const ESoundIndex SoundIndex, const SoundData& SoundData)
{
	if (!SoundData.IsCache)
	{
		return UINT_MAX;
	}
	//キャッシュを使用する場合はすでに存在する同サウンドを再生する
	for (std::unique_ptr<CSound>& sound : m_PlaySoundList)
	{
		if (!sound)
		{
			continue;
		}
		if (SoundIndex != sound->GetSoundIndex())
		{
			continue;
		}
		if (sound->IsSoundEnd())
		{
			continue;
		}
		sound->Replay();
		return sound->GetID();
	}
	return UINT_MAX;
}
