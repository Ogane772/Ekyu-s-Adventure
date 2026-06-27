#include "2DPolygon.h"
#include "../../Renderer/Renderer.h"
#include "../../GameObject/GameObject.h"
#include "../../TextureLoader/TextureLoader.h"
#include "../../System/SystemContext.h"

#ifdef _DEBUG
#ifndef DBG_NEW
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#define new DBG_NEW
#endif
#endif

constexpr float SCORE_SPRITE_SIZE = 40.0f;

C2DPolygon::C2DPolygon()
{
	Init();
}

C2DPolygon::C2DPolygon(const ETextureType Type)
{
	Init();
	SetTexture(Type);
	
}

C2DPolygon::~C2DPolygon()
{	
	if (m_VertexBuffer)
	{
		m_VertexBuffer->Release();
		m_VertexBuffer = nullptr;
	}

	if (CTextureLoader* textureLoader = CTextureLoader::GetInstance())
	{
		textureLoader->Release(m_Texture);
	}
}

void C2DPolygon::Init()
{
	m_Vertex[0] = { XMFLOAT3(-0.5, 0.5, 0), XMFLOAT3(0.0f, 1.0f, 0.0f),  XMFLOAT4(1.0f,1.0f,1.0f,1.0f),   XMFLOAT2(1.0f,1.0f) };
	m_Vertex[1] = { XMFLOAT3(0.5, 0.5, 0),  XMFLOAT3(0.0f, 1.0f, 0.0f),  XMFLOAT4(1.0f,1.0f,1.0f,1.0f),   XMFLOAT2(1.0f,1.0f) };
	m_Vertex[2] = { XMFLOAT3(-0.5, -0.5, 0), XMFLOAT3(0.0f, 1.0f, 0.0f),  XMFLOAT4(1.0f,1.0f,1.0f,1.0f),  XMFLOAT2(1.0f,1.0f) };
	m_Vertex[3] = { XMFLOAT3(0.5, -0.5, 0),  XMFLOAT3(0.0f, 1.0f, 0.0f),  XMFLOAT4(1.0f,1.0f,1.0f,1.0f),  XMFLOAT2(1.0f,1.0f) };

	//頂点バッファ生成
	CD3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	D3D11_SUBRESOURCE_DATA sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.pSysMem = m_Vertex;
	if (ID3D11Device* device = CRenderer::GetDevice())
	{
		device->CreateBuffer(&bd, &sd, &m_VertexBuffer);
	}
}

void C2DPolygon::Draw(std::weak_ptr<ID3D11ShaderResourceView>& Texture, const Vector2& TextureSize)
{
	m_Vertex[0] = { XMFLOAT3(0.0f , 0.0f , 0) , XMFLOAT3(0.0f, 1.0f, 0.0f) , XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),XMFLOAT2(0.0f, 0.0f) };
	m_Vertex[1] = { XMFLOAT3(0.0f + TextureSize.x ,0.0f, 0) , XMFLOAT3(0.0f, 1.0f, 0.0f) , XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),XMFLOAT2(1.0f, 0.0f) };
	m_Vertex[2] = { XMFLOAT3(0.0f , 0.0f + TextureSize.y, 0) , XMFLOAT3(0.0f, 1.0f, 0.0f) , XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),XMFLOAT2(0.0f, 1.0f) };
	m_Vertex[3] = { XMFLOAT3(0.0f + TextureSize.x, 0.0f + TextureSize.y, 0) , XMFLOAT3(0.0f, 1.0f, 0.0f) , XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),XMFLOAT2(1.0f, 1.0f) };

	SetDevice(Texture);
}

