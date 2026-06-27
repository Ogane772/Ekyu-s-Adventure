#include "Renderer.h" 
#include <io.h>
#include "../Model/StaticMesh.h"
#include "../Model/SkeletalMesh.h"
#include "../Light/DirectionalLight.h"
#include "../Line/Line.h"
#include "../Text/Text.h"
#include "../Shadow/ShadowMap.h"
#include "../Fade/Fade.h"
#include "../System/SystemContext.h"
#define SAFE_RELEASE(x) if(x){x->Release(); x=nullptr;}
namespace
{
	//フォグを適用する距離。カメラからNEARとFAR間の距離でフォグが徐々にかかり、FAR以降の距離はフォグが常にかかる状態となる
	constexpr float FOG_NEAR = 18.0f;
	constexpr float FOG_FAR = 171.0f;
}

ID3D11Device*           CRenderer::m_D3DDevice = nullptr;
ID3D11DeviceContext*    CRenderer::m_ImmediateContext = nullptr;
IDXGISwapChain*         CRenderer::m_SwapChain = nullptr;

ID3D11VertexShader*     CRenderer::m_VertexShader = nullptr;
ID3D11PixelShader*      CRenderer::m_PixelShader = nullptr;
ID3D11VertexShader*     CRenderer::m_PolygonVertexShader = nullptr;
ID3D11PixelShader*      CRenderer::m_PolygonPixelShader = nullptr;
ID3D11VertexShader*     CRenderer::m_TitleVertexShader = nullptr;
ID3D11PixelShader*	    CRenderer::m_TitlePixelShader = nullptr;
ID3D11VertexShader*     CRenderer::m_2DVertexShader = nullptr;
ID3D11PixelShader*      CRenderer::m_2DPixelShader = nullptr;
ID3D11InputLayout*      CRenderer::m_VertexLayout = nullptr;
ID3D11Buffer*			CRenderer::m_WorldBuffer = nullptr;
ID3D11Buffer*			CRenderer::m_ViewBuffer = nullptr;
ID3D11Buffer*			CRenderer::m_ProjectionBuffer = nullptr;
ID3D11Buffer*			CRenderer::m_MaterialBuffer = nullptr;
ID3D11Buffer*			CRenderer::m_LightBuffer = nullptr;
ID3D11Buffer*			CRenderer::m_CameraPosBuffer = nullptr;
ID3D11Buffer*			CRenderer::m_FogBuffer = nullptr;
FOG_CONSTANT			CRenderer::m_FogConstant;
FOG_CONSTANT			CRenderer::m_DefaultFogConstant;
ID3D11DepthStencilState* CRenderer::m_DepthStateEnable = nullptr;
ID3D11DepthStencilState* CRenderer::m_DepthStateDisable = nullptr;
D3D11_BLEND_DESC CRenderer::m_BlendDesc;

ID3D11RenderTargetView* CRenderer::m_BackBuffer_TexRTV = nullptr;//バックバッファーのRTV
ID3D11Texture2D* CRenderer::m_BackBuffer_DSTex = nullptr;//バックバッファー用のDS
ID3D11DepthStencilView* CRenderer::m_BackBuffer_DSTexDSV = nullptr;//バックバッファー用のDSのDSV

ID3D11Texture2D* CRenderer::m_DepthMap_Tex = nullptr;//深度マップテクスチャー
ID3D11RenderTargetView* CRenderer::m_DepthMap_TexRTV = nullptr;//深度マップテクスチャーのRTV
ID3D11ShaderResourceView* CRenderer::m_DepthMap_TexSRV = nullptr;//深度マップテクスチャーのSRV
ID3D11Texture2D* CRenderer::m_DepthMap_DSTex = nullptr;//深度マップテクスチャー用DS
ID3D11DepthStencilView* CRenderer::m_DepthMap_DSTexDSV = nullptr;//深度マップテクスチャー用DSのDSV	
IDXGIOutput* CRenderer::m_DXGIOutput = nullptr;

