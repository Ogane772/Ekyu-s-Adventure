#include "../Text/Text.h"
#include "../Renderer/Renderer.h"
#include "../TextureLoader/TextureLoader.h"
#include "../System/SystemContext.h"
#define SAFE_RELEASE(x) if(x){x->Release(); x = nullptr;}
//一文字の横幅
#define TEXT_WIDTH 9.0f * m_Scale
//フォントの横サイズ
#define WIDTH_SCALE 12.0f * m_Scale
//フォントの縦サイズ
#define HEIGHT_SCALE 10.0f * m_Scale
//画像の大きさ
#define TEXTURE_SCALE 128.0f * m_Scale
//表示できる文字数
#define CREATE_TEXT_NUM 100

std::weak_ptr<ID3D11ShaderResourceView> CText::m_Texture;
ID3D11SamplerState* CText::m_SampleLinear;
ID3D11VertexShader* CText::m_VertexShader;
ID3D11PixelShader* CText::m_PixelShader;
ID3D11InputLayout* CText::m_VertexLayout;
ID3D11Buffer* CText::m_ConstantBuffer;
std::vector<ID3D11Buffer*> CText::m_VertexBufferList;
XMMATRIX CText::m_Projection;
float CText::m_Scale;//25pixelを基準 25pixel=1.0f
float CText::m_Alpha;
XMFLOAT4 CText::m_Color;

HRESULT CText::Init(const float Size, const XMFLOAT4 & Color)
{
	ID3D11Device* device = CRenderer::GetDevice();
	if (!device)
	{
		return E_FAIL;
	}
	m_Alpha = Color.w;
	m_Color = Color;
	m_Scale = Size;

	//フォントごとにクアッド作成
	float left = 0, top = 0, right = 0, bottom = 0;
	const int widthTextNum = static_cast<int>(CREATE_TEXT_NUM * 0.1f);
	const int heightTextNum = static_cast<int>(CREATE_TEXT_NUM * 0.1f);
	for (int i = 0; i < widthTextNum; i++)
	{
		for (int j = 0; j < widthTextNum; j++)
		{
			left = i * HEIGHT_SCALE;
			top = j * WIDTH_SCALE;
			right = left + TEXT_WIDTH;;
			bottom = top + WIDTH_SCALE;
			left /= TEXTURE_SCALE;
			top /= TEXTURE_SCALE;
			right /= TEXTURE_SCALE;
			bottom /= TEXTURE_SCALE;
			ShaderTextVertex vertices[] =
			{
				XMFLOAT3(0, WIDTH_SCALE, 0), XMFLOAT2(left,bottom),//頂点1,
				XMFLOAT3(0, 0, 0), XMFLOAT2(left,top),//頂点2
				XMFLOAT3(HEIGHT_SCALE,WIDTH_SCALE, 0), XMFLOAT2(right,bottom), //頂点3
				XMFLOAT3(HEIGHT_SCALE, 0, 0), XMFLOAT2(right,top), //頂点4
			};
			D3D11_BUFFER_DESC bd;
			bd.Usage = D3D11_USAGE_DEFAULT;
			bd.ByteWidth = sizeof(ShaderTextVertex) * 4;
			bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			bd.CPUAccessFlags = 0;
			bd.MiscFlags = 0;

			D3D11_SUBRESOURCE_DATA InitData;
			InitData.pSysMem = vertices;
			ID3D11Buffer* buffer;
			if (FAILED(device->CreateBuffer(&bd, &InitData, &buffer)))
			{
				return E_FAIL;
			}
			m_VertexBufferList.emplace_back(buffer);
		}
	}

	//テクスチャー用サンプラー作成
	D3D11_SAMPLER_DESC SamDesc;
	ZeroMemory(&SamDesc, sizeof(D3D11_SAMPLER_DESC));
	SamDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	SamDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	SamDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	SamDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	device->CreateSamplerState(&SamDesc, &m_SampleLinear);
	
	if (CTextureLoader* textureLoader = CTextureLoader::GetInstance())
	{
		m_Texture = textureLoader->Load(ETextureType::TEXT_FONT);
	}
	
	//hlslファイル読み込み ブロブ作成　ブロブとはシェーダーの塊みたいなもの。XXシェーダーとして特徴を持たない。後で各種シェーダーに成り得る。
	ID3DBlob *pCompiledShader = nullptr;
	ID3DBlob *pErrors = nullptr;
	//ブロブからバーテックスシェーダー作成
	if (FAILED(D3DX11CompileFromFile("shader/TextVSPS.hlsl", nullptr, nullptr, "main", "vs_5_0", 0, 0, nullptr, &pCompiledShader, &pErrors, nullptr)))
	{
		MessageBox(0, "hlsl読み込み失敗", nullptr, MB_OK);
		return E_FAIL;
	}
	SAFE_RELEASE(pErrors);

	if (FAILED(device->CreateVertexShader(pCompiledShader->GetBufferPointer(), pCompiledShader->GetBufferSize(), nullptr, &m_VertexShader)))
	{
		SAFE_RELEASE(pCompiledShader);
		MessageBox(0, "バーテックスシェーダー作成失敗", nullptr, MB_OK);
		return E_FAIL;
	}
	//頂点インプットレイアウトを定義	
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	UINT numElements = sizeof(layout) / sizeof(layout[0]);

	//頂点インプットレイアウトを作成
	if (FAILED(device->CreateInputLayout(layout, numElements, pCompiledShader->GetBufferPointer(), pCompiledShader->GetBufferSize(), &m_VertexLayout)))
	{
		return E_FAIL;
	}
	//ブロブからピクセルシェーダー作成
	if (FAILED(D3DX11CompileFromFile("shader/TextVSPS.hlsl", nullptr, nullptr, "PS", "ps_5_0", 0, 0, nullptr, &pCompiledShader, &pErrors, nullptr)))
	{
		MessageBox(0, "hlsl読み込み失敗", nullptr, MB_OK);
		return E_FAIL;
	}
	SAFE_RELEASE(pErrors);
	if (FAILED(device->CreatePixelShader(pCompiledShader->GetBufferPointer(), pCompiledShader->GetBufferSize(), nullptr, &m_PixelShader)))
	{
		SAFE_RELEASE(pCompiledShader);
		MessageBox(0, "ピクセルシェーダー作成失敗", nullptr, MB_OK);
		return E_FAIL;
	}
	SAFE_RELEASE(pCompiledShader);
	//コンスタントバッファー作成　ここでは変換行列渡し用
	D3D11_BUFFER_DESC cb;
	cb.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cb.ByteWidth = sizeof(TextConstantBuffer);
	cb.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cb.MiscFlags = 0;
	cb.StructureByteStride = 0;
	cb.Usage = D3D11_USAGE_DYNAMIC;

	if (FAILED(device->CreateBuffer(&cb, nullptr, &m_ConstantBuffer)))
	{
		return E_FAIL;
	}

	return S_OK;
}

