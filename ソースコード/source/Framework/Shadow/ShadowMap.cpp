#include "ShadowMap.h"
#include <D3DX11async.h>
#include "ShadowType.h"
#include "../Light/DirectionalLight.h"
#include "../Renderer/Renderer.h"
#include "../Scene/SceneManager.h"
#include "../Camera/Camera.h"
#include "../System/SystemContext.h"

#define SAFE_RELEASE(x) if(x){ x->Release(); x = nullptr;}

namespace
{
	constexpr float DEFAULT_SHADOW_VIEW = 0.01f;
	constexpr float DEFAULT_LIGHT_POSITION_X = -13.0f;
	constexpr float DEFAULT_LIGHT_POSITION_Y = 33.0f;
	constexpr float DEFAULT_LIGHT_ROTATION_X = 79.385f;
	constexpr float DEFAULT_LIGHT_ROTATION_Y = -9.231f;
	constexpr float DEFAULT_LIGHT_ROTATION_Z = 31.385f;
	constexpr float DEPTH_FAR = 256.0f;
}

XMMATRIX CShadowMap::m_LightProjectionMatrix;
CShadowMap::ShadowLightData CShadowMap::m_LightData;
XMMATRIX CShadowMap::m_LightView;
Vector3 CShadowMap::m_DefaultPosition;
Vector3 CShadowMap::m_DefaultRotation;
Vector3 CShadowMap::m_LightFront;
float CShadowMap::m_ShadowView = DEFAULT_SHADOW_VIEW;

Vector3 CShadowMap::m_LightFrontUP;
XMMATRIX CShadowMap::m_LightViewUp;
XMMATRIX CShadowMap::m_LightProjectionUp;

XMMATRIX CShadowMap::m_ClipToUV;

ID3D11VertexShader* CShadowMap::m_NormalMapVertexShader = nullptr;
ID3D11PixelShader* CShadowMap::m_NormalMapPixelShader = nullptr;
ID3D11VertexShader* CShadowMap::m_VertexShader = nullptr;
ID3D11PixelShader* CShadowMap::m_PixelShader = nullptr;
ID3D11VertexShader* CShadowMap::m_LightVertexShader = nullptr;
ID3D11PixelShader* CShadowMap::m_LightPixelShader = nullptr;
ID3D11VertexShader* CShadowMap::m_DisableSelfLightVertexShader = nullptr;
ID3D11PixelShader* CShadowMap::m_DisableSelfLightPixelShader = nullptr;
ID3D11VertexShader* CShadowMap::m_DepthVertexShader = nullptr;
ID3D11PixelShader* CShadowMap::m_DepthPixelShader = nullptr;
ID3D11InputLayout* CShadowMap::m_VertexLayout = nullptr;;
ID3D11Buffer* CShadowMap::m_LightDataBuffer = nullptr;
ID3D11Buffer* CShadowMap::m_StaticMeshBuffer = nullptr;
ID3D11Buffer* CShadowMap::m_MaterialBuffer = nullptr;
ID3D11Buffer* CShadowMap::m_MatrixAndFogBuffer = nullptr;
ID3D11Buffer* CShadowMap::m_DirectionalLightBuffer = nullptr;

ID3D11InputLayout* CShadowMap::m_SkeletalVertexLayout = nullptr;
ID3D11Buffer* CShadowMap::m_SkeletalLightBuffer = nullptr;
ID3D11Buffer* CShadowMap::m_SkeletalMeshBuffer = nullptr;
ID3D11Buffer* CShadowMap::m_SkeletalMaterialBuffer = nullptr;
ID3D11Buffer* CShadowMap::m_SkeletalBoneDataBuffer = nullptr;
ID3D11VertexShader* CShadowMap::m_SkeletalVertexShader = nullptr;
ID3D11PixelShader* CShadowMap::m_SkeletalPixelShader = nullptr;
ID3D11VertexShader* CShadowMap::m_SkeletalDepthVertexShader = nullptr;
ID3D11PixelShader* CShadowMap::m_SkeletalDepthPixelShader = nullptr;

ShaderShadowLightProjection CShadowMap::m_ShaderLightProjection;
ShaderShadowLightProjection CShadowMap::m_DefaultShaderLightProjection;

void CShadowMap::Init()
{
	m_ShaderLightProjection.View_Width = DEPTH_FAR;
	m_ShaderLightProjection.View_Height = DEPTH_FAR;
	m_ShaderLightProjection.FarZ = DEPTH_FAR;
	m_DefaultShaderLightProjection = m_ShaderLightProjection;
	m_LightData.Position.x = DEFAULT_LIGHT_POSITION_X;
	m_LightData.Position.y = DEFAULT_LIGHT_POSITION_Y;
	m_LightData.Rotation.x = DEFAULT_LIGHT_ROTATION_X;
	m_LightData.Rotation.y = DEFAULT_LIGHT_ROTATION_Y;
	m_LightData.Rotation.z = DEFAULT_LIGHT_ROTATION_Z;
	m_DefaultRotation = m_LightData.Rotation;
	m_DefaultPosition = m_LightData.Position;

	XMFLOAT4X4 clip;
	//影用テクスチャのサイズ設定
	clip._11 = 1.5f;
	clip._22 = -1.5f;
	clip._33 = 2.0f;
	clip._41 = 1.5f;
	clip._42 = 1.5f;
	clip._44 = 2.0f;
	m_ClipToUV = XMLoadFloat4x4(&clip);

	StaticMeshShaderInit();

	SkinMeshShaderInit();
}

