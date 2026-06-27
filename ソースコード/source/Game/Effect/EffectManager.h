#pragma once
#include "../../Framework/GameObject/SceneObject.h"
#include "../../Framework/Calculation/Vector.h"
#include <unordered_map>
#include <vector>
#include <memory>
using namespace FrameWork;
class CEffect; 
enum class EEffectType;
struct EffectData;

class CEffectManager : public CSceneObject
{
private:
	std::unordered_map<EEffectType, EffectData> m_EffectDataList;
	std::vector<std::unique_ptr<CEffect>> m_EffectPoolList;

public:
	CEffectManager();
	UINT Active(const EEffectType Type, const Vector3 Position, const float Size);
	virtual void Update(float DeltaTime) override;
	virtual void Draw() override;
	void Stop(const UINT EffectID);
	void UpdatePosition(const UINT EffectID, const Vector3& Position);
};