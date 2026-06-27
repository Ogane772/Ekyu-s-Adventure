#include "stdafx.h"
#include "DemoPlayer.h"
#include "../../../D3D11TextureMediaSink//D3D11TextureMediaSink.h"
#include "../Renderer/Renderer.h"

CDemoPlayer::CDemoPlayer()
{
	HRESULT hr = S_OK;
	m_Work = ::CreateThreadpoolWork(CDemoPlayer::PresentSwapChainWork, this, nullptr);
	m_ReadyOrFailed = ::CreateEvent(nullptr, true, false, nullptr);
	// MediaFoundation のセットアップ
	MFStartup(MF_VERSION);

	// IMFDXGIDeviceManager の作成とD3Dデバイスの登録
	if (FAILED(hr = ::MFCreateDXGIDeviceManager(&m_DXGIDeviceManagerID, &m_DXGIDeviceManager)))
	{
		return;
	}
	m_DXGIDeviceManager->ResetDevice(CRenderer::GetDevice(), m_DXGIDeviceManagerID);
	// トポロジーワークキュー設定用ID。IUnknown になるなら、内容はなんでもいい。
	IMFMediaType* id = nullptr;
	MFCreateMediaType(&id);
	m_RegisterTopologyId = static_cast<IUnknown*>(id);

	// 初期化完了
	m_IsInitialized = true;
}

void CDemoPlayer::Dispose()
{
	m_IsInitialized = false;

	// 再生停止
	if (m_MediaSession)
	{
		m_MediaSession->Stop();
	}

	// 表示中なら終了を待つ。
	WaitForThreadpoolWorkCallbacks(m_Work, true);
	CloseThreadpoolWork(m_Work);
	
	// MediaFoundation の解放
	SafeRelease(m_RegisterTopologyId);
	SafeRelease(m_MediaSinkAttributes);
	SafeRelease(m_MediaSink);
	SafeRelease(m_MediaSource);
	SafeRelease(m_MediaSession);

	// DXGI, D3D11 の解放
	SafeRelease(m_DXGIDeviceManager);

	// MediaFoundation のシャットダウン
	MFShutdown();
}

HRESULT CDemoPlayer::Play(LPCTSTR szFile)
{
	HRESULT hr = S_OK;

	ResetEvent(m_ReadyOrFailed);

	// MediaSession を作成する。
	if (FAILED(hr = CreateMediaSession(&m_MediaSession)))
	{
		return hr;
	}

	// ファイルから MediaSource を作成する。
	if (FAILED(hr = CreateMediaSourceFromFile(szFile, &m_MediaSource)))
	{
		return hr;
	}

	if (!m_MediaSession)
	{
		return S_FALSE;
	}

	IMFTopology* topology = nullptr;
	do
	{
		// 部分トポロジーを作成。
		if (FAILED(hr = CreateTopology(&topology)))
		{
			break;
		}

		// MediaSession に登録。
		if (FAILED(hr = m_MediaSession->SetTopology(0, topology)))
		{
			break;
		}

	} while (false);

	SafeRelease(topology);

	if (FAILED(hr))
	{
		return hr;
	}

	// MediaSession が完全トポロジーの作成に成功（または失敗）したら MESessionTopologyStatus イベントが発出されるので、それまで待つ。
	WaitForSingleObject(m_ReadyOrFailed, 50000);

	if (FAILED(m_HrStatus))
	{
		return m_HrStatus;	// 作成失敗
	}

	// MediaSession の再生を開始。
	PROPVARIANT varStart;
	PropVariantInit(&varStart);
	varStart.vt = VT_I8;
	varStart.hVal.QuadPart = 0;
	m_MediaSession->Start(nullptr, &varStart);
	
	return S_OK;
}

void CDemoPlayer::UpdateAndPresent()
{
	if (!m_IsPresentNow )
	{
		// 描画処理
		Draw();

		// SwapChain 表示
		m_IsPresentNow = true;
		SubmitThreadpoolWork(m_Work);
	}
	else
	{
		m_IsPresentNow = false;
	}
}