void CShadowMap::StaticMeshShaderInit()
{
	//hlslファイル読み込み ブロブ作成　ブロブとはシェーダーの塊みたいなもの。XXシェーダーとして特徴を持たない。後で各種シェーダーに成り得る。
	ID3DBlob *pCompiledShader = nullptr;
	ID3DBlob *pErrors = nullptr;
	//ブロブからバーテックスシェーダー作成
	if (FAILED(D3DX11CompileFromFile("shader/ShadowMapVSPS.hlsl", nullptr, nullptr, "VS", "vs_5_0", D3D10_SHADER_DEBUG | D3D10_SHADER_SKIP_OPTIMIZATION, 0, nullptr, &pCompiledShader, &pErrors, nullptr)))
	{
		MessageBoxA(0, (LPSTR)pErrors->GetBufferPointer(), nullptr, MB_OK);
		return;
	}
	SAFE_RELEASE(pErrors);

	if (FAILED(CRenderer::GetDevice()->CreateVertexShader(pCompiledShader->GetBufferPointer(), pCompiledShader->GetBufferSize(), nullptr, &m_VertexShader)))
	{
		SAFE_RELEASE(pCompiledShader);
		MessageBox(0, "バーテックスシェーダー作成失敗", nullptr, MB_OK);
		return;
	}
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
	UINT numElements = sizeof(layout) / sizeof(layout[0]);
	//頂点インプットレイアウトを作成
	if (FAILED(CRenderer::GetDevice()->CreateInputLayout(layout, numElements, pCompiledShader->GetBufferPointer(), pCompiledShader->GetBufferSize(), &m_VertexLayout)))
	{
		return;
	}

	//ブロブからノーマルマップバーテックスシェーダー作成
	if (FAILED(D3DX11CompileFromFile("shader/LightShadowMapVSPS.hlsl", nullptr, nullptr, "VS", "vs_5_0", D3D10_SHADER_DEBUG | D3D10_SHADER_SKIP_OPTIMIZATION, 0, nullptr, &pCompiledShader, &pErrors, nullptr)))
	{
		MessageBoxA(0, (LPSTR)pErrors->GetBufferPointer(), nullptr, MB_OK);
		return;
	}
	SAFE_RELEASE(pErrors);

	if (FAILED(CRenderer::GetDevice()->CreateVertexShader(pCompiledShader->GetBufferPointer(), pCompiledShader->GetBufferSize(), nullptr, &m_LightVertexShader)))
	{
		SAFE_RELEASE(pCompiledShader);
		MessageBox(0, "バーテックスシェーダー作成失敗", nullptr, MB_OK);
		return;
	}
	//ブロブからノーマルマップピクセルシェーダー作成
	if (FAILED(D3DX11CompileFromFile("shader/LightShadowMapVSPS.hlsl", nullptr, nullptr, "PS", "ps_5_0", D3D10_SHADER_DEBUG | D3D10_SHADER_SKIP_OPTIMIZATION, 0, nullptr, &pCompiledShader, &pErrors, nullptr)))
	{
		MessageBoxA(0, (LPSTR)pErrors->GetBufferPointer(), nullptr, MB_OK);
		return;
	}
	SAFE_RELEASE(pErrors);
	if (FAILED(CRenderer::GetDevice()->CreatePixelShader(pCompiledShader->GetBufferPointer(), pCompiledShader->GetBufferSize(), nullptr, &m_LightPixelShader)))
	{
		SAFE_RELEASE(pCompiledShader);
		MessageBox(0, "ピクセルシェーダー作成失敗", nullptr, MB_OK);
		return;
	}

	//ブロブからノーマルマップバーテックスシェーダー作成
	if (FAILED(D3DX11CompileFromFile("shader/LightShadowMapNoSelfShadowVSPS.hlsl", nullptr, nullptr, "VS", "vs_5_0", D3D10_SHADER_DEBUG | D3D10_SHADER_SKIP_OPTIMIZATION, 0, nullptr, &pCompiledShader, &pErrors, nullptr)))
	{
		MessageBoxA(0, (LPSTR)pErrors->GetBufferPointer(), nullptr, MB_OK);
		return;
	}
	SAFE_RELEASE(pErrors);

	if (FAILED(CRenderer::GetDevice()->CreateVertexShader(pCompiledShader->GetBufferPointer(), pCompiledShader->GetBufferSize(), nullptr, &m_DisableSelfLightVertexShader)))
	{
		SAFE_RELEASE(pCompiledShader);
		MessageBox(0, "バーテックスシェーダー作成失敗", nullptr, MB_OK);
		return;
	}
	//ブロブからノーマルマップピクセルシェーダー作成
	if (FAILED(D3DX11CompileFromFile("shader/LightShadowMapNoSelfShadowVSPS.hlsl", nullptr, nullptr, "PS", "ps_5_0", D3D10_SHADER_DEBUG | D3D10_SHADER_SKIP_OPTIMIZATION, 0, nullptr, &pCompiledShader, &pErrors, nullptr)))
	{
		MessageBoxA(0, (LPSTR)pErrors->GetBufferPointer(), nullptr, MB_OK);
		return;
	}
	SAFE_RELEASE(pErrors);
	if (FAILED(CRenderer::GetDevice()->CreatePixelShader(pCompiledShader->GetBufferPointer(), pCompiledShader->GetBufferSize(), nullptr, &m_DisableSelfLightPixelShader)))
	{
		SAFE_RELEASE(pCompiledShader);
		MessageBox(0, "ピクセルシェーダー作成失敗", nullptr, MB_OK);
		return;
	}

	//ブロブからノーマルマップバーテックスシェーダー作成
	if (FAILED(D3DX11CompileFromFile("shader/ShadowMapNormalMappingVSPS.hlsl", nullptr, nullptr, "VS", "vs_5_0", D3D10_SHADER_DEBUG | D3D10_SHADER_SKIP_OPTIMIZATION, 0, nullptr, &pCompiledShader, &pErrors, nullptr)))
	{
		MessageBoxA(0, (LPSTR)pErrors->GetBufferPointer(), nullptr, MB_OK);
		return;
	}
	SAFE_RELEASE(pErrors);

	if (FAILED(CRenderer::GetDevice()->CreateVertexShader(pCompiledShader->GetBufferPointer(), pCompiledShader->GetBufferSize(), nullptr, &m_NormalMapVertexShader)))
	{
		SAFE_RELEASE(pCompiledShader);
		MessageBox(0, "バーテックスシェーダー作成失敗", nullptr, MB_OK);
		return;
	}
	//ブロブからノーマルマップピクセルシェーダー作成
	if (FAILED(D3DX11CompileFromFile("shader/ShadowMapNormalMappingVSPS.hlsl", nullptr, nullptr, "PS", "ps_5_0", D3D10_SHADER_DEBUG | D3D10_SHADER_SKIP_OPTIMIZATION, 0, nullptr, &pCompiledShader, &pErrors, nullptr)))
	{
		MessageBoxA(0, (LPSTR)pErrors->GetBufferPointer(), nullptr, MB_OK);
		return;
	}
	SAFE_RELEASE(pErrors);
	if (FAILED(CRenderer::GetDevice()->CreatePixelShader(pCompiledShader->GetBufferPointer(), pCompiledShader->GetBufferSize(), nullptr, &m_NormalMapPixelShader)))
	{
		SAFE_RELEASE(pCompiledShader);
		MessageBox(0, "ピクセルシェーダー作成失敗", nullptr, MB_OK);
		return;
	}

	//ブロブからピクセルシェーダー作成
	if (FAILED(D3DX11CompileFromFile("shader/ShadowMapVSPS.hlsl", nullptr, nullptr, "PS", "ps_5_0", D3D10_SHADER_DEBUG | D3D10_SHADER_SKIP_OPTIMIZATION, 0, nullptr, &pCompiledShader, &pErrors, nullptr)))
	{
		MessageBoxA(0, (LPSTR)pErrors->GetBufferPointer(), nullptr, MB_OK);
		return;
	}
	SAFE_RELEASE(pErrors);
	if (FAILED(CRenderer::GetDevice()->CreatePixelShader(pCompiledShader->GetBufferPointer(), pCompiledShader->GetBufferSize(), nullptr, &m_PixelShader)))
	{
		SAFE_RELEASE(pCompiledShader);
		MessageBox(0, "ピクセルシェーダー作成失敗", nullptr, MB_OK);
		return;
	}

	//ブロブから深度テクスチャー用バーテックスシェーダー作成
	if (FAILED(D3DX11CompileFromFile("shader/ShadowMapVSPS.hlsl", nullptr, nullptr, "VS_Depth", "vs_5_0", D3D10_SHADER_DEBUG | D3D10_SHADER_SKIP_OPTIMIZATION, 0, nullptr, &pCompiledShader, &pErrors, nullptr)))
	{
		MessageBoxA(0, (LPSTR)pErrors->GetBufferPointer(), nullptr, MB_OK);
		return;
	}
	SAFE_RELEASE(pErrors);

	if (FAILED(CRenderer::GetDevice()->CreateVertexShader(pCompiledShader->GetBufferPointer(), pCompiledShader->GetBufferSize(), nullptr, &m_DepthVertexShader)))
	{
		SAFE_RELEASE(pCompiledShader);
		MessageBox(0, "バーテックスシェーダー作成失敗", nullptr, MB_OK);
		return;
	}
	//ブロブから深度テクスチャー用ピクセルシェーダー作成
	if (FAILED(D3DX11CompileFromFile("shader/ShadowMapVSPS.hlsl", nullptr, nullptr, "PS_Depth", "ps_5_0", D3D10_SHADER_DEBUG | D3D10_SHADER_SKIP_OPTIMIZATION, 0, nullptr, &pCompiledShader, &pErrors, nullptr)))
	{
		MessageBoxA(0, (LPSTR)pErrors->GetBufferPointer(), nullptr, MB_OK);
		return;
	}
	SAFE_RELEASE(pErrors);
	if (FAILED(CRenderer::GetDevice()->CreatePixelShader(pCompiledShader->GetBufferPointer(), pCompiledShader->GetBufferSize(), nullptr, &m_DepthPixelShader)))
	{
		SAFE_RELEASE(pCompiledShader);
		MessageBox(0, "ピクセルシェーダー作成失敗", nullptr, MB_OK);
		return;
	}
	SAFE_RELEASE(pCompiledShader);
	//コンスタントバッファー作成　フレーム単位で変更する情報
	D3D11_BUFFER_DESC cb;
	cb.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cb.ByteWidth = sizeof(CBufferLightData);
	cb.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cb.MiscFlags = 0;
	cb.Usage = D3D11_USAGE_DYNAMIC;

	if (FAILED(CRenderer::GetDevice()->CreateBuffer(&cb, nullptr, &m_LightDataBuffer)))
	{
		return;
	}
	//コンスタントバッファー作成　メッシュ単位で変更する情報
	cb.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cb.ByteWidth = sizeof(BufferStaticMeshData);
	cb.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cb.MiscFlags = 0;
	cb.Usage = D3D11_USAGE_DYNAMIC;

	if (FAILED(CRenderer::GetDevice()->CreateBuffer(&cb, nullptr, &m_StaticMeshBuffer)))
	{
		return;
	}
	//コンスタントバッファー作成　マテリアル単位で変更する情報
	cb.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cb.ByteWidth = sizeof(BufferMaterial);
	cb.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cb.MiscFlags = 0;
	cb.Usage = D3D11_USAGE_DYNAMIC;

	if (FAILED(CRenderer::GetDevice()->CreateBuffer(&cb, nullptr, &m_MaterialBuffer)))
	{
		return;
	}

	//コンスタントバッファー3作成
	cb.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cb.ByteWidth = sizeof(ShaderMatrixAndFogData);
	cb.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cb.MiscFlags = 0;
	cb.StructureByteStride = 0;
	cb.Usage = D3D11_USAGE_DYNAMIC;

	if (FAILED(CRenderer::GetDevice()->CreateBuffer(&cb, nullptr, &m_MatrixAndFogBuffer)))
	{
		return;
	}
	//コンスタントバッファー4作成  
	cb.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cb.ByteWidth = sizeof(LIGHT);
	cb.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cb.MiscFlags = 0;
	cb.StructureByteStride = 0;
	cb.Usage = D3D11_USAGE_DYNAMIC;

	if (FAILED(CRenderer::GetDevice()->CreateBuffer(&cb, nullptr, &m_DirectionalLightBuffer)))
	{
		return;
	}

}

