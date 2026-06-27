#include "Sound.h"
#include "../Scene/SceneManager.h"
#include "../Camera/Camera.h"

UINT CSound::GenerateID()
{
	static UINT counter = 0;
	return counter++;
}

CSound::CSound(IXAudio2* XAudio, IXAudio2MasteringVoice* MasteringVoice)
	: m_XAudio(XAudio), m_MasteringVoice(MasteringVoice), m_SoundIndex(ESoundIndex::NONE), m_Id(GenerateID())
{
}

CSound::~CSound()
{
	if (m_SourceVoice)
	{
		m_SourceVoice->Stop();
		m_SourceVoice->DestroyVoice();
		m_SourceVoice = nullptr;
	}

	if (m_ByteAudioData)
	{
		delete[] m_ByteAudioData;
		m_ByteAudioData = nullptr;
	}
}

void CSound::Load(const ESoundIndex SoundIndex, const SoundData& SoundData)
{
	m_SoundIndex = SoundIndex;
	m_SoundData = SoundData;
	// サウンドデータ読込
	WAVEFORMATEX wfx = { 0 };
	
	HMMIO hmmio = {};
	MMIOINFO mmioinfo = { 0 };
	MMCKINFO riffchunkinfo = { 0 };
	MMCKINFO datachunkinfo = { 0 };
	MMCKINFO mmckinfo = { 0 };
	UINT32 buflen;
	LONG readlen;

	hmmio = mmioOpen((LPSTR)SoundData.FilePath.c_str(), &mmioinfo, MMIO_READ);
	if (!hmmio)
	{
		return;
	}

	riffchunkinfo.fccType = mmioFOURCC('W', 'A', 'V', 'E');
	mmioDescend(hmmio, &riffchunkinfo, nullptr, MMIO_FINDRIFF);

	mmckinfo.ckid = mmioFOURCC('f', 'm', 't', ' ');
	mmioDescend(hmmio, &mmckinfo, &riffchunkinfo, MMIO_FINDCHUNK);

	if (mmckinfo.cksize >= sizeof(WAVEFORMATEX))
	{
		mmioRead(hmmio, (HPSTR)&wfx, sizeof(wfx));
	}
	else
	{
		PCMWAVEFORMAT pcmwf = { 0 };
		mmioRead(hmmio, (HPSTR)&pcmwf, sizeof(pcmwf));
		memset(&wfx, 0x00, sizeof(wfx));
		memcpy(&wfx, &pcmwf, sizeof(pcmwf));
		wfx.cbSize = 0;
	}
	mmioAscend(hmmio, &mmckinfo, 0);

	datachunkinfo.ckid = mmioFOURCC('d', 'a', 't', 'a');
	mmioDescend(hmmio, &datachunkinfo, &riffchunkinfo, MMIO_FINDCHUNK);

	buflen = datachunkinfo.cksize;
	m_ByteAudioData = new unsigned char[buflen];
	readlen = mmioRead(hmmio, (HPSTR)m_ByteAudioData, buflen);

	m_AudioBytes = readlen;
	m_PlayLength = readlen / wfx.nBlockAlign;

	mmioClose(hmmio, 0);
	
	// サウンドソース生成
	//ソースボイスにデータを詰め込む	
	XAUDIO2_SEND_DESCRIPTOR sendDescriptors[1];
	sendDescriptors[0].Flags = XAUDIO2_SEND_USEFILTER;
	sendDescriptors[0].pOutputVoice = m_MasteringVoice;
	const XAUDIO2_VOICE_SENDS sendList = { 1, sendDescriptors };

	if (m_XAudio)
	{
		m_XAudio->CreateSourceVoice(&m_SourceVoice, &wfx, 0, 2.0f, NULL, &sendList);
	}
	if (m_SourceVoice)
	{
		m_SourceVoice->SetVolume(SoundData.Volume);
	}
}

