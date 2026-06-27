#pragma once

struct AnimationData
{
	float AnimationSpeed = 1.0f;//アニメーションの再生速度
	bool IsLoop = false;//ループするかどうか
	AnimationData() {};
	AnimationData(const float InAnimationSpeed, const  bool InIsLoop)
		: AnimationSpeed(InAnimationSpeed), IsLoop(InIsLoop) {}
};