void CShadowMap::SkinMeshShaderInit()
{
	//hlslファイル読み込み ブロブ作成　ブロブとはシェーダーの塊みたいなもの。XXシェーダーとして特徴を持たない。後で各種シェーダーに成り得る。
	ID3DBlob *pCompiledShader = nullptr;
	ID3DBlob *pErrors = nullptr;
	//ブロブからバーテックスシェーダー作成
	if (FAILED(D3DX11CompileFromFile("shader/SkinShadowMapVSPS.hlsl", nullptr, nullptr, "VS", "vs_5_0", D3D10_SHADER_DEBUG | D3D10_SHADER_SKIP_OPTIMIZATION, 0, nullptr, &pCompiledShader, &pErrors, nullptr)))
	{
		MessageBoxA(0, (LPSTR)pErrors->GetBufferPointer(), nullptr, MB_OK);
		return;
	}
	SAFE_RELEASE(pErrors);

	if (FAILED(CRenderer::GetDevice()->CreateVertexShader(pCompiledShader->GetBufferPointer(), pCompiledShader->GetBufferSize(), nullptr, &m_SkeletalVertexShader)))
	{
		SAFE_RELEASE(pCompiledShader);
		MessageBox(0, "バーテックスシェーダー作成失敗", nullptr, MB_OK);
		return;
	}
	//頂点インプットレイアウトを定義	
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BONE_INDEX", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BONE_WEIGHT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 48, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	UINT numElements = sizeof(layout) / sizeof(layout[0]);
	//頂点インプットレイアウトを作成
	if (FAILED(CRenderer::GetDevice()->CreateInputLayout(layout, numElements, pCompiledShader->GetBufferPointer(), pCompiledShader->GetBufferSize(), &m_SkeletalVertexLayout)))
	{
		return;
	}
	//ブロブからピクセルシェーダー作成
	if (FAILED(D3DX11CompileFromFile("shader/SkinShadowMapVSPS.hlsl", nullptr, nullptr, "PS", "ps_5_0", D3D10_SHADER_DEBUG | D3D10_SHADER_SKIP_OPTIMIZATION, 0, nullptr, &pCompiledShader, &pErrors, nullptr)))
	{
		MessageBoxA(0, (LPSTR)pErrors->GetBufferPointer(), nullptr, MB_OK);
		return;
	}
	SAFE_RELEASE(pErrors);
	if (FAILED(CRenderer::GetDevice()->CreatePixelShader(pCompiledShader->GetBufferPointer(), pCompiledShader->GetBufferSize(), nullptr, &m_SkeletalPixelShader)))
	{
		SAFE_RELEASE(pCompiledShader);
		MessageBox(0, "ピクセルシェーダー作成失敗", nullptr, MB_OK);
		return;
	}

	//ブロブから深度テクスチャー用バーテックスシェーダー作成
	if (FAILED(D3DX11CompileFromFile("shader/SkinShadowMapVSPS.hlsl", nullptr, nullptr, "VS_Depth", "vs_5_0", D3D10_SHADER_DEBUG | D3D10_SHADER_SKIP_OPTIMIZATION, 0, nullptr, &pCompiledShader, &pErrors, nullptr)))
	{
		MessageBoxA(0, (LPSTR)pErrors->GetBufferPointer(), nullptr, MB_OK);
		return;
	}
	SAFE_RELEASE(pErrors);

	if (FAILED(CRenderer::GetDevice()->CreateVertexShader(pCompiledShader->GetBufferPointer(), pCompiledShader->GetBufferSize(), nullptr, &m_SkeletalDepthVertexShader)))
	{
		SAFE_RELEASE(pCompiledShader);
		MessageBox(0, "バーテックスシェーダー作成失敗", nullptr, MB_OK);
		return;
	}
	//ブロブから深度テクスチャー用ピクセルシェーダー作成
	if (FAILED(D3DX11CompileFromFile("shader/SkinShadowMapVSPS.hlsl", nullptr, nullptr, "PS_Depth", "ps_5_0", D3D10_SHADER_DEBUG | D3D10_SHADER_SKIP_OPTIMIZATION, 0, nullptr, &pCompiledShader, &pErrors, nullptr)))
	{
		MessageBoxA(0, (LPSTR)pErrors->GetBufferPointer(), nullptr, MB_OK);
		return;
	}
	SAFE_RELEASE(pErrors);
	if (FAILED(CRenderer::GetDevice()->CreatePixelShader(pCompiledShader->GetBufferPointer(), pCompiledShader->GetBufferSize(), nullptr, &m_SkeletalDepthPixelShader)))
	{
		SAFE_RELEASE(pCompiledShader);
		MessageBox(0, "ピクセルシェーダー作成失敗", nullptr, MB_OK);
		return;
	}
	SAFE_RELEASE(pCompiledShader);
	//コンスタントバッファー作成　フレーム単位で変更する情報
	D3D11_BUFFER_DESC cb;
	cb.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cb.ByteWidth = sizeof(CBufferLightData);
	cb.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cb.MiscFlags = 0;
	cb.Usage = D3D11_USAGE_DYNAMIC;

	if (FAILED(CRenderer::GetDevice()->CreateBuffer(&cb, nullptr, &m_SkeletalLightBuffer)))
	{
		return;
	}
	//コンスタントバッファー作成　メッシュ単位で変更する情報
	cb.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cb.ByteWidth = sizeof(BufferSkeletalMeshData);
	cb.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cb.MiscFlags = 0;
	cb.Usage = D3D11_USAGE_DYNAMIC;

	if (FAILED(CRenderer::GetDevice()->CreateBuffer(&cb, nullptr, &m_SkeletalMeshBuffer)))
	{
		return;
	}
	//コンスタントバッファー作成　マテリアル単位で変更する情報
	cb.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cb.ByteWidth = sizeof(BufferMaterial);
	cb.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cb.MiscFlags = 0;
	cb.Usage = D3D11_USAGE_DYNAMIC;

	if (FAILED(CRenderer::GetDevice()->CreateBuffer(&cb, nullptr, &m_SkeletalMaterialBuffer)))
	{
		return;
	}
	//コンスタントバッファーボーン用　作成  
	cb.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cb.ByteWidth = sizeof(ShaderBoneData);
	cb.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cb.MiscFlags = 0;
	cb.StructureByteStride = 0;
	cb.Usage = D3D11_USAGE_DYNAMIC;

	if (FAILED(CRenderer::GetDevice()->CreateBuffer(&cb, nullptr, &m_SkeletalBoneDataBuffer)))
	{
		return;
	}
}

