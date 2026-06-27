#pragma once
enum class EItemDropType
{
	NO_ITEM,
	ACORN,
	HEART,
	STAR,
};

struct ItemDropData
{
	EItemDropType ItemDropType;
	int Percent = 0;//設定したアイテムが落ちる確率
};