void CDemoPlayer::Draw()
{
	if (!m_MediaSinkAttributes)
	{
		return;
	}

	HRESULT hr = S_OK;

	IMFSample* sample = nullptr;
	IMFMediaBuffer* mediaBuffer = nullptr;
	IMFDXGIBuffer* dXGIBuffer = nullptr;
	ID3D11Texture2D* texture2D = nullptr;
	m_SResView = nullptr;
	do
	{
		// 現在表示すべき IMFSample が TextureMediaSink の TMS_SAMPLE 属性に設定されているので、それを取得する。
		if (FAILED(hr = m_MediaSinkAttributes->GetUnknown(TMS_SAMPLE, IID_IMFSample, (void**)&sample)))
		{
			break;
		}
		if (!sample)
		{
			break;	// 未設定
		}

		// IMFSample からメディアバッファを取得。
		if (FAILED(hr = sample->GetBufferByIndex(0, &mediaBuffer)))
		{
			break;
		}

		// メディアバッファからDXGIバッファを取得。
		if (FAILED(hr = mediaBuffer->QueryInterface(IID_IMFDXGIBuffer, (void**)&dXGIBuffer)))
		{
			break;
		}

		// DXGIバッファから転送元 Texture2D を取得。
		if (FAILED(hr = dXGIBuffer->GetResource(IID_ID3D11Texture2D, (void**)&texture2D)))
		{
			break;
		}

		if (!texture2D)
		{
			break;
		}

		// IMFSampleからID3D11Texture2D を取得することが出来たためテクスチャ情報を取得する
		D3D11_TEXTURE2D_DESC texDesc;
		texture2D->GetDesc(&texDesc);

		D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc;
		ZeroMemory(&SRVDesc, sizeof(SRVDesc));
		SRVDesc.Format = texDesc.Format;
		SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		SRVDesc.Texture2D.MostDetailedMip = 0;
		SRVDesc.Texture2D.MipLevels = 1;
		ID3D11ShaderResourceView* pSResView = nullptr;
		hr = CRenderer::GetDevice()->CreateShaderResourceView(texture2D, nullptr, &pSResView);
		m_SResView = std::shared_ptr<ID3D11ShaderResourceView>(pSResView, [](ID3D11ShaderResourceView* p) { if (p) p->Release(); });
	
	} while (false);
	
	// GetUnknown で TMS_SAMPLE を得たとき、その IMFSample は TextureMediaSink から更新されないよう lock されている。
	// この lock を解除するために、TMS_SAMPLE 属性に何か（なんでもいい）を SetUnknown する。これで IMFSample が TextureMediaSink から更新可能になる。
	
	m_MediaSinkAttributes->SetUnknown(TMS_SAMPLE, nullptr);
	SafeRelease(texture2D);
	SafeRelease(dXGIBuffer);
	SafeRelease(mediaBuffer);
	SafeRelease(sample);
}

HRESULT CDemoPlayer::CreateMediaSession(IMFMediaSession** ppMediaSession)
{
	HRESULT hr = S_OK;

	IMFMediaSession* mediaSession = nullptr;
	do
	{
		// MediaSession を生成する。
		if (FAILED(hr = MFCreateMediaSession(nullptr, &mediaSession)))
		{
			break;
		}
		if (!mediaSession)
		{
			break;
		}
		// MediaSession からのイベント送信を開始する。
		if (FAILED(hr = mediaSession->BeginGetEvent(this, nullptr)))
		{
			break;
		}

		// 作成完了。
		(*ppMediaSession) = mediaSession;
		(*ppMediaSession)->AddRef();
		hr = S_OK;

	} while (false);

	SafeRelease(mediaSession);

	return hr;
}