void CShadowMap::UnInit()
{
	SAFE_RELEASE(m_NormalMapVertexShader);
	SAFE_RELEASE(m_NormalMapPixelShader);
	SAFE_RELEASE(m_VertexShader);
	SAFE_RELEASE(m_PixelShader);
	SAFE_RELEASE(m_DepthVertexShader);
	SAFE_RELEASE(m_DepthPixelShader);

	SAFE_RELEASE(m_VertexLayout);
	SAFE_RELEASE(m_LightDataBuffer);
	SAFE_RELEASE(m_StaticMeshBuffer);
	SAFE_RELEASE(m_MaterialBuffer);
	SAFE_RELEASE(m_MatrixAndFogBuffer);
	SAFE_RELEASE(m_DirectionalLightBuffer);

	SAFE_RELEASE(m_SkeletalBoneDataBuffer);
	SAFE_RELEASE(m_SkeletalVertexLayout);
	SAFE_RELEASE(m_SkeletalLightBuffer);
	SAFE_RELEASE(m_SkeletalMeshBuffer);
	SAFE_RELEASE(m_SkeletalMaterialBuffer);
	SAFE_RELEASE(m_SkeletalVertexShader);
	SAFE_RELEASE(m_SkeletalPixelShader);
	SAFE_RELEASE(m_SkeletalDepthVertexShader);
	SAFE_RELEASE(m_SkeletalDepthPixelShader);
	SAFE_RELEASE(m_LightVertexShader);
	SAFE_RELEASE(m_LightPixelShader);
	SAFE_RELEASE(m_DisableSelfLightVertexShader);
	SAFE_RELEASE(m_DisableSelfLightPixelShader);
}