void CRenderer::Init()
{
	D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;
	CSystemContext* system = CSystemContext::GetInstance();
	if (!system)
	{
		return;
	}
	HRESULT hr = S_OK;

	hr = D3D11CreateDevice(
		NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		0,
		NULL,
		0,
		D3D11_SDK_VERSION,
		&m_D3DDevice,
		&featureLevel,
		&m_ImmediateContext);
	if (FAILED(hr)) 
	{
		MessageBoxW(system->GetWindow(), L"D3D11CreateDevice", L"Err", MB_ICONSTOP);
	}

	if (!m_D3DDevice || !m_ImmediateContext)
	{
		return;
	}

	//使用可能なMSAAを取得
	DXGI_SAMPLE_DESC MSAA;
	for (int i = 0; i <= D3D11_MAX_MULTISAMPLE_SAMPLE_COUNT; i++) 
	{
		UINT Quality;
		if SUCCEEDED(m_D3DDevice->CheckMultisampleQualityLevels(DXGI_FORMAT_D24_UNORM_S8_UINT, i, &Quality)) 
		{
			if (0 < Quality) 
			{
				MSAA.Count = i;
				MSAA.Quality = Quality - 1;
			}
		}
	}

	 //IDXGIOutput の取得
	IDXGIDevice* pDXGIDevice = nullptr;
	IDXGIAdapter* pDXGIAdapter = nullptr;
	do
	{
		if (FAILED(hr = m_D3DDevice->QueryInterface(IID_IDXGIDevice, (void**)&pDXGIDevice)))
		{
			break;
		}
		if(!pDXGIDevice || pDXGIAdapter)
		{
			return;
		}

		if (FAILED(hr = pDXGIDevice->GetAdapter(&pDXGIAdapter)))
		{
			break;
		}

		if (FAILED(hr = pDXGIAdapter->EnumOutputs(0, &m_DXGIOutput)))
		{
			break;
		}

	} while (FALSE);

	const UINT defaultScreenWidth = static_cast<UINT>(system->GetDefaultScreenWidth());
	const UINT defaultScreenHeight = static_cast<UINT>(system->GetDefaultScreenHeight());

	//ファクトリー取得
	IDXGIFactory* hpDXGIFactory = NULL;
	pDXGIAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&hpDXGIFactory);
	if (!hpDXGIFactory) 
	{
		return;
	}
	SAFE_RELEASE(pDXGIAdapter);
	SAFE_RELEASE(pDXGIDevice);
	//スワップチェイン作成
	DXGI_SWAP_CHAIN_DESC sd = {};
	sd.BufferCount = 1;
	sd.BufferDesc.Width = defaultScreenWidth;
	sd.BufferDesc.Height = defaultScreenHeight;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = system->GetWindow();
	sd.SampleDesc.Count = MSAA.Count;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	if (FAILED(hpDXGIFactory->CreateSwapChain(m_D3DDevice, &sd, &m_SwapChain)))
	{
		MessageBoxW(system->GetWindow(), L"CreateSwapChain", L"Err", MB_ICONSTOP);
	}

	// マルチスレッドモードを ON に設定する。DXVAを使う場合は必須。
	ID3D10Multithread* pD3D10Multithread;
	do
	{
		if (FAILED(hr = m_D3DDevice->QueryInterface(IID_ID3D10Multithread, (void**)&pD3D10Multithread)))
		{
			break;
		}
		if (pD3D10Multithread)
		{
			pD3D10Multithread->SetMultithreadProtected(TRUE);
		}

	} while (FALSE);
	SAFE_RELEASE(pD3D10Multithread);

	if(!m_SwapChain)
	{
		return;
	}
	//バックバッファーテクスチャーを取得（既にあるので作成ではない）
	ID3D11Texture2D *pBackBuffer_Tex;
	m_SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer_Tex);
	//そのテクスチャーに対しレンダーターゲットビュー(RTV)を作成
	m_D3DDevice->CreateRenderTargetView(pBackBuffer_Tex, nullptr, &m_BackBuffer_TexRTV);
	SAFE_RELEASE(pBackBuffer_Tex);
	
	////デプスステンシルビュー用のテクスチャーを作成
	D3D11_TEXTURE2D_DESC depthDesc = {};
	depthDesc.Width = defaultScreenWidth;
	depthDesc.Height = defaultScreenHeight;
	depthDesc.MipLevels = 1;
	depthDesc.ArraySize = 1;
	depthDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthDesc.SampleDesc.Count = MSAA.Count;
	depthDesc.SampleDesc.Quality = 0;
	depthDesc.Usage = D3D11_USAGE_DEFAULT;
	depthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthDesc.CPUAccessFlags = 0;
	depthDesc.MiscFlags = 0;

	m_D3DDevice->CreateTexture2D(&depthDesc, nullptr, &m_BackBuffer_DSTex);
	//MSAAに対応するためマルチサンプリングである事を明示する
	{
		D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
		dsvDesc.Format = depthDesc.Format;
		//明示
		dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
		dsvDesc.Flags = 0;
		dsvDesc.Texture2D.MipSlice = 0;
		m_D3DDevice->CreateDepthStencilView(m_BackBuffer_DSTex, &dsvDesc, &m_BackBuffer_DSTexDSV);
		m_ImmediateContext->OMSetRenderTargets(1, &m_BackBuffer_TexRTV, m_BackBuffer_DSTexDSV);
	}

	//深度マップテクスチャーを作成
	D3D11_TEXTURE2D_DESC tdesc;
	ZeroMemory(&tdesc, sizeof(D3D11_TEXTURE2D_DESC));
	tdesc.Width = DEPTHTEX_WIDTH;
	tdesc.Height = DEPTHTEX_HEIGHT;
	tdesc.MipLevels = 1;
	tdesc.ArraySize = 1;
	tdesc.MiscFlags = 0;
	tdesc.Format = DXGI_FORMAT_R32_FLOAT;
	tdesc.SampleDesc.Count = 1;
	tdesc.SampleDesc.Quality = 0;
	tdesc.Usage = D3D11_USAGE_DEFAULT;
	tdesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	tdesc.CPUAccessFlags = 0;

	m_D3DDevice->CreateTexture2D(&tdesc, nullptr, &m_DepthMap_Tex);

	//深度マップテクスチャー用　レンダーターゲットビュー作成
	D3D11_RENDER_TARGET_VIEW_DESC DescRT;
	DescRT.Format = tdesc.Format;
	DescRT.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	DescRT.Texture2D.MipSlice = 0;

	m_D3DDevice->CreateRenderTargetView(m_DepthMap_Tex, &DescRT, &m_DepthMap_TexRTV);

	//深度マップテクスチャをレンダーターゲットにする際のデプスステンシルビュー用のテクスチャーを作成
	depthDesc.Width = DEPTHTEX_WIDTH;
	depthDesc.Height = DEPTHTEX_HEIGHT;
	depthDesc.MipLevels = 1;
	depthDesc.ArraySize = 1;
	depthDesc.Format = DXGI_FORMAT_D32_FLOAT;
	depthDesc.SampleDesc.Count = 1;
	depthDesc.SampleDesc.Quality = 0;
	depthDesc.Usage = D3D11_USAGE_DEFAULT;
	depthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthDesc.CPUAccessFlags = 0;
	depthDesc.MiscFlags = 0;

	m_D3DDevice->CreateTexture2D(&depthDesc, nullptr, &m_DepthMap_DSTex);

	//そのテクスチャーに対しデプスステンシルビュー(DSV)を作成
	m_D3DDevice->CreateDepthStencilView(m_DepthMap_DSTex, nullptr, &m_DepthMap_DSTexDSV);

	//深度マップテクスチャ用　シェーダーリソースビュー(SRV)作成	
	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc;
	ZeroMemory(&SRVDesc, sizeof(SRVDesc));
	SRVDesc.Format = tdesc.Format;
	SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	SRVDesc.Texture2D.MipLevels = 1;

	m_D3DDevice->CreateShaderResourceView(m_DepthMap_Tex, &SRVDesc, &m_DepthMap_TexSRV);

	// ビューポート設定
	D3D11_VIEWPORT vp;
	vp.Width = system->GetDefaultScreenWidth();
	vp.Height = system->GetDefaultScreenHeight();
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	m_ImmediateContext->RSSetViewports(1, &vp);

	// ラスタライザステート設定
	D3D11_RASTERIZER_DESC rd; 
	ZeroMemory( &rd, sizeof(rd));
	
	rd.FillMode = D3D11_FILL_SOLID; 
	//rd.FillMode = D3D11_FILL_WIREFRAME;
	rd.CullMode = D3D11_CULL_BACK; 
	rd.DepthClipEnable = TRUE; 
	rd.MultisampleEnable = FALSE; 

	ID3D11RasterizerState *rs;
	m_D3DDevice->CreateRasterizerState(&rd, &rs);

	m_ImmediateContext->RSSetState(rs);

	// ブレンドステート設定
	ZeroMemory(&m_BlendDesc, sizeof(m_BlendDesc));
	//ここをfalseでアルファ値0.5以下でも描画される
	m_BlendDesc.AlphaToCoverageEnable = FALSE;
	m_BlendDesc.IndependentBlendEnable = FALSE;
	m_BlendDesc.RenderTarget[0].BlendEnable = TRUE;
	m_BlendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	m_BlendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	m_BlendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	m_BlendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	m_BlendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	m_BlendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	m_BlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	ID3D11BlendState* blendState = nullptr;
	m_D3DDevice->CreateBlendState(&m_BlendDesc, &blendState);
	m_ImmediateContext->OMSetBlendState(blendState, blendFactor, 0xffffffff);
	
	// 深度ステンシルステート設定
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	ZeroMemory( &depthStencilDesc, sizeof(depthStencilDesc));
	depthStencilDesc.DepthEnable = TRUE;
	depthStencilDesc.DepthWriteMask	= D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
	depthStencilDesc.StencilEnable = FALSE;

	m_D3DDevice->CreateDepthStencilState(&depthStencilDesc, &m_DepthStateEnable);//深度有効ステート

	depthStencilDesc.DepthWriteMask	= D3D11_DEPTH_WRITE_MASK_ZERO;
	m_D3DDevice->CreateDepthStencilState(&depthStencilDesc, &m_DepthStateDisable);//深度無効ステート

	m_ImmediateContext->OMSetDepthStencilState(m_DepthStateEnable, NULL);

	// サンプラーステート設定
	D3D11_SAMPLER_DESC samplerDesc;
	ZeroMemory( &samplerDesc, sizeof( samplerDesc ) );
	samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0;
	samplerDesc.MaxAnisotropy = 16;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	ID3D11SamplerState* samplerState = nullptr;
	m_D3DDevice->CreateSamplerState(&samplerDesc, &samplerState);

	m_ImmediateContext->PSSetSamplers(0, 1, &samplerState );

	// 頂点シェーダ生成
	std::vector <std::pair<char const*, ID3D11VertexShader**>> vertexShaderFiles =
	{
		{ "shader/vertexShader.cso", &m_VertexShader },
		{ "shader/PolygonVS.cso", &m_PolygonVertexShader },
		{ "shader/TitleVS.cso", &m_TitleVertexShader },
		{ "shader/2DShaderVS.cso", &m_2DVertexShader },
	};
	for (int i = 0; i < vertexShaderFiles.size(); ++i)
	{
		FILE* file;
		long int size;

		std::pair<char const*, ID3D11VertexShader**>& pair = vertexShaderFiles[i];
		fopen_s(&file, pair.first, "rb");
		size = _filelength(_fileno(file));
		unsigned char* buffer = new unsigned char[size];
		fread(buffer, size, 1, file);
		fclose(file);

		m_D3DDevice->CreateVertexShader(buffer, size, nullptr, pair.second);

		if (i == 0)
		{
			D3D11_INPUT_ELEMENT_DESC layout[] =
			{
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 4 * 3, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 4 * 6, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 4 * 10, D3D11_INPUT_PER_VERTEX_DATA, 0 }
			};
			const UINT numElements = ARRAYSIZE(layout);

			m_D3DDevice->CreateInputLayout(layout,
				numElements,
				buffer,
				size,
				&m_VertexLayout);
		}
		delete[] buffer;
	}

	// ピクセルシェーダ生成
	std::vector <std::pair<char const*, ID3D11PixelShader**>> pixelShaderFiles =
	{
		{ "shader/pixelShader.cso", &m_PixelShader },
		{ "shader/PolygonPS.cso", &m_PolygonPixelShader },
		{ "shader/TitlePS.cso", &m_TitlePixelShader },
		{ "shader/2DShaderPS.cso", &m_2DPixelShader },
	};
	for (int i = 0; i < pixelShaderFiles.size(); ++i)
	{
		FILE* file;
		long int size;

		std::pair<char const*, ID3D11PixelShader**>& pair = pixelShaderFiles[i];
		fopen_s(&file, pair.first, "rb");
		size = _filelength(_fileno(file));
		unsigned char* buffer = new unsigned char[size];
		fread(buffer, size, 1, file);
		fclose(file);

		m_D3DDevice->CreatePixelShader(buffer, size, nullptr, pair.second);

		delete[] buffer;
	}

	// 定数バッファ生成
	D3D11_BUFFER_DESC hBufferDesc;
	hBufferDesc.ByteWidth = sizeof(XMMATRIX);
	hBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	hBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	hBufferDesc.CPUAccessFlags = 0;
	hBufferDesc.MiscFlags = 0;
	hBufferDesc.StructureByteStride = sizeof(float);

	m_D3DDevice->CreateBuffer( &hBufferDesc, nullptr, &m_WorldBuffer );
	m_ImmediateContext->VSSetConstantBuffers(0, 1, &m_WorldBuffer);

	m_D3DDevice->CreateBuffer( &hBufferDesc, nullptr, &m_ViewBuffer );
	m_ImmediateContext->VSSetConstantBuffers(1, 1, &m_ViewBuffer );

	m_D3DDevice->CreateBuffer( &hBufferDesc, nullptr, &m_ProjectionBuffer );
	m_ImmediateContext->VSSetConstantBuffers(2, 1, &m_ProjectionBuffer );


	hBufferDesc.ByteWidth = sizeof(MATERIAL);

	m_D3DDevice->CreateBuffer( &hBufferDesc, nullptr, &m_MaterialBuffer );
	m_ImmediateContext->VSSetConstantBuffers(3, 1, &m_MaterialBuffer );

	hBufferDesc.ByteWidth = sizeof(LIGHT);

	m_D3DDevice->CreateBuffer(&hBufferDesc, nullptr, &m_LightBuffer);
	m_ImmediateContext->VSSetConstantBuffers(4, 1, &m_LightBuffer );

	hBufferDesc.ByteWidth = sizeof(XMFLOAT4);

	m_D3DDevice->CreateBuffer(&hBufferDesc, nullptr, &m_CameraPosBuffer);
	m_ImmediateContext->VSSetConstantBuffers(5, 1, &m_CameraPosBuffer);
	
	hBufferDesc.ByteWidth = sizeof(FOG_CONSTANT);

	m_D3DDevice->CreateBuffer(&hBufferDesc, nullptr, &m_FogBuffer);
	m_ImmediateContext->PSSetConstantBuffers(2, 1, &m_FogBuffer);
	
	// 入力レイアウト設定
	m_ImmediateContext->IASetInputLayout( m_VertexLayout );

	// シェーダ設定
	m_ImmediateContext->VSSetShader(m_VertexShader, nullptr, 0 );
	m_ImmediateContext->PSSetShader(m_PixelShader, nullptr, 0 );

	// マテリアル初期化
	MATERIAL material;
	ZeroMemory( &material, sizeof(material) );
	material.Diffuse = COLOR( 1.0f, 1.0f, 1.0f, 1.0f );
	material.Ambient = COLOR( 1.0f, 1.0f, 1.0f, 1.0f );
	SetMaterial( material );
	//ディレクショナルライト作成
	CDirectionalLight::GetInstance();
	//アニメーション用シェーダー作成
	CSkeletalMesh::ShaderInit();
	//ライン描画シェーダー作成
	CLine::ShaderInit();
	//文字列レンダリングの初期化	
	CText::Init(3.0f, XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f));
	CShadowMap::Init();
	CStaticMesh::InitModelData();
	// フェードクラス初期化
	CSingleton<CFade>::GetInstance();
	m_FogConstant.FogParam.x = 0.0f;
	m_FogConstant.FogParam.y = FOG_NEAR;
	m_FogConstant.FogParam.z = FOG_FAR;
	m_DefaultFogConstant = m_FogConstant;
}