void CText::UnInit()
{
	if (CTextureLoader* textureLoader = CTextureLoader::GetInstance())
	{
		textureLoader->Release(m_Texture);
	}
	SAFE_RELEASE(m_SampleLinear);
	SAFE_RELEASE(m_ConstantBuffer);
	SAFE_RELEASE(m_VertexShader);
	SAFE_RELEASE(m_PixelShader);
	SAFE_RELEASE(m_VertexLayout);
	for (ID3D11Buffer* buffer : m_VertexBufferList)
	{
		if (buffer)
		{
			buffer->Release();
		}
	}
}

void CText::Render(const char* Text, float X, const float Y)
{
	ID3D11DeviceContext* deviceContext = CRenderer::GetDeviceContext();
	if (!deviceContext)
	{
		return;
	}
	CSystemContext* system = CSystemContext::GetInstance();
	if (!system)
	{
		return;
	}
	const float currentScreenWidth = (std::max)(system->GetCurrentScreenWidth(), 1.0f);
	const float currentScreenHeight = (std::max)(system->GetCurrentScreenHeight(), 1.0f);
	CRenderer::SetDepthEnable(false);

	// プロジェクショントランスフォーム（射影変換）
	m_Projection = XMMatrixOrthographicOffCenterLH(0.0f, currentScreenWidth, currentScreenHeight, 0.0f, 0.0f, 1.0f);
	//トポロジー
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	//頂点インプットレイアウトをセット
	deviceContext->IASetInputLayout(m_VertexLayout);

	//使用するシェーダーの登録
	deviceContext->VSSetShader(m_VertexShader, nullptr, 0);
	deviceContext->PSSetShader(m_PixelShader, nullptr, 0);
	//このコンスタントバッファーを使うシェーダーの登録
	deviceContext->VSSetConstantBuffers(0, 1, &m_ConstantBuffer);
	deviceContext->PSSetConstantBuffers(0, 1, &m_ConstantBuffer);
	//テクスチャーをシェーダーに渡す
	deviceContext->PSSetSamplers(0, 1, &m_SampleLinear);
	ID3D11ShaderResourceView* texture = m_Texture.lock().get();
	deviceContext->PSSetShaderResources(0, 1, &texture);

	for (int i = 0; i < strlen(Text); i++)
	{
		const char font = Text[i];
		const int index = font - 32;
		RenderFont(index, X, Y);
		X += static_cast<int>(TEXT_WIDTH);
	}
	CRenderer::SetDepthEnable(true);
	CRenderer::SetShader();
}

void CText::RenderFont(const int FontIndex, const float X, const float Y)
{
	ID3D11DeviceContext* deviceContext = CRenderer::GetDeviceContext();
	if (!deviceContext)
	{
		return;
	}
	//ワールド変換
	XMMATRIX world = XMMatrixScaling(1, 1, 1);
	world *= XMMatrixTranslation(X, Y, 0);
	
	//シェーダーのコンスタントバッファーに各種データを渡す	
	D3D11_MAPPED_SUBRESOURCE data;
	TextConstantBuffer cb;
	if (SUCCEEDED(deviceContext->Map(m_ConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &data)))
	{
		//ワールド、射影行列を渡す
		XMMATRIX m = world * m_Projection;
		m = XMMatrixTranspose(m);
		cb.WP = m;
		//カラーを渡す
		cb.Color = m_Color;
		//透明度を渡す
		cb.Alpha.x = m_Alpha;

		memcpy_s(data.pData, data.RowPitch, (void*)(&cb), sizeof(cb));
		deviceContext->Unmap(m_ConstantBuffer, 0);
	}
	
	//バーテックスバッファーをセット
	UINT stride = sizeof(ShaderTextVertex);
	UINT offset = 0;
	deviceContext->IASetVertexBuffers(0, 1, &m_VertexBufferList[FontIndex], &stride, &offset);
	
	//描画
	CRenderer::SetDepthEnable(true);
	deviceContext->Draw(4, 0);
	CRenderer::SetDepthEnable(false);
	
}

void CText::Draw(const float X, const float Y, const char* Text, ...)
{
	va_list argp;
	char buf[256];
	va_start(argp, Text);
	vsprintf_s(buf, 256, Text, argp);//バッファ量と第二引数を合わせること
	va_end(argp);

	Render(buf, X, Y);
}