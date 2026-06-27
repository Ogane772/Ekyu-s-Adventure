#pragma once
#include "../../../Framework/GameObject/GameObject.h"

class CSphere;

class CAcorn : public CGameObject
{
private:
	std::weak_ptr<ID3D11ShaderResourceView> m_Texture;
	UINT m_EffectId = 0;
	std::unique_ptr<CSphere> m_Sphere;
	
public:
	CAcorn(const Vector3 SetPos);
	virtual void Update(float DeltaTime)override;
	virtual void UnInit()override;
	virtual void Draw()override;
	//生成時一度だけメッシュの押し出し処理を行う
	void InitBodyCollisionCheck();
	virtual CSphere* GetSphere() const override;
	virtual void SetPosition(const Vector3& SetPos)override;
	virtual void SetDestroyEdit()override;
};
