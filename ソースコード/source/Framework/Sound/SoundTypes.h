#pragma once
#include <string>

enum class ESoundIndex
{
	FIRST_STAGE_BGM,
	BOSS_BGM,
	TUTORIAL_BGM,
	STAGESELECT_BGM,
	BOM_SE,
	BOM_START,
	BOM_THROW,
	DAMAGE,
	DIVE,
	ENEMY_DEATH,
	ItemDropData,
	JUMP_END,
	STUN,
	PLAYER_DAMAGE,
	TAIL_HIT,
	WALL_HIT,
	SWING,
	JUMP_HIT,
	BOSS_JUMP_HIT,
	FINAL_JUMP,
	DIVE_HIT,
	SPIN_JUMP,
	TAIL_ATTACK,
	HIPDROP_START,
	HIPDROP_END,
	WALL_SLIP,
	MOVE_CHANGE_JUMP,
	CROUCHING_JUMP,
	DASH_JUMP,
	FIRST_JUMP,
	SECOND_JUMP,
	DOWN,
	DOWN2,
	DEATH_MOVE,
	ACORN_HIT,
	HEAL,
	CLIFF_HIT,
	CLIFF_UP,
	STAR_MOVE,
	DOOR_OPEN,
	GETSTAR,
	SLIME_DISCOVERY,
	CLEAR_DOOR,
	SLIME_WALK,
	SLIME_DASH,
	RESPAWN,
	DORAMING,
	GORILLA_JUMP,
	GORILLA_ATTACK_STANDBY,
	GORILLA_WALL_HIT,
	GORILLA_ATTACK_HEAL,
	GORILLA_JUMP_ATTACK,
	GORILLA_JUMP_LOOP,
	HIT_END,
	BOSS_DEATH,
	SELECT_STAGE,
	SELECT_FRAME,
	FADEOUT,
	HIPDROP_JUMP,
	DEATH,
	WALL_JUMP,
	FOOTSTEPS_NORMAL,
	FOOTSTEPS_BRIDGE,
	FOOTSTEPS_ROCK,
	THORNMAN_DEATH,
	THORNMAN_DISCOVERY,
	IN_PAUSE,
	OUT_PAUSE,
	PAUSE_CURSOR_MOVE,
	PAUSE_MENU_OK,
	PAUSE_MENU_BACK,
	PAUSE_OPTION_CHANGE,
	GROUND_SLIP,
	NONE,
	MAX
};

enum class ESoundType
{
	NONE,
	BGM,
	SE,
};

//サウンド再生に必要なデータ
struct SoundData 
{
	std::string FilePath;//音声ファイルのパス
	ESoundType Type = ESoundType::NONE;//音声の種類
	float Volume = 1.0f;//音量。1が基準となる
	bool IsLoop = false;//ループを行うか
	bool IsCache = false;//キャッシュを行うか(真の場合、再生を行う際に作成済みのものを使用する)

	SoundData() {};
	SoundData(const std::string& InFilePath, const ESoundType InType, const float InVolume = 1.0f, const bool InLoop = false, const bool InCache = false)
	{
		FilePath = InFilePath;
		Type = InType;
		Volume = InVolume;
		IsLoop = InLoop;
		IsCache = InCache;
	}
};