void CRenderer::UnInit()
{
	// オブジェクト解放
	CSkeletalMesh::ShaderUnInit();
	CText::UnInit();
	CLine::ShaderUnInit();
	CShadowMap::UnInit();
	CStaticMesh::UnInitModelData();

	SAFE_RELEASE(m_WorldBuffer);
	SAFE_RELEASE(m_ViewBuffer);
	SAFE_RELEASE(m_ProjectionBuffer);

	SAFE_RELEASE(m_MaterialBuffer);
	SAFE_RELEASE(m_CameraPosBuffer);
	SAFE_RELEASE(m_FogBuffer);
	SAFE_RELEASE(m_VertexLayout);
	SAFE_RELEASE(m_VertexShader);
	SAFE_RELEASE(m_PixelShader);

	if (m_ImmediateContext)
	{
		m_ImmediateContext->ClearState();
	}
	SAFE_RELEASE(m_SwapChain);
	SAFE_RELEASE(m_ImmediateContext);
	SAFE_RELEASE(m_D3DDevice);

	SAFE_RELEASE(m_PolygonVertexShader);
	SAFE_RELEASE(m_PolygonPixelShader);
	SAFE_RELEASE(m_TitlePixelShader);
	SAFE_RELEASE(m_TitleVertexShader);
	SAFE_RELEASE(m_2DVertexShader);
	SAFE_RELEASE(m_2DPixelShader);
	SAFE_RELEASE(m_BackBuffer_TexRTV);
	SAFE_RELEASE(m_BackBuffer_DSTex);
	SAFE_RELEASE(m_BackBuffer_DSTexDSV);
	
	SAFE_RELEASE(m_DepthMap_Tex);
	SAFE_RELEASE(m_DepthMap_TexRTV);
	SAFE_RELEASE(m_DepthMap_TexSRV);
	SAFE_RELEASE(m_DepthMap_DSTex);
	SAFE_RELEASE(m_DepthMap_DSTexDSV);
}