void C2DPolygon::Draw(const float Alpha)
{
	CTextureLoader* textureLoader = CTextureLoader::GetInstance();
	if (!textureLoader)
	{
		return;
	}
	const int w = textureLoader->GetWidth(m_TextureType);
	const int h = textureLoader->GetHeight(m_TextureType);

	m_Vertex[0] = { XMFLOAT3(0.0f , 0.0f,        0) , XMFLOAT3(0.0f, 1.0f, 0.0f) , XMFLOAT4(1.0f, 1.0f,1.0f, Alpha),XMFLOAT2(0.0f, 0.0f) };
	m_Vertex[1] = { XMFLOAT3(0.0f + w ,0.0f,     0) , XMFLOAT3(0.0f, 1.0f, 0.0f) , XMFLOAT4(1.0f, 1.0f,1.0f, Alpha),XMFLOAT2(1.0f, 0.0f) };
	m_Vertex[2] = { XMFLOAT3(0.0f , 0.0f + h,    0) , XMFLOAT3(0.0f, 1.0f, 0.0f) , XMFLOAT4(1.0f, 1.0f,1.0f, Alpha),XMFLOAT2(0.0f, 1.0f) };
	m_Vertex[3] = { XMFLOAT3(0.0f + w, 0.0f + h, 0) , XMFLOAT3(0.0f, 1.0f, 0.0f) , XMFLOAT4(1.0f, 1.0f,1.0f, Alpha),XMFLOAT2(1.0f, 1.0f) };

	SetDevice(m_Texture);
}

void C2DPolygon::Draw(const SpriteParameter& SpriteParam)
{
	CTextureLoader* textureLoader = CTextureLoader::GetInstance();
	if (!textureLoader)
	{
		return;
	}
	const int w = textureLoader->GetWidth(m_TextureType);
	const int h = textureLoader->GetHeight(m_TextureType);
	const float tw = (float)w;
	const float th = (float)h;
	float u[2], v[2];
	const Vector2& textureSize = SpriteParam.TextureSize;
	u[0] = (float)textureSize.x / w;
	v[0] = (float)textureSize.y / h;
	u[1] = (float)(textureSize.x + tw) / w;
	v[1] = (float)(textureSize.y + th) / h;

	float px[4], py[4];
	const Vector2& position = GetConversionDisplayScale(SpriteParam.Position);
	const Vector2& scale = GetConversionDisplayScale(SpriteParam.Scale);
	const float rotation = XMConvertToRadians(SpriteParam.Rotation);
	const Vector2& center = SpriteParam.TextureCenterPosition;
	const float alpha = SpriteParam.Alpha;
	const Vector3& color = SpriteParam.Color;

	px[0] = -center.x * scale.x * cosf(rotation) - -center.y * scale.y * sinf(rotation);
	py[0] = -center.x * scale.x * sinf(rotation) + -center.y * scale.y * cosf(rotation);
	px[1] = (-center.x + tw) * scale.x * cosf(rotation) - -center.y * scale.y * sinf(rotation);
	py[1] = (-center.x + tw) * scale.x * sinf(rotation) + -center.y * scale.y * cosf(rotation);
	px[2] = -center.x * scale.x * cosf(rotation) - (-center.y + th) * scale.y * sinf(rotation);
	py[2] = -center.x * scale.x * sinf(rotation) + (-center.y + th) * scale.y * cosf(rotation);
	px[3] = (-center.x + tw) * scale.x * cosf(rotation) - (-center.y + th) * scale.y * sinf(rotation);
	py[3] = (-center.x + tw) * scale.x * sinf(rotation) + (-center.y + th) * scale.y * cosf(rotation);

	m_Vertex[0] = { XMFLOAT3(px[0] + position.x + center.x - 0.5f, py[0] + position.y + center.y - 0.5f, 0.0f) , XMFLOAT3(0.0f, 1.0f, 0.0f) , XMFLOAT4(color.x, color.y, color.z, alpha),XMFLOAT2(u[0], v[0]) };
	m_Vertex[1] = { XMFLOAT3(px[1] + position.x + center.x - 0.5f, py[1] + position.y + center.y - 0.5f, 0.0f) , XMFLOAT3(0.0f, 1.0f, 0.0f) , XMFLOAT4(color.x, color.y, color.z, alpha),XMFLOAT2(u[1], v[0]) };
	m_Vertex[2] = { XMFLOAT3(px[2] + position.x + center.x - 0.5f, py[2] + position.y + center.y - 0.5f, 0.0f) , XMFLOAT3(0.0f, 1.0f, 0.0f) , XMFLOAT4(color.x, color.y, color.z, alpha),XMFLOAT2(u[0], v[1]) };
	m_Vertex[3] = { XMFLOAT3(px[3] + position.x + center.x - 0.5f, py[3] + position.y + center.y - 0.5f, 0.0f) , XMFLOAT3(0.0f, 1.0f, 0.0f) , XMFLOAT4(color.x, color.y, color.z, alpha),XMFLOAT2(u[1], v[1]) };

	SetDevice(m_Texture);
}

