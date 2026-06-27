#pragma once
#include <D3D11.h>
#include "../Calculation/Vector.h"
#include "../Calculation/Matrix.h"

using namespace FrameWork;
struct CollisionMesh;
class CCommonObject;
enum class ECollisionTagName;
enum class ECollisionTagCameraName;
enum class ECollisionTagSteps;
enum class ECheckCollisionType;

//線描画クラス
struct LINE_CBUFFER
{
	XMMATRIX WVP = XMMATRIX();
	XMFLOAT4 Color = XMFLOAT4();
};

struct RayCastInfo
{
	Vector3 Point;
	Vector3 Normal;
	float Distance = 0.0f;
	ECollisionTagName CollisionTag = ECollisionTagName();
	ECollisionTagCameraName CameraCollisionTag = ECollisionTagCameraName();
	ECollisionTagSteps StepsCollisionTag = ECollisionTagSteps();

	RayCastInfo() {};
}
;
struct CBUFFER
{
	XMFLOAT4 Start = XMFLOAT4();
	XMFLOAT4 End = XMFLOAT4();
};

class CLine
{
private:
	//毎フレームUpdateで渡す必要がないのでポインタにする
	Vector3* m_PositionPointer = nullptr;
	XMFLOAT4 m_LineColor = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
	ID3D11Buffer* m_LineVertexBuffer = nullptr;

	//シェーダー用変数
	static ID3D11Buffer* m_LineConstantBuffer;
	static ID3D11InputLayout* m_LineVertexLayout;
	static ID3D11VertexShader* m_LineVertexShader;
	static ID3D11PixelShader* m_LinePixelShader;
	static ID3D11Buffer* m_ConstantBuffer;
	static ID3D11Buffer* m_BufferInput;
	static ID3D11Buffer* m_BufferResult;
	static ID3D11ShaderResourceView* m_BufferInputSRV;
	static ID3D11UnorderedAccessView* m_BufferResultUAV;

	//LineHit共通関数
	bool LineHitCheckSimple(const ECheckCollisionType Type, const Vector3& Start, const Vector3& End, RayCastInfo* CastInfo, CCommonObject* Object);
	bool JudgeFace(const Vector3& Start, const Vector3& End, CollisionMesh* CollisionMesh, RayCastInfo* CastInfo);
	bool IsInside(const Vector3& Point, const Vector3& P1, const Vector3& P2, const Vector3& P3, const Vector3& Normal);

public:
	static void ShaderInit();
	static void ShaderUnInit();
	CLine() {};
	CLine(Vector3* Pos, const Vector3& Start, const Vector3& End);
	~CLine();

	//描画を行う際はメンバ変数として持ち、Initで情報を設定する必要がある
	void Draw();

	void SetLineColor(const XMFLOAT4& Color) { m_LineColor = Color; }
	
	bool LineHitCheck(const ECheckCollisionType Type, const Vector3& Start, const Vector3& End, RayCastInfo * CastInfo);
	//エディットに使う当たり判定ボックスを無視する判定 選択したオブジェクトのみ当たり判定を行う
	bool LineHitCheck(const Vector3& Start, const Vector3& End, RayCastInfo * CastInfo, CCommonObject* Object);
};