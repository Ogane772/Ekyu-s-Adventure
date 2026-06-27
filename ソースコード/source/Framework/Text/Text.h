#pragma once
//画面上にデバッグ用のテキストを描画するクラス
//利便性のためスタティッククラス
#define _CRT_SECURE_NO_WARNINGS
#pragma warning(disable : 4005) 
#include <d3dx11.h>
#include <memory>
#include <vector>
#include <DirectXMath.h>
using namespace DirectX;

class CText
{
private:
	struct TextConstantBuffer
	{
		XMMATRIX WP;
		XMFLOAT4 Color;
		XMFLOAT4 Alpha;
	};
	struct ShaderTextVertex
	{
		XMFLOAT3 Pos; //位置
		XMFLOAT2 Tex; //テクスチャー座標
	};
	static std::weak_ptr<ID3D11ShaderResourceView> m_Texture;
	static ID3D11SamplerState* m_SampleLinear;
	static ID3D11VertexShader* m_VertexShader;
	static ID3D11PixelShader* m_PixelShader;
	static ID3D11InputLayout* m_VertexLayout;
	static ID3D11Buffer* m_ConstantBuffer;

	static std::vector<ID3D11Buffer*> m_VertexBufferList;
	static XMMATRIX m_Projection;
	static float m_Scale;
	static float m_Alpha;
	static XMFLOAT4 m_Color;

	static void Render(const char* Text, float X, const float Y);
	static void RenderFont(const int FontIndex, const float X, const float Y);
public:

	static HRESULT Init(const float Size, const XMFLOAT4& Color);
	static void UnInit();
	//テキスト描画　引数1 X座標 引数2 Y座標 引数3 テキスト
	//例:CText::Draw(50.0f, 50.0f, "ABC");
	//変数の場合:CText::Draw(50.0f, 50.0f, "HP = %d",HP);
	static void Draw(const float X, const float Y, const char* Text, ...);
};