void C2DPolygon::DrawNumber(const Vector2& Position, const Vector2& TexturePosition, const Vector2& TextureSize)
{
	CTextureLoader* textureLoader = CTextureLoader::GetInstance();
	if (!textureLoader)
	{
		return;
	}
	const float w = (float)textureLoader->GetWidth(m_TextureType);
	const float h = (float)textureLoader->GetHeight(m_TextureType);

	const float v0 = (float)TexturePosition.y / h;
	const float v1 = (float)TexturePosition.y / h + (float)TextureSize.y / h;
	const float u0 = (float)TexturePosition.x / w;
	const float u1 = (float)TexturePosition.x / w + (float)TextureSize.x / w;

	const Vector2& position = GetConversionDisplayScale(Position);
	const Vector2& scale = GetConversionDisplayScale(Vector2::One());

	m_Vertex[0] = { XMFLOAT3(position.x - 0.5f,                  position.y - 0.5f,0.0f) , XMFLOAT3(0.0f, 1.0f, 0.0f) , XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),XMFLOAT2(u0 ,v0) };
	m_Vertex[1] = { XMFLOAT3(position.x + (TextureSize.x * scale.x)- 0.5f , position.y - 0.5f, 0.0f) , XMFLOAT3(0.0f, 1.0f, 0.0f) , XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),XMFLOAT2(u1 ,v0) };
	m_Vertex[2] = { XMFLOAT3(position.x - 0.5f,                  position.y + (TextureSize.y * scale.y)- 0.5f, 0.0f) , XMFLOAT3(0.0f, 1.0f, 0.0f) , XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),XMFLOAT2(u0, v1) };
	m_Vertex[3] = { XMFLOAT3(position.x + (TextureSize.x * scale.x) - 0.5f,  position.y + (TextureSize.y * scale.y) - 0.5f, 0.0f) , XMFLOAT3(0.0f, 1.0f, 0.0f) , XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),XMFLOAT2(u1 ,v1) };

	SetDevice(m_Texture);
}

void C2DPolygon::DrawAnimation(const SpriteAnimationParameter& AnimeParam)
{
	CTextureLoader* textureLoader = CTextureLoader::GetInstance();
	if (!textureLoader)
	{
		return;
	}
	int applyAnimationPattern = 0;
	if (AnimeParam.MaxAnimationTime > 0.0f)
	{
		const float onePatternAnimationTime = AnimeParam.MaxAnimationTime / AnimeParam.MaxAnimationPattern;
		const int rate = static_cast<int>(AnimeParam.CountAnimationTime / onePatternAnimationTime);
		const int pattern = AnimeParam.MaxAnimationPattern;
		applyAnimationPattern = rate % pattern;
		if (applyAnimationPattern < 0)
		{
			applyAnimationPattern += pattern;
		}
	}

	const float textureWidth = (float)textureLoader->GetWidth(m_TextureType);
	const float textureHeight = (float)textureLoader->GetHeight(m_TextureType);
	Vector2 defaultTextureAnimationSize;
	defaultTextureAnimationSize.x = textureWidth / AnimeParam.WidthPattern;
	defaultTextureAnimationSize.y = textureHeight / AnimeParam.HeightPattern;
	Vector2 applyTextureSize;
	applyTextureSize.x = defaultTextureAnimationSize.x * (applyAnimationPattern % AnimeParam.WidthPattern);
	applyTextureSize.y = defaultTextureAnimationSize.y * (applyAnimationPattern / AnimeParam.WidthPattern);

	//UV座標指定
	const float u0 = applyTextureSize.x/ textureWidth;
	const float v0 = applyTextureSize.y / textureHeight;
	const float u1 = applyTextureSize.x / textureWidth + defaultTextureAnimationSize.x / textureWidth;
	const float v1 = applyTextureSize.y / textureHeight + defaultTextureAnimationSize.y / textureHeight;
	float px[4], py[4];;
	const Vector2& position = GetConversionDisplayScale(AnimeParam.Position);
	const Vector2& scale = GetConversionDisplayScale(AnimeParam.Scale);
	const float rotation = 0.0f;

	px[0] = position.x * scale.x * cosf(rotation) - position.y * scale.y * sinf(rotation);
	py[0] = position.x * scale.x * sinf(rotation) + position.y * scale.y * cosf(rotation);
	px[1] = (position.x + defaultTextureAnimationSize.x) * scale.x * cosf(rotation) - position.y * scale.y * sinf(rotation);
	py[1] = (position.x + defaultTextureAnimationSize.x) * scale.x * sinf(rotation) + position.y * scale.y * cosf(rotation);
	px[2] = position.x * scale.x * cosf(rotation) - (position.y + defaultTextureAnimationSize.y) * scale.y * sinf(rotation);
	py[2] = position.x * scale.x * sinf(rotation) + (position.y + defaultTextureAnimationSize.y) * scale.y * cosf(rotation);
	px[3] = (position.x + defaultTextureAnimationSize.x) * scale.x * cosf(rotation) - (position.y + defaultTextureAnimationSize.y) * scale.y * sinf(rotation);
	py[3] = (position.x + defaultTextureAnimationSize.x) * scale.x * sinf(rotation) + (position.y + defaultTextureAnimationSize.y) * scale.y * cosf(rotation);

	m_Vertex[0] = { XMFLOAT3(px[0], py[0], 0.0f) , XMFLOAT3(0.0f, 1.0f, 0.0f) , XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),XMFLOAT2(u0,v0) };
	m_Vertex[1] = { XMFLOAT3(px[1], py[1], 0.0f) , XMFLOAT3(0.0f, 1.0f, 0.0f) , XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),XMFLOAT2(u1,v0) };
	m_Vertex[2] = { XMFLOAT3(px[2], py[2], 0.0f) , XMFLOAT3(0.0f, 1.0f, 0.0f) , XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),XMFLOAT2(u0,v1) };
	m_Vertex[3] = { XMFLOAT3(px[3], py[3], 0.0f) , XMFLOAT3(0.0f, 1.0f, 0.0f) , XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),XMFLOAT2(u1,v1) };

	SetDevice(m_Texture);
}

