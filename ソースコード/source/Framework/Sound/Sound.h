#pragma once
#include <xaudio2.h>
#include <x3daudio.h>
#include "../Calculation/Vector.h"
#include "SoundTypes.h"
using namespace FrameWork;
enum class ESoundType;

class CSound
{
private:
	IXAudio2* m_XAudio = nullptr;
	IXAudio2MasteringVoice* m_MasteringVoice = nullptr;
	X3DAUDIO_HANDLE m_X3DAudioHandle = {};
	IXAudio2SourceVoice* m_SourceVoice = nullptr;
	BYTE* m_ByteAudioData = nullptr;

	int	m_AudioBytes = 0;
	int	m_PlayLength = 0;
	SoundData m_SoundData = SoundData();
	Vector3 m_3DSoundPosition;
	bool m_IsSoundEnd = false;
	bool m_Is3DSound = false;
	bool m_IsCache = false;
	ESoundIndex m_SoundIndex;
	UINT m_Id = 0;

	static UINT GenerateID();

public:
	CSound(IXAudio2* XAudio, IXAudio2MasteringVoice* MasteringVoice);
	~CSound();

	void Load(const ESoundIndex SoundIndex, const SoundData& SoundData);
	void Update();
	UINT Play();
	void Replay();
	void Stop();
	void Play3DSound(const X3DAUDIO_HANDLE& m_X3DAudioHandle, const Vector3& PlayPosition);
	void Update3DSoundPosition(const Vector3& Position);
	bool IsSoundEnd() const { return m_IsSoundEnd; }
	void EnableSoundEnd() { m_IsSoundEnd = true; }
	UINT GetID() const { return m_Id; }
	ESoundIndex GetSoundIndex() const { return m_SoundIndex; }
	ESoundType GetSoundType() const;
};