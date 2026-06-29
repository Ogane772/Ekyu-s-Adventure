#pragma once
//敵が落とすアイテムの種類
enum class EItemDropType
{
	NO_ITEM,
	ACORN,
	HEART,
	STAR,
};
//敵が落とすアイテムのパラメータ
struct ItemDropData
{
	EItemDropType ItemDropType;
	int Percent = 0;//設定したアイテムが落ちる確率
};
