#pragma once
//エフェクトクラス
#include <d3d11.h>
#include <memory>
#include "EffectTypes.h"
class CBillboard;
struct EffectParameter;

class CEffect
{
private:
	std::unique_ptr<CBillboard> m_Billboard;
	EffectParameter m_EffectParameter;
	bool IsActiveEffect = false;
	UINT m_Id = 0;
	bool m_IsCulling = false;

public:
	CEffect();
	~CEffect();
	static UINT GenerateID();

	UINT Active(const EffectParameter& InParameter);
	void Update(float DeltaTime);
	void Draw();
	void Deactivate();
	void UpdatePosition(const Vector3& Position);
	bool IsActive() const { return IsActiveEffect; }
	UINT GetID() const { return m_Id; }
};