HRESULT CDemoPlayer::CreateMediaSourceFromFile(LPCTSTR szFile, IMFMediaSource** ppMediaSource)
{
	HRESULT hr = S_OK;

	IMFSourceResolver* resolver = nullptr;
	IMFMediaSource* mediaSource = nullptr;
	do
	{
		// ソースリゾルバを作成する。
		if (FAILED(hr = ::MFCreateSourceResolver(&resolver )))
		{
			break;
		}
		if (!resolver )
		{
			break;
		}

		// ファイルパスからメディアソースを作成する。
		MF_OBJECT_TYPE type;
		if (FAILED(hr = resolver ->CreateObjectFromURL((LPCWSTR)szFile, MF_RESOLUTION_MEDIASOURCE, nullptr, &type, (IUnknown**)&mediaSource)))
		{
			break;
		}

		// 作成完了。
		(*ppMediaSource) = mediaSource;
		(*ppMediaSource)->AddRef();
		hr = S_OK;

	} while (false);

	SafeRelease(mediaSource);
	SafeRelease(resolver );

	return hr;
}

HRESULT CDemoPlayer::CreateTopology(IMFTopology** ppTopology)
{
	if (!m_MediaSource)
	{
		return S_FALSE;
	}
	HRESULT hr = S_OK;

	IMFTopology* topology = nullptr;
	IMFPresentationDescriptor* presentationDesc = nullptr;
	do
	{
		// 新しいトポロジーを作成する。
		if (FAILED(hr = ::MFCreateTopology(&topology)))
		{
			break;
		}

		// メディアソース用のプレゼン識別子を作成する。
		if(FAILED(hr = m_MediaSource->CreatePresentationDescriptor(&presentationDesc)))
		{
			break;
		}
		if (!presentationDesc)
		{
			break;
		}

		// プレゼン識別子から、メディアソースのストリームの数を取得する。
		DWORD dwDescCount;
		if (FAILED(hr = presentationDesc->GetStreamDescriptorCount(&dwDescCount)))
		{
			break;
		}

		// メディアソースの各ストリームについて、トポロジーノードを作成してトポロジーに追加する。
		for (DWORD i = 0; i < dwDescCount; i++)
		{
			if (FAILED(hr = AddTopologyNodes(topology, presentationDesc, i)))
			{
				break;
			}
		}
		if (FAILED(hr))
		{
			break;
		}

		// 作成完了。
		(*ppTopology) = topology;
		(*ppTopology)->AddRef();
		hr = S_OK;

	} while (false);

	SafeRelease(presentationDesc);
	SafeRelease(topology);

	return hr;
}

HRESULT CDemoPlayer::AddTopologyNodes(IMFTopology* topology, IMFPresentationDescriptor* presentationDesc, DWORD index)
{
	if (!presentationDesc)
	{
		return S_FALSE;
	}
	HRESULT hr = S_OK;

	BOOL isSelected = false;
	IMFStreamDescriptor* streamDesc = nullptr;
	IMFTopologyNode* sourceNode = nullptr;
	IMFTopologyNode* outputNode = nullptr;
	do
	{
		// 指定されたストリーム番号のストリーム記述子を取得する。
		if (FAILED(hr = presentationDesc->GetStreamDescriptorByIndex(index, &isSelected, &streamDesc)))
		{
			break;
		}

		if (isSelected)
		{
			//ストリームが選択されているなら、トポロジーに追加する。
			if (FAILED(hr = CreateSourceNode(presentationDesc, streamDesc, &sourceNode)))
			{
				break;
			}

			GUID majorType;
			if (FAILED(hr = CreateOutputNode(streamDesc, &outputNode, &majorType)))
			{
				break;
			}
			if (!sourceNode)
			{
				break;
			}

			if (majorType == MFMediaType_Audio)
			{
				sourceNode->SetString(MF_TOPONODE_WORKQUEUE_MMCSS_CLASS, ((LPCWSTR)"Audio"));
				sourceNode->SetUINT32(MF_TOPONODE_WORKQUEUE_ID, 1);
			}
			if (majorType == MFMediaType_Video)
			{
				sourceNode->SetString(MF_TOPONODE_WORKQUEUE_MMCSS_CLASS, ((LPCWSTR)"Playback"));
				sourceNode->SetUINT32(MF_TOPONODE_WORKQUEUE_ID, 2);
			}

			if (sourceNode && outputNode)
			{
				topology->AddNode(sourceNode);
				topology->AddNode(outputNode);

				sourceNode->ConnectOutput(0, outputNode, 0);
			}
		}

	} while (false);

	SafeRelease(outputNode);
	SafeRelease(sourceNode);
	SafeRelease(streamDesc);

	return hr;
}