void C2DPolygon::ScoreDraw(const Vector2& Position, int Score, const int Digit)
{
	//カンスト用の最大数値
	int countStopScore = 1;

	for (int i = 0; i < Digit; i++)
	{
		countStopScore *= 10;
	}

	if (Score >= countStopScore)
	{
		Score = countStopScore - 1;
	}

	int number = 0;
	for (int i = 0; i < Digit; i++)
	{
		//右端の座標
		number = Score % 10;//1の位
		Score /= 10;

		Number_Draw(Vector2(Position.x + SCORE_SPRITE_SIZE*(Digit - (i + 1)), Position.y), number);
	}
}

void C2DPolygon::Number_Draw(const Vector2& Position, const int Number)
{
	if (Number < 0 || Number >= 10)
	{
		return;
	}
	DrawNumber(Position, Vector2((SCORE_SPRITE_SIZE * Number) + 1.0f, 0.0f), Vector2(SCORE_SPRITE_SIZE - 1.0f, SCORE_SPRITE_SIZE));
}

void C2DPolygon::SetDevice(std::weak_ptr<ID3D11ShaderResourceView>& Texture)
{
	ID3D11DeviceContext* device = CRenderer::GetDeviceContext();
	if (!device)
	{
		return;
	}
	device->UpdateSubresource(m_VertexBuffer, 0, nullptr, &m_Vertex, 0, 0);
	// 頂点バッファ設定
	CRenderer::SetVertexBuffers(m_VertexBuffer);
	//テクスチャ設定
	CRenderer::SetTexture(Texture);

	//トポロジ設定
	device->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	//ポリゴン描画（頂点数を指定）第二引数　スタートする頂点
	device->Draw(4, 0);
}

Vector2 C2DPolygon::GetConversionDisplayScale(const Vector2& ConversionVector) const
{
	CSystemContext* system = CSystemContext::GetInstance();
	if (!system)
	{
		return Vector2();
	}
	return  Vector2(ConversionVector.x * system->GetWidthDisplayScale(), ConversionVector.y * system->GetHeightDisplayScale());
}

void C2DPolygon::SetTexture(const ETextureType Type)
{
	CTextureLoader* textureLoader = CTextureLoader::GetInstance();
	if (!textureLoader)
	{
		return;
	}
	m_TextureType = Type;
	m_Texture = textureLoader->Load(Type);
}