void CSound::Update()
{
	if (m_IsSoundEnd)
	{
		return;
	}
	if (!m_SourceVoice)
	{
		return;
	}

	if (m_Is3DSound)
	{
		CSceneManager* sceneManager = CSceneManager::GetInstance();
		if (!sceneManager)
		{
			return;
		}
		//聞き手
		CCamera* camera = sceneManager->GetCamera();
		if (!camera)
		{
			return;
		}
		const Vector3 pos = camera->GetPosition();
		const Vector3 front = camera->GetFront();
		X3DAUDIO_LISTENER listener = {};
		listener.Position = { pos.x, pos.y, pos.z };
		listener.OrientFront = { front.x, front.y, front.z };
		listener.OrientTop = { 0.0f, 1.0f, 0.0f };

		//音源
		X3DAUDIO_EMITTER emitter = {};
		emitter.Position = { m_3DSoundPosition.x, m_3DSoundPosition.y, m_3DSoundPosition.z }; // 右にある音
		emitter.ChannelCount = 1;
		emitter.CurveDistanceScaler = 1.0f;
		emitter.DopplerScaler = 1.0f;
		emitter.InnerRadius = 1.0f;
		emitter.InnerRadiusAngle = 0.0f;

		X3DAUDIO_DISTANCE_CURVE_POINT points[] = {
				{0.0f, 1.0f},   // 距離0～1の間を最大音量
				{1.0f, 1.0f},
				{15.0f, 0.5f},  // 1～15は半分
				{35.0f, 0.0f}  // 15～35は半分から0の音量
		};

		X3DAUDIO_DISTANCE_CURVE curve = {
			points,
			_countof(points)
		};

		emitter.pVolumeCurve = &curve;

		FLOAT32 matrix[8];

		//DSP計算
		X3DAUDIO_DSP_SETTINGS dsp = {};
		dsp.SrcChannelCount = 1;
		dsp.DstChannelCount = 2; // ステレオ
		dsp.pMatrixCoefficients = matrix;

		X3DAudioCalculate(m_X3DAudioHandle, &listener, &emitter, X3DAUDIO_CALCULATE_MATRIX, &dsp);

		m_SourceVoice->SetOutputMatrix(m_MasteringVoice, 1, dsp.DstChannelCount, matrix);
	}

	XAUDIO2_VOICE_STATE state;
	m_SourceVoice->GetState(&state);
	//音声の再生が終了したか
	if (state.SamplesPlayed != 0 && state.BuffersQueued == 0)
	{
		//キャッシュする音声であれば再生を止める。不要であれば削除
		if (m_IsCache)
		{
			m_SourceVoice->Stop();
		}
		else
		{
			m_IsSoundEnd = true;
		}
	}
}

UINT CSound::Play()
{
	if (!m_SourceVoice)
	{
		return -1;
	}
	XAUDIO2_VOICE_STATE state;

	m_SourceVoice->GetState(&state);

	if (state.BuffersQueued == 0)
	{
		// バッファ設定
		XAUDIO2_BUFFER buffer;

		memset(&buffer, 0x00, sizeof(buffer));
		buffer.AudioBytes = m_AudioBytes;
		buffer.pAudioData = m_ByteAudioData;
		buffer.PlayBegin = 0;
		buffer.PlayLength = m_PlayLength;

		// ループ設定
		if (m_SoundData.IsLoop)
		{
			buffer.LoopBegin = 0;
			buffer.LoopLength = m_PlayLength;
			buffer.LoopCount = XAUDIO2_LOOP_INFINITE;
		}

		m_SourceVoice->SubmitSourceBuffer(&buffer, nullptr);

		// 再生
		m_SourceVoice->Start();
	}
	return m_Id;
}

void CSound::Replay()
{
	if (!m_SourceVoice)
	{
		return;
	}
	if (m_IsCache)
	{
		//音を0から再生
		XAUDIO2_BUFFER buffer;

		memset(&buffer, 0x00, sizeof(buffer));
		buffer.AudioBytes = m_AudioBytes;
		buffer.pAudioData = m_ByteAudioData;
		buffer.PlayBegin = 0;
		buffer.PlayLength = m_PlayLength;
		m_SourceVoice->SubmitSourceBuffer(&buffer, nullptr);
	}
	m_SourceVoice->Start();
}

void CSound::Stop()
{
	if (m_SourceVoice)
	{
		m_SourceVoice->Stop();
	}
}

void CSound::Play3DSound(const X3DAUDIO_HANDLE& X3DAudioHandle, const Vector3& PlayPosition)
{
	for (int i = 0; i < X3DAUDIO_HANDLE_BYTESIZE; i++)
	{
		m_X3DAudioHandle[i] = X3DAudioHandle[i];
	}
	m_3DSoundPosition = PlayPosition;
	m_Is3DSound = true;
}

void CSound::Update3DSoundPosition(const Vector3& Position)
{
	m_3DSoundPosition = Position;
}

ESoundType CSound::GetSoundType() const
{
	return m_SoundData.Type;
}
