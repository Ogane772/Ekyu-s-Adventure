#pragma once
#include "../../Framework/Calculation/Vector.h"
using namespace FrameWork;
enum class ETextureType;

//エフェクトの種類
enum class EEffectType
{
	ACORN,
	WALL_HIT,
	DASH,
	JUMP,
	STUN,
	ITEM_GET,
	SMOKE,
	REVIVAL,
	BOM,
	EXPLOSION,
	EXPLOSION2,
	HEART,
	ATTACK_HIT,
	ATTACK_HIT2,
	ATTACK_HIT3,
	PLAYER_HIT,
	GROUND_HIT,
	MAX,
};

//エフェクトに必要なデータ
struct EffectData
{
	bool IsLoop = false;//ループするか
	ETextureType Type = ETextureType();//エフェクトに使用するテクスチャの種類
	int AnimationPattern = 0;//エフェクトに使用する画像のアニメーションパターン数
	float AnimationTime = 0;//エフェクトの再生時間(分割した1枚の画像の表示時間となる)
	float EndTime = 0;//エフェクトの再生が終わるまでの時間
	int WidthPattern = 0;//エフェクトに使用する画像の横のパターン数
	int HeightPattern = 0;//エフェクトに使用する画像の縦のパターン数
	bool IsEnableDepth = false;//深度を使用した描画を行うか
	bool IsRandomStartTime = false;//エフェクト再生時に0～アニメーション終了時間の間を開始時間とする

	EffectData() {};
	EffectData(const bool InIsLoop, const ETextureType InType, const float InAnimationTime, const int InWidthPattern, const int InHeightPattern, const bool InIsEnableDepth, const bool InIsRandomStartTime = false)
	{
		IsLoop = InIsLoop;
		Type = InType;
		AnimationPattern = InWidthPattern * InHeightPattern;
		AnimationTime = InAnimationTime;
		EndTime = AnimationTime * AnimationPattern;
		WidthPattern = InWidthPattern;
		HeightPattern = InHeightPattern;
		IsEnableDepth = InIsEnableDepth;
		IsRandomStartTime = InIsRandomStartTime;
	}
};

//実際にエフェクトに使用するデータ
struct EffectParameter
{
	Vector3 Position;
	float Size = 0;//エフェクトのビルボードの大きさ
	float CountTime = 0;//アニメ経過時間
	EffectData EffectData;

	ETextureType GetTextureType() const { return EffectData.Type; }
	bool IsLoop() const { return EffectData.IsLoop; }
	bool IsEndTime() const { return CountTime >= EffectData.EndTime; }
	void ResetCountTime() { CountTime = 0.0f; }
	float GetAnimationTime() const { return EffectData.AnimationTime; }
	float GetAnimationEndTime() const { return EffectData.EndTime; }
	int GetAnimationPattern() const { return EffectData.AnimationPattern; }
	int GetWidthPattern() const { return EffectData.WidthPattern; }
	int GetHeightPattern() const { return EffectData.HeightPattern; }
	bool IsEnableDepth() const { return EffectData.IsEnableDepth; }
	bool IsRandomStartTime() const { return EffectData.IsRandomStartTime; }

	EffectParameter() {};
};