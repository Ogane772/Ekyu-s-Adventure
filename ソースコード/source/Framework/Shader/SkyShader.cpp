#include "SkyShader.h"
#include "../Renderer/Renderer.h"
#include "../Camera/Camera.h"
#include <io.h>

CSkyShader::CSkyShader()
{
	ID3D11Device* device = CRenderer::GetDevice();
	if (!device)
	{
		return;
	}
	FILE* file;

	// 頂点シェーダ生成
	{
		long int fsize;

		fopen_s(&file, "shader/ShaderSkyVS.cso", "rb");
		fsize = _filelength(_fileno(file));
		unsigned char* buffer = new unsigned char[fsize];
		fread(buffer, fsize, 1, file);
		fclose(file);

		device->CreateVertexShader(buffer, fsize, NULL, &m_VertexShader);

		// 入力レイアウト生成
		{
			//頂点インプットレイアウトを定義	
			D3D11_INPUT_ELEMENT_DESC layout[] =
			{
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 36, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "COLOR", 0 ,DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 48, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "TEXCOORD", 0 ,DXGI_FORMAT_R32G32_FLOAT, 0, 64, D3D11_INPUT_PER_VERTEX_DATA , 0 }
			};
			UINT numElements = ARRAYSIZE(layout);

			device->CreateInputLayout(layout,
				numElements,
				buffer,
				fsize,
				&m_VertexLayout);
		}

		delete[] buffer;
	}

	// ピクセルシェーダ生成
	{
		long int fsize;

		fopen_s(&file, "shader/ShaderSkyPS.cso", "rb");
		fsize = _filelength(_fileno(file));
		unsigned char* buffer = new unsigned char[fsize];
		fread(buffer, fsize, 1, file);
		fclose(file);

		device->CreatePixelShader(buffer, fsize, NULL, &m_PixelShader);

		delete[] buffer;
	}

	// 定数バッファ生成
	{
		D3D11_BUFFER_DESC hBufferDesc;
		hBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		hBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		hBufferDesc.CPUAccessFlags = 0;
		hBufferDesc.MiscFlags = 0;
		hBufferDesc.StructureByteStride = sizeof(float);

		hBufferDesc.ByteWidth = sizeof(ConstantSky);
		device->CreateBuffer(&hBufferDesc, NULL, &m_SkyConstantBuffer);
	}
}

CSkyShader::~CSkyShader()
{
	if (m_SkyConstantBuffer)
	{
		m_SkyConstantBuffer->Release();
	}
	if (m_VertexLayout)
	{
		m_VertexLayout->Release();
	}
	if (m_VertexShader)
	{
		m_VertexShader->Release();
	}
	if (m_PixelShader)
	{
		m_PixelShader->Release();
	}
}

void CSkyShader::Set(const XMMATRIX& World, const CCamera* Camera, const float TexScroll)
{
	ID3D11DeviceContext* deviceContext = CRenderer::GetDeviceContext();
	if (!Camera)
	{
		return;
	}
	//定数バッファに値を設定
	const Vector3 eye = Camera->GetPosition();
	m_Constant.WorldMatrix = XMMatrixTranspose(World);
	m_Constant.ViewMatrix = XMMatrixTranspose(Camera->GetInvViewMatrix());
	m_Constant.ProjectionMatrix = XMMatrixTranspose(Camera->GetProjectionMatrix());
	m_Constant.CameraPosition = XMFLOAT4(eye.x, eye.y, eye.z, 1.0f);
	m_Constant.TextureScroll = XMFLOAT4(TexScroll, 0.0f, 0.0f, 0.0f);
	m_Constant.FogParam = CRenderer::GetFogConstant().FogParam;

	// シェーダ設定
	deviceContext->VSSetShader(m_VertexShader, NULL, 0);
	deviceContext->PSSetShader(m_PixelShader, NULL, 0);

	// 入力レイアウト設定
	deviceContext->IASetInputLayout(m_VertexLayout);

	// 定数バッファ更新
	deviceContext->UpdateSubresource(m_SkyConstantBuffer, 0, NULL, &m_Constant, 0, 0);

	// 定数バッファ設定
	deviceContext->VSSetConstantBuffers(0, 1, &m_SkyConstantBuffer);
	deviceContext->PSSetConstantBuffers(0, 1, &m_SkyConstantBuffer);
}