void CRenderer::Begin()
{
	if (!m_ImmediateContext)
	{
		return;
	}
	// バックバッファクリア
	const float ClearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	m_ImmediateContext->ClearRenderTargetView(m_BackBuffer_TexRTV, ClearColor );
	m_ImmediateContext->ClearDepthStencilView(m_BackBuffer_DSTexDSV, D3D11_CLEAR_DEPTH, 1.0f, 0);

}

void CRenderer::End()
{
	if (m_SwapChain)
	{
		m_SwapChain->Present(1, 0);
	}
}

void CRenderer::SetDepthEnable(const bool Enable)
{
	if (!m_ImmediateContext)
	{
		return;
	}

	if (Enable)
	{
		m_ImmediateContext->OMSetDepthStencilState(m_DepthStateEnable, NULL);
	}
	else
	{
		m_ImmediateContext->OMSetDepthStencilState(m_DepthStateDisable, NULL);
	}
}

void CRenderer::SetWorldViewProjection2D()
{
	CSystemContext* system = CSystemContext::GetInstance();
	if (!system)
	{
		return;
	}
	if (!m_ImmediateContext)
	{
		return;
	}
	XMMATRIX world;
	world = XMMatrixIdentity();
	XMMATRIX transpose = XMMatrixTranspose(world);
	m_ImmediateContext->UpdateSubresource(m_WorldBuffer, 0, nullptr, &transpose, 0, 0);

	XMMATRIX view;
	view = XMMatrixIdentity();
	transpose = XMMatrixTranspose(view);
	m_ImmediateContext->UpdateSubresource(m_ViewBuffer, 0, nullptr, &transpose, 0, 0);

	float w = system->GetCurrentScreenWidth();
	float h = system->GetCurrentScreenHeight();
	if (w <= 0)
	{
		w = system->GetDefaultScreenWidth();
	}
	if (h <= 0)
	{
		h = system->GetDefaultScreenHeight();
	}

	const XMMATRIX projection = XMMatrixOrthographicOffCenterLH(0.0f, w, h, 0.0f, 0.0f, 1.0f);
	transpose = XMMatrixTranspose(projection);
	m_ImmediateContext->UpdateSubresource(m_ProjectionBuffer, 0, nullptr, &transpose, 0, 0);
}


