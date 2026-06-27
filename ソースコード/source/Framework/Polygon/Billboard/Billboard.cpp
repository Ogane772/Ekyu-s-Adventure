#include "Billboard.h"
#include "../../Renderer/Renderer.h"
#include "../../Scene/Scene.h"
#include "../../Scene/SceneManager.h"
#include "../../TextureLoader/TextureLoader.h"
#include "../../Camera/Camera.h"
#include "../../../Game/Effect/EffectTypes.h"

void CBillboard::EffectInit()
{
	CSceneManager* sceneManager = CSceneManager::GetInstance();
	if (!sceneManager)
	{
		return;
	}
	m_Camera = sceneManager->GetCamera();

	m_EffectVertex[0] = { XMFLOAT3(-0.5, 0.5, 0), XMFLOAT3(0.0f, 1.0f, 0.0f),  XMFLOAT4(1.0f,1.0f,1.0f,1.0f),  XMFLOAT2(1.0f,1.0f) };
	m_EffectVertex[1] = { XMFLOAT3(0.5, 0.5, 0),  XMFLOAT3(0.0f, 1.0f, 0.0f),  XMFLOAT4(1.0f,1.0f,1.0f,1.0f),  XMFLOAT2(1.0f,1.0f) };
	m_EffectVertex[2] = { XMFLOAT3(-0.5, -0.5, 0), XMFLOAT3(0.0f, 1.0f, 0.0f),  XMFLOAT4(1.0f,1.0f,1.0f,1.0f), XMFLOAT2(1.0f,1.0f) };
	m_EffectVertex[3] = { XMFLOAT3(0.5, -0.5, 0),  XMFLOAT3(0.0f, 1.0f, 0.0f),  XMFLOAT4(1.0f,1.0f,1.0f,1.0f), XMFLOAT2(1.0f,1.0f) };

	//頂点バッファ生成////////
	CD3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	D3D11_SUBRESOURCE_DATA sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.pSysMem = m_EffectVertex;
	CRenderer::GetDevice()->CreateBuffer(&bd, &sd, &m_VertexBuffer);

	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
}

void CBillboard::BillboardInit(const ETextureType Type, const Vector3& Position, const float Size)
{
	CSceneManager* sceneManager = CSceneManager::GetInstance();
	if (!sceneManager)
	{
		return;
	}
	m_Camera = sceneManager->GetCamera();
	m_TextureType = Type;
	if (CTextureLoader* textureLoader = CTextureLoader::GetInstance())
	{
		m_Texture = textureLoader->Load(Type);
	}
	m_Position = Position;
	m_Size = Size;

	VERTEX_3D vertex[4];
	ID3D11Device* m_D3DDevice = CRenderer::GetDevice();

	vertex[0].Position = XMFLOAT3(-0.5, 0.5, 0);
	vertex[0].Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
	vertex[0].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[0].TexCoord =XMFLOAT2(0.0f, 0.0f);

	vertex[1].Position = XMFLOAT3(0.5, 0.5, 0);
	vertex[1].Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
	vertex[1].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[1].TexCoord =XMFLOAT2(1.0f, 0.0f);

	vertex[2].Position = XMFLOAT3(-0.5, -0.5, 0);
	vertex[2].Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
	vertex[2].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[2].TexCoord =XMFLOAT2(0.0f, 1.0f);

	vertex[3].Position = XMFLOAT3(0.5, -0.5, 0);
	vertex[3].Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
	vertex[3].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[3].TexCoord =XMFLOAT2(1.0f, 1.0f);
		
	//頂点バッファ生成////////
	CD3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	D3D11_SUBRESOURCE_DATA sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.pSysMem = vertex;
	CRenderer::GetDevice()->CreateBuffer(&bd, &sd, &m_VertexBuffer);	
}

CBillboard::~CBillboard()
{
	if (CTextureLoader* textureLoader = CTextureLoader::GetInstance())
	{
		textureLoader->Release(m_Texture);
	}
	if (m_VertexBuffer)
	{
		m_VertexBuffer->Release();
		m_VertexBuffer = nullptr;
	}
}

void CBillboard::Draw()
{
	DrawBillboard(m_Position, m_Size, false);
}

