#pragma once
#include <mfobjects.h>
#include <mfidl.h>
#include <memory>
#define _CRT_SECURE_NO_WARNINGS
#pragma warning(disable : 4005) 

struct ID3D11ShaderResourceView;

class CDemoPlayer : public IMFAsyncCallback
{
public:
	
	CDemoPlayer();
	HRESULT Play(LPCTSTR szFile);
	void Dispose();
	void UpdateAndPresent();
	std::weak_ptr<ID3D11ShaderResourceView> GetSResView() const { return m_SResView; }
	void ReleaseSResView();
	// IUnknown 実装
	STDMETHODIMP_(ULONG) AddRef();
	STDMETHODIMP QueryInterface(REFIID iid, __RPC__deref_out _Result_nullonfailure_ void** ppv);
	STDMETHODIMP_(ULONG) Release();

	// IMFAsyncCallback 実装
	HRESULT STDMETHODCALLTYPE GetParameters(__RPC__out DWORD *pdwFlags, __RPC__out DWORD *pdwQueue);
	HRESULT STDMETHODCALLTYPE Invoke(__RPC__in_opt IMFAsyncResult *pAsyncResult);

private:
	long m_RefCount = 0;
	bool m_IsInitialized = false;
	std::shared_ptr<ID3D11ShaderResourceView> m_SResView;

	IMFDXGIDeviceManager* m_DXGIDeviceManager = nullptr;
	UINT m_DXGIDeviceManagerID = 0;

	std::atomic_bool m_IsPresentNow = false;
	PTP_WORK m_Work = nullptr;
	IMFMediaSession* m_MediaSession = nullptr;
	IMFMediaSource* m_MediaSource = nullptr;
	IMFMediaSink* m_MediaSink = nullptr;
	IMFAttributes* m_MediaSinkAttributes = nullptr;
	IUnknown* m_RegisterTopologyId = nullptr;
	HANDLE m_ReadyOrFailed = nullptr;
	HRESULT m_HrStatus = S_OK;

	void Draw();

	HRESULT CreateMediaSession(IMFMediaSession** ppMediaSession);
	HRESULT CreateMediaSourceFromFile(LPCTSTR szFile, IMFMediaSource** ppMediaSource);
	HRESULT CreateTopology(IMFTopology** ppTopology);
	HRESULT AddTopologyNodes(IMFTopology* pTopology, IMFPresentationDescriptor* pPresentationDesc, DWORD index);
	HRESULT CreateSourceNode(IMFPresentationDescriptor* pPresentationDesc, IMFStreamDescriptor* pStreamDesc, IMFTopologyNode** ppSourceNode);
	HRESULT CreateOutputNode(IMFStreamDescriptor* pStreamDesc, IMFTopologyNode** ppOutputNode, GUID* pMajorType);

	void OnTopologyReady(IMFMediaEvent* pMediaEvent);
	void OnPresentationEnded(IMFMediaEvent* pMediaEvent);
	void OnEndRegisterTopologyWorkQueueWithMMCSS(IMFAsyncResult* pAsyncResult);

	static void CALLBACK PresentSwapChainWork(PTP_CALLBACK_INSTANCE pInstance, LPVOID pvParam, PTP_WORK pWork);
};