void CShadowMap::Update()
{
	CSceneManager* sceneManager = CSceneManager::GetInstance();
	if (!sceneManager)
	{
		return;
	}
	CCamera* camera = sceneManager->GetCamera();
	if (!camera)
	{
		return;
	}
	const Vector3 eye = camera->GetPosition() + camera->GetFront() * DEPTH_FAR*0.5f;
	//カメラ視点のライト情報設定
	{
		const XMMATRIX light_rot = XMMatrixRotationRollPitchYaw(XMConvertToRadians(m_LightData.Rotation.x), XMConvertToRadians(m_LightData.Rotation.y), XMConvertToRadians(m_LightData.Rotation.z));
		m_LightFront = XMFLOAT3(light_rot.r[2].m128_f32[0], light_rot.r[2].m128_f32[1], light_rot.r[2].m128_f32[2]);
		m_LightFront.Normalize();
		const XMFLOAT3 pos = XMFLOAT3(eye.x - m_LightFront.x * DEPTH_FAR*0.5f, eye.y - m_LightFront.y * DEPTH_FAR*0.5f, eye.z - m_LightFront.z * DEPTH_FAR*0.5f);

		const XMFLOAT3 at = XMFLOAT3(eye.x + m_LightFront.x, eye.y + m_LightFront.y, eye.z + m_LightFront.z);

		m_LightView = XMMatrixLookAtLH({ pos.x,pos.y,pos.z,1.0f }, { at.x,at.y,at.z,1.0f }, { 0.0f,1.0f,0.0f,1.0f });
		m_LightProjectionMatrix = XMMatrixOrthographicLH(m_ShaderLightProjection.View_Width, m_ShaderLightProjection.View_Height, 1.0f, m_ShaderLightProjection.FarZ);
	}
	//上空からのカメラ視点作成
	{
		const XMMATRIX light_rot = XMMatrixRotationRollPitchYaw(XMConvertToRadians(m_LightData.Rotation.x), XMConvertToRadians(m_LightData.Rotation.y), XMConvertToRadians(m_LightData.Rotation.z));
		m_LightFrontUP = XMFLOAT3(light_rot.r[2].m128_f32[0], light_rot.r[2].m128_f32[1], light_rot.r[2].m128_f32[2]);
		m_LightFrontUP.Normalize();
		const XMFLOAT3 pos = XMFLOAT3(eye.x - m_LightFrontUP.x * DEPTH_FAR*0.5f, eye.y - m_LightFrontUP.y * DEPTH_FAR*0.5f, eye.z - m_LightFrontUP.z * DEPTH_FAR*0.5f);

		const XMFLOAT3 at = XMFLOAT3(eye.x + m_LightFrontUP.x, eye.y + m_LightFrontUP.y, eye.z + m_LightFrontUP.z);

		m_LightViewUp = XMMatrixLookAtLH({ pos.x,pos.y,pos.z,1.0f }, { at.x,at.y,at.z,1.0f }, { 0.0f,1.0f,0.0f,1.0f });
		m_LightProjectionUp = XMMatrixOrthographicLH(m_ShaderLightProjection.View_Width, m_ShaderLightProjection.View_Height, 1.0f, m_ShaderLightProjection.FarZ);
	}
}

void CShadowMap::SetConstantBuffers()
{
	ID3D11DeviceContext* deviceContext = CRenderer::GetDeviceContext();
	if (!deviceContext)
	{
		return;
	}
	deviceContext->VSSetConstantBuffers(0, 1, &m_LightDataBuffer);
	deviceContext->PSSetConstantBuffers(0, 1, &m_LightDataBuffer);
	deviceContext->VSSetConstantBuffers(1, 1, &m_StaticMeshBuffer);
	deviceContext->PSSetConstantBuffers(1, 1, &m_StaticMeshBuffer);
	deviceContext->VSSetConstantBuffers(2, 1, &m_MaterialBuffer);
	deviceContext->PSSetConstantBuffers(2, 1, &m_MaterialBuffer);
	deviceContext->VSSetConstantBuffers(3, 1, &m_MatrixAndFogBuffer);
	deviceContext->PSSetConstantBuffers(3, 1, &m_MatrixAndFogBuffer);
	deviceContext->VSSetConstantBuffers(4, 1, &m_DirectionalLightBuffer);
	deviceContext->PSSetConstantBuffers(4, 1, &m_DirectionalLightBuffer);
}

void CShadowMap::SetSkinConstantBuffers()
{
	ID3D11DeviceContext* deviceContext = CRenderer::GetDeviceContext();
	if (!deviceContext)
	{
		return;
	}
	deviceContext->VSSetConstantBuffers(0, 1, &m_SkeletalLightBuffer);
	deviceContext->PSSetConstantBuffers(0, 1, &m_SkeletalLightBuffer);
	deviceContext->VSSetConstantBuffers(1, 1, &m_SkeletalMeshBuffer);
	deviceContext->PSSetConstantBuffers(1, 1, &m_SkeletalMeshBuffer);
	deviceContext->VSSetConstantBuffers(2, 1, &m_SkeletalMaterialBuffer);
	deviceContext->PSSetConstantBuffers(2, 1, &m_SkeletalMaterialBuffer);
	deviceContext->VSSetConstantBuffers(3, 1, &m_SkeletalBoneDataBuffer);
	deviceContext->PSSetConstantBuffers(3, 1, &m_SkeletalBoneDataBuffer);
}

