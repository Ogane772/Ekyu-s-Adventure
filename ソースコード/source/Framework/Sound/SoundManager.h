#pragma once
//各サウンドを管理するクラス
#include "../Singleton/Singleton.h"
#include "../Calculation/Vector.h"
#include <xaudio2.h>
#include <x3daudio.h>
#include <memory>
#include <vector>
#include <unordered_map>
class CSound;
enum class ESoundIndex;
struct SoundData;
using namespace FrameWork;

class CSoundManager : public CSingleton<CSoundManager>
{
public:
	CSoundManager();
	~CSoundManager();

	void Update();
	UINT Play(const ESoundIndex SoundIndex);
	void Play(const UINT Id);
	UINT Play3DSound(const ESoundIndex SoundIndex, const Vector3& Position);
	void Stop(const ESoundIndex SoundIndex);
	void Stop(const UINT Id);
	void DeleteSoundIndex(const ESoundIndex SoundIndex);
	void DeleteSoundID(const UINT Id);
	void ReplayBGM();
	void DeleteBGM();
	void StopBGM();
	void Update3DSoundPosition(const UINT Id, const Vector3& Position);
	void AllClear();
	void AllStop();
	void AllReplay();

private:
	IXAudio2* m_XAudio = nullptr;
	IXAudio2MasteringVoice* m_MasteringVoice = nullptr;
	X3DAUDIO_HANDLE m_X3DAudioHandle = {};
	std::unordered_map<ESoundIndex, SoundData> m_SoundDataList;
	std::vector<std::unique_ptr<CSound>> m_PlaySoundList;

	UINT PlayCache(const ESoundIndex SoundIndex, const SoundData& SoundData);
};