void CRenderer::SetWorldMatrix(const XMMATRIX& WorldMatrix)
{
	if (!m_ImmediateContext)
	{
		return;
	}
	const XMMATRIX transpose = XMMatrixTranspose(WorldMatrix);
	m_ImmediateContext->UpdateSubresource(m_WorldBuffer, 0, nullptr, &transpose, 0, 0);
}

void CRenderer::SetViewMatrix(const XMMATRIX& ViewMatrix)
{
	if (!m_ImmediateContext)
	{
		return;
	}
	const XMMATRIX transpose = XMMatrixTranspose(ViewMatrix);
	m_ImmediateContext->UpdateSubresource(m_ViewBuffer, 0, nullptr, &transpose, 0, 0);
}

void CRenderer::SetProjectionMatrix(const XMMATRIX& ProjectionMatrix)
{
	if (!m_ImmediateContext)
	{
		return;
	}
	const XMMATRIX transpose = XMMatrixTranspose(ProjectionMatrix);
	m_ImmediateContext->UpdateSubresource(m_ProjectionBuffer, 0, nullptr, &transpose, 0, 0);
}

void CRenderer::SetMaterial(const MATERIAL& Material )
{
	if (!m_ImmediateContext)
	{
		return;
	}
	m_ImmediateContext->UpdateSubresource(m_MaterialBuffer, 0, nullptr, &Material, 0, 0);
}