void CShadowMap::SetLightCameraPosition()
{
	ID3D11DeviceContext* deviceContext = CRenderer::GetDeviceContext();
	if (!deviceContext)
	{
		return;
	}
	CSceneManager* sceneManager = CSceneManager::GetInstance();
	if (!sceneManager)
	{
		return;
	}
	D3D11_MAPPED_SUBRESOURCE data;
	CCamera* camera = sceneManager->GetCamera();
	if (!camera)
	{
		return;
	}
	Vector3 eye = camera->GetPosition();
	if (SUCCEEDED(deviceContext->Map(m_LightDataBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &data)))
	{
		CBufferLightData sg;
		sg.LightPos = XMFLOAT4(m_LightData.Position.x, m_LightData.Position.y, m_LightData.Position.z, 1);
		sg.Eye = XMFLOAT4(eye.x, eye.y, eye.z, 0);
		sg.ShadowView = XMFLOAT4(m_ShadowView, 0, 0, 0);
		memcpy_s(data.pData, data.RowPitch, (void*)&sg, sizeof(CBufferLightData));
		deviceContext->Unmap(m_LightDataBuffer, 0);
	}
}

void CShadowMap::SetSkinLightCameraPosition()
{
	ID3D11DeviceContext* deviceContext = CRenderer::GetDeviceContext();
	if (!deviceContext)
	{
		return;
	}
	CSceneManager* sceneManager = CSceneManager::GetInstance();
	if (!sceneManager)
	{
		return;
	}
	D3D11_MAPPED_SUBRESOURCE data;
	CCamera* camera = sceneManager->GetCamera();
	if (!camera)
	{
		return;
	}
	Vector3 eye = camera->GetPosition();
	if (SUCCEEDED(deviceContext->Map(m_SkeletalLightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &data)))
	{
		CBufferLightData sg;
		sg.LightPos = XMFLOAT4(m_LightData.Position.x, m_LightData.Position.y, m_LightData.Position.z, 1);
		sg.Eye = XMFLOAT4(eye.x, eye.y, eye.z, 0);
		sg.ShadowView = XMFLOAT4(m_ShadowView, 0, 0, 0);
		memcpy_s(data.pData, data.RowPitch, (void*)&sg, sizeof(CBufferLightData));
		deviceContext->Unmap(m_SkeletalLightBuffer, 0);
	}
}

void CShadowMap::SetDepthShader()
{
	ID3D11DeviceContext* deviceContext = CRenderer::GetDeviceContext();
	if (!deviceContext)
	{
		return;
	}
	deviceContext->VSSetShader(m_DepthVertexShader, nullptr, 0);
	deviceContext->PSSetShader(m_DepthPixelShader, nullptr, 0);
}

void CShadowMap::SetSkinDepthShader()
{
	ID3D11DeviceContext* deviceContext = CRenderer::GetDeviceContext();
	if (!deviceContext)
	{
		return;
	}
	deviceContext->VSSetShader(m_SkeletalDepthVertexShader, nullptr, 0);
	deviceContext->PSSetShader(m_SkeletalDepthPixelShader, nullptr, 0);
}

void CShadowMap::SetNormalModelShader()
{
	ID3D11DeviceContext* deviceContext = CRenderer::GetDeviceContext();
	if (!deviceContext)
	{
		return;
	}
	deviceContext->VSSetShader(m_VertexShader, nullptr, 0);
	deviceContext->PSSetShader(m_PixelShader, nullptr, 0);
}

void CShadowMap::SetNormalModelLightShader()
{
	ID3D11DeviceContext* deviceContext = CRenderer::GetDeviceContext();
	if (!deviceContext)
	{
		return;
	}
	deviceContext->VSSetShader(m_LightVertexShader, nullptr, 0);
	deviceContext->PSSetShader(m_LightPixelShader, nullptr, 0);
}

void CShadowMap::SetNormalModelDisableSelfLightShader()
{
	ID3D11DeviceContext* deviceContext = CRenderer::GetDeviceContext();
	if (!deviceContext)
	{
		return;
	}
	deviceContext->VSSetShader(m_DisableSelfLightVertexShader, nullptr, 0);
	deviceContext->PSSetShader(m_DisableSelfLightPixelShader, nullptr, 0);
}

void CShadowMap::SetNormalMappingModelShader()
{
	ID3D11DeviceContext* deviceContext = CRenderer::GetDeviceContext();
	if (!deviceContext)
	{
		return;
	}
	deviceContext->VSSetShader(m_NormalMapVertexShader, nullptr, 0);
	deviceContext->PSSetShader(m_NormalMapPixelShader, nullptr, 0);
}

void CShadowMap::SetSkinMeshShader()
{
	ID3D11DeviceContext* deviceContext = CRenderer::GetDeviceContext();
	if (!deviceContext)
	{
		return;
	}
	deviceContext->VSSetShader(m_SkeletalVertexShader, nullptr, 0);
	deviceContext->PSSetShader(m_SkeletalPixelShader, nullptr, 0);
}

void CShadowMap::SetNormalModelMatrix(const EShadowType Type, const XMMATRIX& Matrix)
{
	ID3D11DeviceContext* deviceContext = CRenderer::GetDeviceContext();
	if (!deviceContext)
	{
		return;
	}
	CSceneManager* sceneManager = CSceneManager::GetInstance();
	if (!sceneManager)
	{
		return;
	}
	CCamera* camera = sceneManager->GetCamera();
	if (!camera)
	{
		return;
	}
	Vector3 eye = camera->GetPosition();
	const XMMATRIX& view = camera->GetInvViewMatrix();
	const XMMATRIX& proj = camera->GetProjectionMatrix();
	D3D11_MAPPED_SUBRESOURCE data;
	if (SUCCEEDED(deviceContext->Map(m_StaticMeshBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &data)))
	{
		BufferStaticMeshData cb;
		//ワールド行列を渡す
		cb.W = XMMatrixTranspose(Matrix);
		//ワールド、カメラ、射影行列　を渡す
		XMMATRIX m;
		m = Matrix * view * proj;
		m = XMMatrixTranspose(m);
		cb.WVP = m;
		if (Type == EShadowType::NO_DOWN_SHADOW)
		{
			//ワールド、ライトビュー、射影行列　を渡す
			//ここのProjをライトのプロジェクション（正射影行列)にする
			m = Matrix * m_LightView * m_LightProjectionMatrix;
			m = XMMatrixTranspose(m);
			cb.WLP = m;
			//ワールド、ライトビュー、射影行列、テクスチャ行列　を渡す
			m = Matrix * m_LightView * m_LightProjectionMatrix * m_ClipToUV;
			m = XMMatrixTranspose(m);
			cb.WLPT = m;
			cb.Front = XMFLOAT4(m_LightFront.x, m_LightFront.y, m_LightFront.z, 1.0f);
		}
		else
		{
			m = Matrix * m_LightViewUp * m_LightProjectionUp;
			m = XMMatrixTranspose(m);
			cb.WLP = m;
			m = Matrix * m_LightViewUp * m_LightProjectionUp * m_ClipToUV;
			m = XMMatrixTranspose(m);
			cb.WLPT = m;
			cb.Front = XMFLOAT4(m_LightFrontUP.x, m_LightFrontUP.y, m_LightFrontUP.z, 1.0f);
		}
		memcpy_s(data.pData, data.RowPitch, (void*)(&cb), sizeof(cb));
		deviceContext->Unmap(m_StaticMeshBuffer, 0);
	}
}