HRESULT CDemoPlayer::CreateSourceNode(IMFPresentationDescriptor* presentationDesc, IMFStreamDescriptor* streamDesc, IMFTopologyNode** ppSourceNode)
{
	HRESULT hr = S_OK;

	IMFTopologyNode* sourceNode = nullptr;
	do
	{
		// ソースノードを作成する。
		if (FAILED(hr = MFCreateTopologyNode(MF_TOPOLOGY_SOURCESTREAM_NODE, &sourceNode)))
		{
			break;
		}

		if (!sourceNode)
		{
			break;
		}

		// ソースノードに３属性を設定する。
		if (FAILED(hr = sourceNode->SetUnknown(MF_TOPONODE_SOURCE, m_MediaSource)))
		{
			break;
		}
		if (FAILED(hr = sourceNode->SetUnknown(MF_TOPONODE_PRESENTATION_DESCRIPTOR, presentationDesc)))
		{
			break;
		}
		if (FAILED(hr = sourceNode->SetUnknown(MF_TOPONODE_STREAM_DESCRIPTOR, streamDesc)))
		{
			break;
		}
	
		// 作成完了。
		(*ppSourceNode) = sourceNode;
		(*ppSourceNode)->AddRef();
		hr = S_OK;

	} while (false);

	SafeRelease(sourceNode);

	return hr;
}

HRESULT CDemoPlayer::CreateOutputNode(IMFStreamDescriptor* streamDesc, IMFTopologyNode** ppOutputNode, GUID* pMajorType)
{
	if (!streamDesc)
	{
		return S_FALSE;
	}
	HRESULT hr = S_OK;

	IMFTopologyNode* outputNode = nullptr;
	IMFMediaTypeHandler* mediaTypeHandler = nullptr;
	do
	{
		// 出力ノードを作成。
		if (FAILED(hr = MFCreateTopologyNode(MF_TOPOLOGY_OUTPUT_NODE, &outputNode)))
		{
			break;
		}
		if (!outputNode)
		{
			break;
		}
		// メディアハンドラを取得。
		if (FAILED(hr = streamDesc->GetMediaTypeHandler(&mediaTypeHandler)))
		{
			break;
		}
		if (!mediaTypeHandler)
		{
			break;
		}

		// メジャータイプを取得。
		GUID majorType;
		if (FAILED(hr = mediaTypeHandler->GetMajorType(&majorType)))
		{
			break;
		}

		if (majorType == MFMediaType_Video)
		{
			if (!m_MediaSink)
			{
				// TextureMediaSink を作成。
				if (FAILED(hr = CreateD3D11TextureMediaSink(IID_IMFMediaSink, (void**)&m_MediaSink, m_DXGIDeviceManager, CRenderer::GetDevice())))
				{
					break;
				}
				if (!m_MediaSink)
				{
					break;
				}
				// IMFSample 受け取り用に IMFAttributes を取得しておく。
				if (FAILED(hr = m_MediaSink->QueryInterface(IID_IMFAttributes, (void**)&m_MediaSinkAttributes)))
				{
					break;
				}
			}
			if (!m_MediaSink)
			{
				break;
			}
			IMFStreamSink* pStreamSink = nullptr;
			do
			{
				// ストリームシンク #0 を取得。
				if (FAILED(hr = m_MediaSink->GetStreamSinkById(0, &pStreamSink)))
				{
					break;
				}
				// ストリームシンクを出力ノードに登録。
				if (FAILED(hr = outputNode->SetObject(pStreamSink)))
				{
					break;
				}

			} while (false);

			SafeRelease(streamDesc);
		}
		else if (majorType == MFMediaType_Audio)
		{
			IMFActivate* pActivate = nullptr;
			do
			{
				// 既定のオーディオレンダラのアクティベートを生成。
				if (FAILED(hr = MFCreateAudioRendererActivate(&pActivate)))
				{
					break;
				}

				// 出力ノードに登録。
				if (FAILED(hr = outputNode->SetObject(pActivate)))
				{
					break;
				}

			} while (false);

			SafeRelease(pActivate);
		}

		// 作成完了。
		(*ppOutputNode) = outputNode;
		(*ppOutputNode)->AddRef();
		hr = S_OK;

	} while (false);

	SafeRelease(mediaTypeHandler);
	SafeRelease(outputNode);

	return hr;
}

