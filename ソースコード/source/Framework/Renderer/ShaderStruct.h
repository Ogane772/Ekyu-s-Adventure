#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <string>
//複数間で使用するシェーダーに必要な構造体
using namespace DirectX;
#define MAX_BONES (20)//シェーダーに渡すボーンの最大数

// 頂点構造体
struct VERTEX_3D
{
	XMFLOAT3 Position = XMFLOAT3();
	XMFLOAT3 Normal = XMFLOAT3();
	XMFLOAT4 Diffuse = XMFLOAT4();
	XMFLOAT2 TexCoord = XMFLOAT2();
};

// 色構造体
struct COLOR
{
	COLOR() {}
	COLOR(float _r, float _g, float _b, float _a)
	{
		r = _r;
		g = _g;
		b = _b;
		a = _a;
	}

	float r;
	float g;
	float b;
	float a;
};

// マテリアル構造体
struct MATERIAL
{
	COLOR		Ambient;
	COLOR		Diffuse;
	COLOR		Specular;
	COLOR		Emission;
	COLOR		Alpha;//透明度.Rのみ使用
	float		Shininess;
	float		Dummy[3];//16bit境界用
};

struct ShaderNormalMapData
{
	XMFLOAT4 SpecularPower;//xのみ使用 スペキュラの鋭さ
	XMFLOAT4 NormalPower;//xのみ使用　法線マップの強さ
};

struct FOG_CONSTANT
{
	XMFLOAT4 FogParam;//x = 1ならフォグを行う y = near z = far 
};

// マテリアル構造体
struct DX11_MODEL_MATERIAL
{
	MATERIAL		Material;
};

//Bufferに渡すマテリアル情報
struct BufferMaterial
{
	XMFLOAT4 vAmbient;//アンビエント光
	XMFLOAT4 vDiffuse;//ディフューズ色
	XMFLOAT4 vSpecular;//鏡面反射
	XMFLOAT4 vAlpha;//透明度 xのみ使用
};

struct LIGHT
{
	XMFLOAT4	Direction;//位置
	COLOR		Diffuse;//光の色
	COLOR		Ambient;//光の当たっている場所の色
};

//影の情報に使用するライト情報
struct ShaderShadowLightProjection
{
	float View_Width;
	float View_Height;
	float FarZ;
};

struct ShaderMatrixAndFogData
{
	XMMATRIX WorldMatrix;
	XMMATRIX ViewMatrix;
	XMMATRIX ProjectionMatrix;
	XMFLOAT4 CameraPos;
	XMFLOAT4 FogParam;
};

//ボーンの行列、名前情報
struct BoneNameMatrixData
{
	XMMATRIX Matrix;
	XMMATRIX OffsetMatrix;
	XMMATRIX AnimationMatrix;
	std::string Name;
};

//シェーダーに渡すボーン行列
struct ShaderBoneData
{
	XMMATRIX Bones[MAX_BONES];
};