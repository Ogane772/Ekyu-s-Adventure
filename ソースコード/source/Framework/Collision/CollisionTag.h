#pragma once
#include "../Calculation/Vector.h"
using namespace FrameWork;

enum class ECollisionTagName
{//Editで読むので順番を変えないこと
	ALL_COLISION,//全ての判定を取る
	NO_WALL_HIT,//壁キック判定を行わない
	NO_MESH_HIT,//メッシュの当たり判定を行わない
	NO_CLIFF_HIT,//崖つかまりを行わない
	NO_LINE_HIT,//ラインヒットを行わない
	NO_COLLISION_HIT,//コリジョン判定を行わない
	NO_SLOPESLIP_HIT,//プレイヤーが滑り状態にならない
	OBJECT_COLISION,//プレイヤー以外のメッシュ判定を取る
};

enum class ECollisionTagCameraName
{//Editで読むので順番を変えないこと
	NONE,//カメラ補正を行わない
	BLOCK,//カメラをブロック
	CLEAR,//カメラからラインが当たったオブジェクトを透明にする
};

//足音判別用タグ
enum class ECollisionTagSteps
{//Editで読むので順番を変えないこと
	NORMAL,
	BRIDGE,
	ROCK,
};

struct CollisionMesh
{
	//三角形ポリゴンの頂点3つ
	Vector3 Point[3];
	//上記の頂点3つから法線を作成
	Vector3 Normal;
};

//メッシュやラインのコリジョンにおいて、どの種類のコリジョンで判定するか
enum class ECheckCollisionType
{
	PLAYER,
	OBJECT,
	CAMERA,
	EDITOR,
};