ULONG	CDemoPlayer::AddRef()
{
	return InterlockedIncrement(&m_RefCount);
}

HRESULT CDemoPlayer::QueryInterface(REFIID iid, __RPC__deref_out _Result_nullonfailure_ void** ppv)
{
	if (!ppv)
	{
		return E_POINTER;
	}

	if (iid == IID_IUnknown)
	{
		*ppv = static_cast<IUnknown*>(static_cast<IMFAsyncCallback*>(this));
	}
	else if (iid == __uuidof(IMFAsyncCallback))
	{
		*ppv = static_cast<IMFAsyncCallback*>(this);
	}
	else
	{
		*ppv = nullptr;
		return E_NOINTERFACE;
	}

	AddRef();

	return S_OK;
}

ULONG CDemoPlayer::Release()
{
	const ULONG uCount = InterlockedDecrement(&m_RefCount);

	if (uCount == 0)
	{
		delete this;
	}

	return uCount;
}

STDMETHODIMP CDemoPlayer::GetParameters(__RPC__out DWORD *pdwFlags, __RPC__out DWORD *pdwQueue)
{
	return E_NOTIMPL;
}

STDMETHODIMP CDemoPlayer::Invoke(__RPC__in_opt IMFAsyncResult *pAsyncResult)
{
	if (!m_IsInitialized || !m_MediaSession || !pAsyncResult)
	{
		return MF_E_SHUTDOWN;
	}

	HRESULT hr = S_OK;

	IUnknown* unknown = nullptr;
	if (SUCCEEDED(hr = pAsyncResult->GetState(&unknown)))
	{
		if (m_RegisterTopologyId == unknown)
		{
			OnEndRegisterTopologyWorkQueueWithMMCSS(pAsyncResult);
			return S_OK;
		}
		else
		{
			return E_INVALIDARG;
		}
	}
	else
	{
		IMFMediaEvent* mediaEvent = nullptr;
		MediaEventType eventType = 0;
		do
		{
			// MediaSession からのイベントを受信する。
			if (FAILED(hr = m_MediaSession->EndGetEvent(pAsyncResult, &mediaEvent)))
			{
				break;
			}
			if (!mediaEvent)
			{
				break;
			}
			if (FAILED(hr = mediaEvent->GetType(&eventType)))
			{
				break;
			}
			if (FAILED(hr = mediaEvent->GetStatus(&m_HrStatus)))
			{
				break;
			}

			// 結果が失敗なら終了。
			if (FAILED(m_HrStatus))
			{
				SetEvent(m_ReadyOrFailed);
				return m_HrStatus;
			}

			// イベントタイプ別に分岐。
			switch (eventType)
			{
			case MESessionTopologyStatus:

				// Status 取得。
				UINT32 topoStatus;
				if (FAILED(hr = mediaEvent->GetUINT32(MF_EVENT_TOPOLOGY_STATUS, &topoStatus)))
				{
					break;
				}

				switch (topoStatus)
				{
				case MF_TOPOSTATUS_READY:
					OnTopologyReady(mediaEvent);
					break;
				}
				break;

				//再生が終了したら呼ばれる
			case MEEndOfPresentation:
				OnPresentationEnded(mediaEvent);
				break;
			}

		} while (false);

		SafeRelease(mediaEvent);

		// 次の MediaSession イベントの受信を待つ。
		if (eventType != MESessionClosed)
		{
			hr = m_MediaSession->BeginGetEvent(this, nullptr);
		}

		return hr;
	}
}

