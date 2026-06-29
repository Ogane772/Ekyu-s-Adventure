#pragma once
//UI等の画像を表示するクラス
#include <memory>
#include "../../Renderer/ShaderStruct.h"
#include "../../Calculation/Vector.h"
enum class ETextureType;
using namespace FrameWork;

struct SpriteParameter
{
	Vector2 Position = Vector2::Zero();
	Vector2 TextureSize = Vector2::One();
	Vector2 TextureCenterPosition = Vector2::Zero();
	Vector2 Scale = Vector2::One();
	float Rotation = 0;
	float Alpha = 1.0f;
	Vector3 Color = Vector3::One();

	SpriteParameter() {};

	SpriteParameter(const Vector2& InPosition, const Vector2& InScale = Vector2::One(), const float InRotation = 0.0f, const float InAlpha = 1.0f, const Vector3& InColor = Vector3::One())
		: Position(InPosition), Scale(InScale), Rotation(InRotation), Alpha(InAlpha), Color(InColor) 
	{
	}
};

struct SpriteAnimationParameter
{
	Vector2 Position = Vector2::Zero();
	Vector2 Scale = Vector2::One();
	int MaxAnimationPattern = 0;
	int WidthPattern = 0;//エフェクトに使用する画像の横のパターン数
	int HeightPattern = 0;//エフェクトに使用する画像の縦のパターン数
	float CountAnimationTime = 0;
	float MaxAnimationTime = 0;

	SpriteAnimationParameter() {};

	void Init(const Vector2& InScale, const int InWidthPattern, const int InHeightPattern)
	{
		Scale = InScale;
		MaxAnimationPattern = InWidthPattern * InHeightPattern;
		WidthPattern = InWidthPattern;
		HeightPattern = InHeightPattern;
	}

	void SetDrawParameter(const Vector2& InPosition, const float InCountAnimationTime, const float InMaxAnimationTime)
	{
		Position = InPosition;
		MaxAnimationTime = InMaxAnimationTime;
		CountAnimationTime = (std::min)(InCountAnimationTime, InMaxAnimationTime);
	}
};

class C2DPolygon
{
private:
	ID3D11Buffer* m_VertexBuffer = nullptr;//頂点バッファ
	std::weak_ptr<ID3D11ShaderResourceView> m_Texture;
	VERTEX_3D m_Vertex[4];
	ETextureType m_TextureType;

	void Init();
	void Number_Draw(const Vector2& Position, const int Number);
	void SetDevice(std::weak_ptr<ID3D11ShaderResourceView>& Texture);
	//引数で受け取った値を画面表示用に変換して返す
	Vector2 GetConversionDisplayScale(const Vector2& ConversionVector) const;

public:
	C2DPolygon();
	C2DPolygon(const ETextureType Type);
	~C2DPolygon();
	
	void Draw(std::weak_ptr<ID3D11ShaderResourceView>& Texture, const Vector2& TextureSize);
	void Draw(const float Alpha);
	void Draw(const SpriteParameter& SpriteParam);
	void DrawNumber(const Vector2& Position, const Vector2& TexturePosition, const Vector2& TextureSize);

	//テクスチャアニメーション
	void DrawAnimation(const SpriteAnimationParameter& AnimeParam);

	//数字描画用 Digitは桁数
	void ScoreDraw(const Vector2& Position, int Score, const int Digit);
	void SetTexture(const ETextureType Type);
};