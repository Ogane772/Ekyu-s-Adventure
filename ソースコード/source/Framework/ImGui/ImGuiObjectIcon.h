#pragma once
//エディタのドラッグドロップで作成するモデルタイプ
enum class EEditorCreateObjectType
{//セーブデータで参照しているので番号を変更しないこと
	NONE,
	ACORN,
	HEART,
	STAR,
	COMMON,
	NO_MESH,
	NO_CLIFFBOX,
	SLIME,
	MONKEY,
	THORNMAN,
	STAR_DOOR,
	PLAYER_START,
	CAMERA_HEIGHTLOCK_BOX,
	TUTORIAL_BOX,
	INDEX_MAX
};

//マップで選択しているオブジェクトタイプ
enum class EEditorSelectObjectType
{//セーブデータで参照しているので番号を変更しないこと
	NONE,
	ACORN,
	HEART,
	STAR,
	COMMON,
	SLIME,
	MONKEY,
	THORNMAN,
	STAR_DOOR,
	NO_CLIFFBOX,
	NO_MESH,
	CAMERA_HEIGHTLOCK_BOX,
	PLAYER_START,
	TUTORIAL_BOX,
	FOG,
};