void CDemoPlayer::OnTopologyReady(IMFMediaEvent* mediaEvent)
{
	if (!m_MediaSession)
	{
		return;
	}
	HRESULT hr;

	// トポロジーワークキューの MMCSS へのクラス割り当て（非同期処理）を開始する。

	IMFGetService* getService = nullptr;
	IMFWorkQueueServices* workQueueServices = nullptr;
	do
	{
		if (FAILED(hr = m_MediaSession->QueryInterface(IID_IMFGetService, (void**)&getService)))
		{
			break;
		}

		if (!getService)
		{
			break;
		}

		if (FAILED(hr = getService->GetService(MF_WORKQUEUE_SERVICES, IID_IMFWorkQueueServices, (void**)&workQueueServices)))
		{
			break;
		}

		if (!workQueueServices)
		{
			break;
		}

		if (FAILED(hr = workQueueServices->BeginRegisterTopologyWorkQueuesWithMMCSS(this, m_RegisterTopologyId)))
		{
			break;
		}

	} while (false);

	SafeRelease(workQueueServices);
	SafeRelease(getService);

	if (FAILED(hr))
	{
		m_HrStatus = hr;
		SetEvent(m_ReadyOrFailed);
	}
}

void CDemoPlayer::OnPresentationEnded(IMFMediaEvent* mediaEvent)
{
	if (!m_MediaSession)
	{
		return;
	}
	//再生が終了したら一から再生する
	PROPVARIANT varStart;
	PropVariantInit(&varStart);
	varStart.vt = VT_I8;
	varStart.hVal.QuadPart = 0;
	m_MediaSession->Start(nullptr, &varStart);
}

void CDemoPlayer::OnEndRegisterTopologyWorkQueueWithMMCSS(IMFAsyncResult* pAsyncResult)
{
	if (!m_MediaSession)
	{
		return;
	}
	HRESULT hr;

	// トポロジーワークキューの MMCSS へのクラス割り当て（非同期処理）を完了する。

	IMFGetService* getService = nullptr;
	IMFWorkQueueServices* workQueueServices = nullptr;
	do
	{
		if (FAILED(hr = m_MediaSession->QueryInterface(IID_IMFGetService, (void**)&getService)))
		{
			break;
		}
		if (!getService)
		{
			break;
		}
		if (FAILED(hr = getService->GetService(MF_WORKQUEUE_SERVICES, IID_IMFWorkQueueServices, (void**)&workQueueServices)))
		{
			break;
		}
		if (!workQueueServices)
		{
			break;
		}
		if (FAILED(hr = workQueueServices->EndRegisterTopologyWorkQueuesWithMMCSS(pAsyncResult)))
		{
			break;
		}

	} while (false);

	SafeRelease(workQueueServices);
	SafeRelease(getService);


	if (FAILED(hr))
	{
		m_HrStatus = hr;
	}
	else
	{
		m_HrStatus = S_OK;
	}

	SetEvent(m_ReadyOrFailed);	// イベント発火。
}

void CALLBACK CDemoPlayer::PresentSwapChainWork(PTP_CALLBACK_INSTANCE pInstance, LPVOID pvParam, PTP_WORK pWork)
{
	CDemoPlayer* demoPlayer = static_cast<CDemoPlayer*>(pvParam);
	if (demoPlayer)
	{
		demoPlayer->m_IsPresentNow = false;
	}
}

void CDemoPlayer::ReleaseSResView()
{
	if (m_SResView)
	{
		m_SResView->Release();
		m_SResView.reset();
	}	
}