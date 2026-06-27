#pragma once
#include "../../GameObject/GameObject.h"
#include "../../Renderer/ShaderStruct.h"
class CCamera;
struct EffectParameter;

class CBillboard
{
private:
	std::weak_ptr<ID3D11ShaderResourceView> m_Texture;
	ID3D11Buffer* m_VertexBuffer = nullptr;//頂点バッファ
	CCamera* m_Camera = nullptr;
	Vector3 m_Position;
	float m_Size = 0.0f;
	VERTEX_3D m_EffectVertex[4];
	ETextureType m_TextureType = ETextureType();

	void DrawBillboard(const Vector3& Position, const float Size, const bool IsEnableDepth);

public:
	void EffectInit();
	void BillboardInit(const ETextureType Type, const Vector3& Position, const float Size);
	~CBillboard();
	void Draw();
	void Draw(const EffectParameter& EffectParameter);
	void SetTexture(const ETextureType Type);
	void UpdatePosition(const Vector3& Position) { m_Position = Position; }
	ETextureType GetTextureType() const { return m_TextureType; }
};