void CBillboard::Draw(const EffectParameter& EffectParameter)
{
	CTextureLoader* textureLoader = CTextureLoader::GetInstance();
	if (!textureLoader)
	{
		return;
	}

	// 現在使用するエフェクトのパターン番号を求める
	int patternIndex = 0;
	if (EffectParameter.GetAnimationTime() > 0.0f)
	{
		const int rate = static_cast<int>(EffectParameter.CountTime / EffectParameter.GetAnimationTime());
		const int pattern = EffectParameter.GetAnimationPattern();
		patternIndex = rate % pattern;
		if (patternIndex < 0)
		{
			patternIndex += pattern;
		}
	}
	//使用するエフェクトのテクスチャの画像サイズ取得
	const ETextureType type = EffectParameter.GetTextureType();
	const float width = (float)textureLoader->GetWidth(type);
	const float height = (float)textureLoader->GetHeight(type);

	//現在のエフェクトパターンに使用するUVの開始位置/終了位置を求める
	const int widthPattern = EffectParameter.GetWidthPattern();
	const int heightPattern = EffectParameter.GetHeightPattern();
	//描画するエフェクト1枚の画像サイズを求める
	const float widthTextureSize = width / widthPattern;
	const float heightTextureSize = height / heightPattern;
	//画像のどのパターンを使用するかの開始位置を求める
	const float currentWidth = widthTextureSize * (patternIndex % widthPattern);
	const float currentHeight = heightTextureSize * (patternIndex / widthPattern);
	//実際に描画するためのUV座標を求める
	const float u0 = currentWidth / width;
	const float v0 = currentHeight / height;
	const float u1 = currentWidth / width + widthTextureSize / width;
	const float v1 = currentHeight / height + heightTextureSize / height;
	
	m_EffectVertex[0].TexCoord = XMFLOAT2(u0, v0);
	m_EffectVertex[1].TexCoord = XMFLOAT2(u1, v0);
	m_EffectVertex[2].TexCoord = XMFLOAT2(u0, v1);
	m_EffectVertex[3].TexCoord = XMFLOAT2(u1, v1);

	CRenderer::GetDeviceContext()->UpdateSubresource(m_VertexBuffer, 0, nullptr, &m_EffectVertex, 0, 0);

	DrawBillboard(EffectParameter.Position, EffectParameter.Size, EffectParameter.IsEnableDepth());
}

void CBillboard::DrawBillboard(const Vector3& Position, const float Size, const bool IsEnableDepth)
{
	if (!m_Camera)
	{
		return;
	}

	if (IsEnableDepth)
	{
		CRenderer::SetDepthEnable(false);
	}
	else
	{
		CRenderer::BillBoardBlendSet();
	}
	CRenderer::SetPolygonShader();

	XMFLOAT4X4 mtxView = m_Camera->GetViewMatrix();

	//カメラの拡大値をもらわないため0にする
	mtxView._14 = 0.0f;
	mtxView._24 = 0.0f;
	mtxView._34 = 0.0f;
	mtxView._44 = 1.0f;

	//XMMATRIXに変換
	const XMMATRIX cameraView = XMLoadFloat4x4(&mtxView);

	const XMMATRIX translationMatrix = XMMatrixTranslation(Position.x, Position.y, Position.z);
	const XMMATRIX scaleMatrix = XMMatrixScaling(Size, Size, Size);

	XMMATRIX world = scaleMatrix * cameraView * translationMatrix;

	CRenderer::SetWorldMatrix(world);
	// 頂点バッファ設定
	CRenderer::SetVertexBuffers(m_VertexBuffer);
	//テクスチャ設定
	CRenderer::SetTexture(m_Texture);
	//トポロジ設定
	CRenderer::GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	//ポリゴン描画（頂点数を指定）第二引数　スタートする頂点
	CRenderer::GetDeviceContext()->Draw(4, 0);

	if (IsEnableDepth)
	{
		CRenderer::SetDepthEnable(true);
	}
	else
	{
		CRenderer::BillBoardBlendEnd();
	}
}

void CBillboard::SetTexture(const ETextureType Type)
{
	m_TextureType = Type;
	if (CTextureLoader* textureLoader = CTextureLoader::GetInstance())
	{
		textureLoader->Release(m_Texture);
		m_Texture = textureLoader->Load(Type);
	}
}