void CRenderer::SetLight(const LIGHT& Light)
{
	if (!m_ImmediateContext)
	{
		return;
	}
	m_ImmediateContext->UpdateSubresource(m_LightBuffer, 0, nullptr, &Light, 0, 0);
}

void CRenderer::SetCameraPos(const XMFLOAT4& CameraPos)
{
	if (!m_ImmediateContext)
	{
		return;
	}
	m_ImmediateContext->UpdateSubresource(m_CameraPosBuffer, 0, nullptr, &CameraPos, 0, 0);
}


void CRenderer::SetVertexBuffers(ID3D11Buffer* VertexBuffer)
{
	if (!m_ImmediateContext)
	{
		return;
	}
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	ID3D11Buffer* vb[1] = { VertexBuffer };
	m_ImmediateContext->IASetVertexBuffers(0, 1, vb, &stride, &offset);
}


void CRenderer::SetIndexBuffer(ID3D11Buffer* IndexBuffer)
{
	if (!m_ImmediateContext)
	{
		return;
	}
	m_ImmediateContext->IASetIndexBuffer(IndexBuffer, DXGI_FORMAT_R16_UINT, 0);
}

void CRenderer::SetTexture(std::weak_ptr<ID3D11ShaderResourceView>& Texture)
{
	ID3D11ShaderResourceView* texture = Texture.lock().get();
	if (!m_ImmediateContext || !texture)
	{
		return;
	}
	ID3D11ShaderResourceView* srv[1] = { texture };
	m_ImmediateContext->PSSetShaderResources(0, 1, srv);
}

void CRenderer::SetTextureIndex(const int Index, std::weak_ptr<ID3D11ShaderResourceView>& Texture)
{
	ID3D11ShaderResourceView* texture = Texture.lock().get();
	if (!m_ImmediateContext || !texture)
	{
		return;
	}
	ID3D11ShaderResourceView* srv[1] = { texture };
	m_ImmediateContext->PSSetShaderResources(Index, 1, srv);
}

void CRenderer::DrawIndexed(const UINT IndexCount, const UINT StartIndexLocation, const int BaseVertexLocation)
{
	if (!m_ImmediateContext)
	{
		return;
	}
	m_ImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_ImmediateContext->DrawIndexed(IndexCount, StartIndexLocation, BaseVertexLocation);
}

void CRenderer::SetWireframeMode()
{
	if (!m_D3DDevice || !m_ImmediateContext)
	{
		return;
	}
	// ラスタライザステート設定
	D3D11_RASTERIZER_DESC rd;
	ZeroMemory(&rd, sizeof(rd));

	rd.FillMode = D3D11_FILL_WIREFRAME;
	rd.CullMode = D3D11_CULL_BACK;
	rd.DepthClipEnable = TRUE;
	rd.MultisampleEnable = FALSE;

	ID3D11RasterizerState *rs;
	m_D3DDevice->CreateRasterizerState(&rd, &rs);

	m_ImmediateContext->RSSetState(rs);
}

void CRenderer::SetSolidMode()
{
	if (!m_D3DDevice || !m_ImmediateContext)
	{
		return;
	}
	// ラスタライザステート設定
	D3D11_RASTERIZER_DESC rd;
	ZeroMemory(&rd, sizeof(rd));

	rd.FillMode = D3D11_FILL_SOLID;
	rd.CullMode = D3D11_CULL_BACK;
	rd.DepthClipEnable = TRUE;
	rd.MultisampleEnable = FALSE;

	ID3D11RasterizerState *rs;
	m_D3DDevice->CreateRasterizerState(&rd, &rs);

	m_ImmediateContext->RSSetState(rs);
}

