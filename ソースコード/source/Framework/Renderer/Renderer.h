#pragma once
//デバイスの生成、ブレンドステート等の描画関連の設定を行うクラス。
//シェーダー作成や描画を行う際にあらゆるクラスから参照され、利便性のためStaticクラス
#include <d3d11.h>
#include <memory>
#include "ShaderStruct.h"

//影のテクスチャ解像度
#define DEPTHTEX_WIDTH 1920*2
#define DEPTHTEX_HEIGHT 1920*2

class CRenderer
{
private:
	static ID3D11Device*           m_D3DDevice;
	static ID3D11DeviceContext*    m_ImmediateContext;
	static IDXGISwapChain*         m_SwapChain;

	static ID3D11RenderTargetView* m_BackBuffer_TexRTV;//バックバッファーのRTV
	static ID3D11Texture2D* m_BackBuffer_DSTex;//バックバッファー用のDS
	static ID3D11DepthStencilView* m_BackBuffer_DSTexDSV;//バックバッファー用のDSのDSV

	static ID3D11Texture2D* m_DepthMap_Tex;//深度マップテクスチャー
	static ID3D11RenderTargetView* m_DepthMap_TexRTV;//深度マップテクスチャーのRTV
	static ID3D11ShaderResourceView* m_DepthMap_TexSRV;//深度マップテクスチャーのSRV
	static ID3D11Texture2D* m_DepthMap_DSTex;//深度マップテクスチャー用DS
	static ID3D11DepthStencilView* m_DepthMap_DSTexDSV;//深度マップテクスチャー用DSのDSV	

	static D3D11_BLEND_DESC m_BlendDesc;

	static ID3D11VertexShader*      m_VertexShader;
	static ID3D11PixelShader*       m_PixelShader;
	static ID3D11VertexShader*	    m_PolygonVertexShader;
	static ID3D11PixelShader*	    m_PolygonPixelShader;
	static ID3D11VertexShader*      m_TitleVertexShader;
	static ID3D11PixelShader*	    m_TitlePixelShader;
	static ID3D11VertexShader*      m_2DVertexShader;
	static ID3D11PixelShader*       m_2DPixelShader;
	static ID3D11InputLayout*       m_VertexLayout;
	static ID3D11Buffer*			m_WorldBuffer;
	static ID3D11Buffer*			m_ViewBuffer;
	static ID3D11Buffer*			m_ProjectionBuffer;
	static ID3D11Buffer*			m_MaterialBuffer;
	static ID3D11Buffer*			m_LightBuffer;
	static ID3D11Buffer*			m_CameraPosBuffer;
	static ID3D11Buffer*			m_FogBuffer;

	static FOG_CONSTANT				m_FogConstant;
	static FOG_CONSTANT				m_DefaultFogConstant;

	static ID3D11DepthStencilState* m_DepthStateEnable;
	static ID3D11DepthStencilState* m_DepthStateDisable;
	static IDXGIOutput* m_DXGIOutput;

public:
	static void Init();
	static void UnInit();
	static void Begin();
	static void End();

	//深度設定
	static void SetDepthEnable(const bool Enable);
	//シェーダーに必要なマトリクス、カメラ等を設定
	static void SetWorldViewProjection2D();
	static void SetWorldMatrix(const XMMATRIX& WorldMatrix);
	static void SetViewMatrix(const XMMATRIX& ViewMatrix);
	static void SetProjectionMatrix(const XMMATRIX& ProjectionMatrix);
	static void SetMaterial(const MATERIAL& Material);
	static void SetLight(const LIGHT& Light);
	static void SetCameraPos(const XMFLOAT4& CameraPos);
	static void SetVertexBuffers(ID3D11Buffer* VertexBuffer );
	static void SetIndexBuffer(ID3D11Buffer* IndexBuffer );
	//テクスチャ設定
	static void SetTexture(std::weak_ptr<ID3D11ShaderResourceView>& Texture);
	static void SetTextureIndex(const int Index , std::weak_ptr<ID3D11ShaderResourceView>& Texture);
	static void DrawIndexed(const UINT IndexCount, const UINT StartIndexLocation, const int BaseVertexLocation);
	static void SetWireframeMode();//ワイヤーフレームモードにする
	static void SetSolidMode();//ワイヤーフレームを元に戻す	

	//画面サイズが変更された時にレンダーターゲットを再生性する必要があるため関数定義
	static void CleanupRenderTarget();
	static void ResizeBuffers(const LPARAM& LParam);
	static void CreateRenderTarget();

	//各デバイス取得
	static ID3D11Device* GetDevice() { return m_D3DDevice; }
	static ID3D11DeviceContext* GetDeviceContext() { return m_ImmediateContext; }
	static ID3D11ShaderResourceView* GetDepthMapTexSRV() { return m_DepthMap_TexSRV; }
	static ID3D11DepthStencilView* GetDepthMapDSTexDSV() { return m_DepthMap_DSTexDSV; }
	static ID3D11RenderTargetView* GetDepthMapTexRTV() { return m_DepthMap_TexRTV; }
	static ID3D11RenderTargetView* GetBackBufferTexRTV() { return m_BackBuffer_TexRTV; }
	static ID3D11DepthStencilView* GetBackBufferDSTexDSV() { return m_BackBuffer_DSTexDSV; }
	static IDXGISwapChain* GetSwapChain() { return m_SwapChain; }
	//フォグ情報取得/リセット
	static FOG_CONSTANT& GetFogConstant() { return m_FogConstant;}
	static void ResetFogConstant() { m_FogConstant = m_DefaultFogConstant; }

	//シェーダーを設定する
	static void SetShader();
	static void SetPolygonShader();
	static void Set2DShader();
	static void SetTitleShader();
	static void SetPixelShader();

	static void BillBoardBlendSet();
	static void BillBoardBlendEnd();
};