void CShadowMap::SetSkinMeshMatrix(const EShadowType Type, const UINT NumBone, const std::vector<BoneNameMatrixData>& m_BoneList, const XMMATRIX& Matrix)
{
	ID3D11DeviceContext* deviceContext = CRenderer::GetDeviceContext();
	if (!deviceContext)
	{
		return;
	}
	CSceneManager* sceneManager = CSceneManager::GetInstance();
	if (!sceneManager)
	{
		return;
	}
	CCamera* camera = sceneManager->GetCamera();
	if (!camera)
	{
		return;
	}
	Vector3 eye = camera->GetPosition();
	const XMMATRIX& view = camera->GetInvViewMatrix();
	const XMMATRIX& proj = camera->GetProjectionMatrix();

	D3D11_MAPPED_SUBRESOURCE data;
	//シェーダーにボーン行列を渡す
	if (SUCCEEDED(deviceContext->Map(m_SkeletalBoneDataBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &data)))
	{
		unsigned int i;
		ShaderBoneData boneData;
		for (i = 0; i < NumBone; i++)
		{
			boneData.Bones[i] = m_BoneList[i].Matrix;
		}
		memcpy_s(data.pData, data.RowPitch, (void*)&boneData, sizeof(ShaderBoneData));
		deviceContext->Unmap(m_SkeletalBoneDataBuffer, 0);
	}
	if (SUCCEEDED(deviceContext->Map(m_SkeletalMeshBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &data)))
	{
		BufferSkeletalMeshData cb;
		if (Type == EShadowType::NO_DOWN_SHADOW)
		{
			cb.LightView = XMMatrixTranspose(m_LightView);
			cb.ShaderShadowLightProjection = XMMatrixTranspose(m_LightProjectionMatrix);
			cb.View = XMMatrixTranspose(view);
			cb.Projection = XMMatrixTranspose(proj);
			//ワールド行列を渡す
			cb.W = XMMatrixTranspose(Matrix);
			cb.FogParam = CRenderer::GetFogConstant().FogParam;
			//ワールド、カメラ、射影行列　を渡す
			XMMATRIX m;
			m = Matrix * view*proj;
			m = XMMatrixTranspose(m);
			cb.WVP = m;
			//ワールド、ライトビュー、射影行列　を渡す
			//ここのProjをライトのプロジェクション（正射影行列)にする
			m = Matrix * m_LightView*m_LightProjectionMatrix;
			m = XMMatrixTranspose(m);
			cb.WLP = m;
			//ワールド、ライトビュー、射影行列、テクスチャ行列　を渡す
			m = Matrix * m_LightView * m_LightProjectionMatrix * m_ClipToUV;
			m = XMMatrixTranspose(m);
			cb.WLPT = m;
			cb.Front = XMFLOAT4(m_LightFront.x, m_LightFront.y, m_LightFront.z, 1.0f);
		}
		else
		{
			cb.LightView = XMMatrixTranspose(m_LightViewUp);
			cb.ShaderShadowLightProjection = XMMatrixTranspose(m_LightProjectionUp);
			cb.View = XMMatrixTranspose(view);
			cb.Projection = XMMatrixTranspose(proj);
			//ワールド行列を渡す
			cb.W = XMMatrixTranspose(Matrix);
			//ワールド、カメラ、射影行列　を渡す
			XMMATRIX m;
			m = Matrix * view * proj;
			m = XMMatrixTranspose(m);
			cb.WVP = m;
			//ワールド、ライトビュー、射影行列　を渡す
			//ここのProjをライトのプロジェクション（正射影行列)にする
			m = Matrix * m_LightViewUp * m_LightProjectionUp;
			m = XMMatrixTranspose(m);
			cb.WLP = m;
			cb.FogParam = CRenderer::GetFogConstant().FogParam;
			//ワールド、ライトビュー、射影行列、テクスチャ行列　を渡す
			m = Matrix * m_LightViewUp * m_LightProjectionUp * m_ClipToUV;
			m = XMMatrixTranspose(m);
			cb.WLPT = m;
			cb.Front = XMFLOAT4(m_LightFrontUP.x, m_LightFrontUP.y, m_LightFrontUP.z, 1.0f);
		}
		memcpy_s(data.pData, data.RowPitch, (void*)(&cb), sizeof(cb));
		deviceContext->Unmap(m_SkeletalMeshBuffer, 0);
	}
	//頂点インプットレイアウトをセット
	deviceContext->IASetInputLayout(m_SkeletalVertexLayout);
}

void CShadowMap::SetNormalModelMaterial(const XMFLOAT4& Ambient, const XMFLOAT4& Diffuse, const XMFLOAT4& Specular, const XMFLOAT4& Alpha)
{
	ID3D11DeviceContext* deviceContext = CRenderer::GetDeviceContext();
	if (!deviceContext)
	{
		return;
	}
	D3D11_MAPPED_SUBRESOURCE data;
	if (SUCCEEDED(deviceContext->Map(m_MaterialBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &data)))
	{
		BufferMaterial sg;
		sg.vAmbient = Ambient;//アンビエントををシェーダーに渡す
		sg.vDiffuse = Diffuse;//ディフューズカラーをシェーダーに渡す
		sg.vSpecular = Specular;//スペキュラーをシェーダーに渡す
		sg.vAlpha = Alpha;
		memcpy_s(data.pData, data.RowPitch, (void*)&sg, sizeof(BufferMaterial));
		deviceContext->Unmap(m_MaterialBuffer, 0);
	}
}