void CRenderer::CleanupRenderTarget()
{
	// GPU にバインドされている RTV/DSV を解除
	if (m_ImmediateContext)
	{
		ID3D11RenderTargetView* nullRTV[1] = { nullptr };
		m_ImmediateContext->OMSetRenderTargets(1, nullRTV, nullptr);

		// ピクセルシェーダ・頂点シェーダに残るSRVを16スロット分解除する）
		ID3D11ShaderResourceView* nullSRV[16] = { nullptr };
		m_ImmediateContext->PSSetShaderResources(0, 16, nullSRV);
		m_ImmediateContext->VSSetShaderResources(0, 16, nullSRV);
	}

	// 既存のバックバッファ関連リソースを解放
	SAFE_RELEASE(m_BackBuffer_TexRTV);
	SAFE_RELEASE(m_BackBuffer_DSTexDSV);
	SAFE_RELEASE(m_BackBuffer_DSTex);
}

void CRenderer::ResizeBuffers(const LPARAM& LParam)
{
	const UINT newW = (UINT)LOWORD(LParam);
	const UINT newH = (UINT)HIWORD(LParam);
	if (newW == 0 || newH == 0 || !m_SwapChain)
	{
		return;
	}

	m_SwapChain->ResizeBuffers(0, newW, newH, DXGI_FORMAT_UNKNOWN, 0);
}

void CRenderer::CreateRenderTarget()
{
	CSystemContext* system = CSystemContext::GetInstance();
	if (!system || !m_SwapChain)
	{
		return;
	}
	// バックバッファ取得 -> RTV 作成
	ID3D11Texture2D* backBuffer = nullptr;
	if (FAILED(m_SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBuffer)))
	{
		MessageBoxW(system ->GetWindow(), L"GetBuffer failed", L"Err", MB_ICONSTOP);
		return;
	}
	if (!backBuffer || !m_D3DDevice)
	{
		return;
	}
	// バックバッファの記述を取得（MSAA の SampleDesc を取得するため）
	D3D11_TEXTURE2D_DESC bbDesc;
	backBuffer->GetDesc(&bbDesc);

	if (FAILED(m_D3DDevice->CreateRenderTargetView(backBuffer, nullptr, &m_BackBuffer_TexRTV)))
	{
		backBuffer->Release();
		MessageBoxW(system->GetWindow(), L"CreateRenderTargetView failed", L"Err", MB_ICONSTOP);
		return;
	}


	backBuffer->Release();

	// バックバッファ用の深度テクスチャ（DSV）を作成する。SampleDesc はバックバッファに合わせること。
	D3D11_TEXTURE2D_DESC depthDesc = {};
	depthDesc.Width = static_cast<UINT>(system->GetCurrentScreenWidth());
	depthDesc.Height = static_cast<UINT>(system->GetCurrentScreenHeight());
	depthDesc.MipLevels = 1;
	depthDesc.ArraySize = 1;
	depthDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthDesc.SampleDesc = bbDesc.SampleDesc; // MSAA に合わせる
	depthDesc.Usage = D3D11_USAGE_DEFAULT;
	depthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthDesc.CPUAccessFlags = 0;
	depthDesc.MiscFlags = 0;

	if (FAILED(m_D3DDevice->CreateTexture2D(&depthDesc, nullptr, &m_BackBuffer_DSTex)))
	{
		MessageBoxW(system->GetWindow(), L"CreateTexture2D(depth) failed", L"Err", MB_ICONSTOP);
		return;
	}

	if (FAILED(m_D3DDevice->CreateDepthStencilView(m_BackBuffer_DSTex, nullptr, &m_BackBuffer_DSTexDSV)))
	{
		MessageBoxW(system->GetWindow(), L"CreateDepthStencilView failed", L"Err", MB_ICONSTOP);
		return;
	}

	if (!m_ImmediateContext)
	{
		return;
	}

	// バックバッファの RTV と DSV をバインド
	m_ImmediateContext->OMSetRenderTargets(1, &m_BackBuffer_TexRTV, m_BackBuffer_DSTexDSV);

	// ビューポート更新
	D3D11_VIEWPORT vp;
	vp.Width = system->GetCurrentScreenWidth();
	vp.Height = system->GetCurrentScreenHeight();
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0.0f;
	vp.TopLeftY = 0.0f;
	m_ImmediateContext->RSSetViewports(1, &vp);
}

void CRenderer::SetShader()
{
	if (!m_ImmediateContext)
	{
		return;
	}
	m_ImmediateContext->VSSetConstantBuffers(0, 1, &m_WorldBuffer);

	m_ImmediateContext->VSSetConstantBuffers(1, 1, &m_ViewBuffer);

	m_ImmediateContext->VSSetConstantBuffers(2, 1, &m_ProjectionBuffer);

	m_ImmediateContext->VSSetConstantBuffers(3, 1, &m_MaterialBuffer);

	m_ImmediateContext->VSSetConstantBuffers(4, 1, &m_LightBuffer);

	// 入力レイアウト設定
	m_ImmediateContext->IASetInputLayout(m_VertexLayout);
	// シェーダ設定
	m_ImmediateContext->VSSetShader(m_VertexShader, nullptr, 0);
	m_ImmediateContext->PSSetShader(m_PixelShader, nullptr, 0);
}

