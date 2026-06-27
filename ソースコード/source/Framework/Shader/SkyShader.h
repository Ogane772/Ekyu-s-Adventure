#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
using namespace DirectX;
class CCamera;

class CSkyShader
{
private:
	struct ConstantSky
	{
		XMMATRIX WorldMatrix;
		XMMATRIX ViewMatrix;
		XMMATRIX ProjectionMatrix;

		XMFLOAT4 CameraPosition;
		XMFLOAT4 TextureScroll;
		XMFLOAT4 FogParam;
	};

	ID3D11VertexShader* m_VertexShader = nullptr;
	ID3D11PixelShader* m_PixelShader = nullptr;
	ID3D11InputLayout* m_VertexLayout = nullptr;

	ID3D11Buffer* m_SkyConstantBuffer = nullptr;
	ConstantSky	m_Constant;

public:
	CSkyShader();
	~CSkyShader();
	void Set(const XMMATRIX& World, const CCamera* Camera, const float TexScroll);
};