void CShadowMap::SetNormalModelCameraLight(const XMMATRIX& Matrix)
{
	ID3D11DeviceContext* deviceContext = CRenderer::GetDeviceContext();
	if (!deviceContext)
	{
		return;
	}
	CSceneManager* sceneManager = CSceneManager::GetInstance();
	if (!sceneManager)
	{
		return;
	}
	CCamera* camera = sceneManager->GetCamera();
	if (!camera)
	{
		return;
	}
	Vector3 eye = camera->GetPosition();
	const XMMATRIX& view = camera->GetInvViewMatrix();
	const XMMATRIX& proj = camera->GetProjectionMatrix();
	D3D11_MAPPED_SUBRESOURCE data;
	if (SUCCEEDED(deviceContext->Map(m_MatrixAndFogBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &data)))
	{
		ShaderMatrixAndFogData cb;
		//ワールド行列を渡す
		cb.WorldMatrix = XMMatrixTranspose(Matrix);
		//ワールド、カメラ、射影行列　を渡す
		cb.ProjectionMatrix = XMMatrixTranspose(proj);
		cb.ViewMatrix = XMMatrixTranspose(view);
		cb.CameraPos = XMFLOAT4(eye.x, eye.y, eye.z, 0.0f);
		cb.FogParam = CRenderer::GetFogConstant().FogParam;
		memcpy_s(data.pData, data.RowPitch, (void*)(&cb), sizeof(cb));
		deviceContext->Unmap(m_MatrixAndFogBuffer, 0);
	}
	if (SUCCEEDED(deviceContext->Map(m_DirectionalLightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &data)))
	{
		CDirectionalLight* directionalLight = directionalLight->GetInstance();
		if (!directionalLight)
		{
			return;
		}
		const LIGHT light = directionalLight->GetLight();
		memcpy_s(data.pData, data.RowPitch, (void*)(&light), sizeof(light));
		deviceContext->Unmap(m_DirectionalLightBuffer, 0);
	}
}

void CShadowMap::SetSkinMeshMaterial(const XMFLOAT4& Ambient, const XMFLOAT4& Diffuse, const XMFLOAT4& Specular, const XMFLOAT4& Alpha)
{
	ID3D11DeviceContext* deviceContext = CRenderer::GetDeviceContext();
	if (!deviceContext)
	{
		return;
	}
	D3D11_MAPPED_SUBRESOURCE data;
	if (SUCCEEDED(deviceContext->Map(m_SkeletalMaterialBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &data)))
	{
		BufferMaterial sg;
		sg.vAmbient = Ambient;//アンビエントををシェーダーに渡す
		sg.vDiffuse = Diffuse;//ディフューズカラーをシェーダーに渡す
		sg.vSpecular = Specular;//スペキュラーをシェーダーに渡す
		sg.vAlpha = Alpha;//アルファ値をシェーダーに渡す
		memcpy_s(data.pData, data.RowPitch, (void*)&sg, sizeof(BufferMaterial));
		deviceContext->Unmap(m_SkeletalMaterialBuffer, 0);
	}
}

void CShadowMap::SetDepthViewPort()
{
	ID3D11DeviceContext* deviceContext = CRenderer::GetDeviceContext();
	if (!deviceContext)
	{
		return;
	}
	//ビューポートの設定
	D3D11_VIEWPORT vp;
	vp.Width = (float)DEPTHTEX_WIDTH;
	vp.Height = (float)DEPTHTEX_HEIGHT;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	deviceContext->RSSetViewports(1, &vp);
	//レンダーターゲットを深度テクスチャーに変更
	ID3D11RenderTargetView* m_DepthMapTexRTV = CRenderer::GetDepthMapTexRTV();//深度マップテクスチャーのRTV
	ID3D11DepthStencilView* m_DepthMapDSTexDSV = CRenderer::GetDepthMapDSTexDSV();//深度マップテクスチャー用DSのDSV	
	deviceContext->OMSetRenderTargets(1, &m_DepthMapTexRTV, m_DepthMapDSTexDSV);
	////このレンダーターゲットをクリア
	float ClearColor[4] = { 0,0,0,1 };// クリア色作成　RGBAの順
	deviceContext->ClearRenderTargetView(m_DepthMapTexRTV, ClearColor);//画面クリア
	deviceContext->ClearDepthStencilView(m_DepthMapDSTexDSV, D3D11_CLEAR_DEPTH, 1.0f, 0);//深度バッファクリア
}

void CShadowMap::SetNormalViewPort()
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
	//ビューポートの設定

	D3D11_VIEWPORT vp;
	vp.Width = system->GetCurrentScreenWidth();
	vp.Height = system->GetCurrentScreenHeight();
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	deviceContext->RSSetViewports(1, &vp);
	//レンダーターゲットを通常（バックバッファー）に戻す
	ID3D11RenderTargetView* m_BackBufferTexRTV = CRenderer::GetBackBufferTexRTV();//深度マップテクスチャーのRTV
	ID3D11DepthStencilView* m_BackBufferDSTexDSV = CRenderer::GetBackBufferDSTexDSV();//深度マップテクスチャー用DSのDSV	
	deviceContext->OMSetRenderTargets(1, &m_BackBufferTexRTV, m_BackBufferDSTexDSV);
	//////画面クリア
	float ClearColor[4] = { 0,0,0,1 };// クリア色作成　RGBAの順
	deviceContext->ClearRenderTargetView(m_BackBufferTexRTV, ClearColor);//画面クリア
	deviceContext->ClearDepthStencilView(m_BackBufferDSTexDSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);//深度バッファクリア		
																																   //１パス目で作った深度テクスチャーをシェーダーに渡す
	ID3D11ShaderResourceView* m_DepthMapTexSRV = CRenderer::GetDepthMapTexSRV();
	//ピクセルシェーダーの[1]番目に深度テクスチャをセット
	deviceContext->PSSetShaderResources(1, 1, &m_DepthMapTexSRV);
}