void CRenderer::SetPolygonShader()
{
	if (!m_ImmediateContext)
	{
		return;
	}
	m_ImmediateContext->VSSetConstantBuffers(0, 1, &m_WorldBuffer);

	m_ImmediateContext->VSSetConstantBuffers(1, 1, &m_ViewBuffer);

	m_ImmediateContext->VSSetConstantBuffers(2, 1, &m_ProjectionBuffer);

	m_ImmediateContext->VSSetConstantBuffers(3, 1, &m_MaterialBuffer);

	m_ImmediateContext->VSSetConstantBuffers(4, 1, &m_LightBuffer);

	m_ImmediateContext->VSSetConstantBuffers(5, 1, &m_CameraPosBuffer);

	m_ImmediateContext->UpdateSubresource(m_FogBuffer, 0, nullptr, &m_FogConstant, 0, 0);
	m_ImmediateContext->PSSetConstantBuffers(2, 1, &m_FogBuffer);

	// 入力レイアウト設定
	m_ImmediateContext->IASetInputLayout(m_VertexLayout);
	// シェーダ設定
	m_ImmediateContext->VSSetShader(m_PolygonVertexShader, nullptr, 0);
	m_ImmediateContext->PSSetShader(m_PolygonPixelShader, nullptr, 0);
}

void CRenderer::Set2DShader()
{
	if (!m_ImmediateContext)
	{
		return;
	}
	m_ImmediateContext->VSSetConstantBuffers(0, 1, &m_WorldBuffer);

	m_ImmediateContext->VSSetConstantBuffers(1, 1, &m_ViewBuffer);

	m_ImmediateContext->VSSetConstantBuffers(2, 1, &m_ProjectionBuffer);

	m_ImmediateContext->VSSetConstantBuffers(3, 1, &m_MaterialBuffer);

	m_ImmediateContext->VSSetConstantBuffers(4, 1, &m_LightBuffer);

	// 入力レイアウト設定
	m_ImmediateContext->IASetInputLayout(m_VertexLayout);
	// シェーダ設定
	m_ImmediateContext->VSSetShader(m_2DVertexShader, nullptr, 0);
	m_ImmediateContext->PSSetShader(m_2DPixelShader, nullptr, 0);
}

void CRenderer::SetTitleShader()
{
	if (!m_ImmediateContext)
	{
		return;
	}
	m_ImmediateContext->VSSetConstantBuffers(0, 1, &m_WorldBuffer);

	m_ImmediateContext->VSSetConstantBuffers(1, 1, &m_ViewBuffer);

	m_ImmediateContext->VSSetConstantBuffers(2, 1, &m_ProjectionBuffer);

	m_ImmediateContext->VSSetConstantBuffers(3, 1, &m_MaterialBuffer);

	m_ImmediateContext->VSSetConstantBuffers(4, 1, &m_LightBuffer);

	// 入力レイアウト設定
	m_ImmediateContext->IASetInputLayout(m_VertexLayout);
	// シェーダ設定
	m_ImmediateContext->VSSetShader(m_TitleVertexShader, nullptr, 0);
	m_ImmediateContext->PSSetShader(m_TitlePixelShader, nullptr, 0);
}

void CRenderer::SetPixelShader()
{
	if (!m_ImmediateContext)
	{
		return;
	}
	m_ImmediateContext->PSSetShader(m_PixelShader, nullptr, 0);
}

void CRenderer::BillBoardBlendSet()
{
	if (!m_D3DDevice || !m_ImmediateContext)
	{
		return;
	}
	// ブレンドステート設定
	ZeroMemory(&m_BlendDesc, sizeof(m_BlendDesc));
	m_BlendDesc.AlphaToCoverageEnable = TRUE;
	m_BlendDesc.IndependentBlendEnable = FALSE;
	m_BlendDesc.RenderTarget[0].BlendEnable = TRUE;
	m_BlendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	m_BlendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	m_BlendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	m_BlendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	m_BlendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	m_BlendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	m_BlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	ID3D11BlendState* blendState = nullptr;
	m_D3DDevice->CreateBlendState(&m_BlendDesc, &blendState);
	m_ImmediateContext->OMSetBlendState(blendState, blendFactor, 0xffffffff);
}

void CRenderer::BillBoardBlendEnd()
{
	if (!m_D3DDevice || !m_ImmediateContext)
	{
		return;
	}
	// ブレンドステート設定
	ZeroMemory(&m_BlendDesc, sizeof(m_BlendDesc));
	//ここをfalseでアルファ値0.5以下でも通る
	m_BlendDesc.AlphaToCoverageEnable = FALSE;
	m_BlendDesc.IndependentBlendEnable = TRUE;
	m_BlendDesc.RenderTarget[0].BlendEnable = TRUE;
	m_BlendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	m_BlendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	m_BlendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	m_BlendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	m_BlendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	m_BlendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	m_BlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	ID3D11BlendState* blendState = nullptr;
	m_D3DDevice->CreateBlendState(&m_BlendDesc, &blendState);
	m_ImmediateContext->OMSetBlendState(blendState, blendFactor, 0xffffffff);
}