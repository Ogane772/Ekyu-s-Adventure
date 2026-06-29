#pragma once
//float型をまとめた各種座標等の設定に用いるクラス
#include <DirectXMath.h>
#include <cmath>
using namespace DirectX;

namespace FrameWork
{
	class Vector2
	{
	public:
		float x, y;

		Vector2() : x(0.0f), y(0.0f) {}
		Vector2(const float X, const float Y) : x(X), y(Y) {}

		Vector2& operator= (const Vector2& InVector2) { x = InVector2.x; y = InVector2.y; return *this; }
		Vector2& operator+= (const Vector2& InVector2) { x += InVector2.x; y += InVector2.y; return *this; }
		Vector2& operator-= (const Vector2& InVector2) { x -= InVector2.x; y -= InVector2.y; return *this; }
		Vector2& operator*= (const float Float) { x *= Float; y *= Float; return *this; }
		Vector2& operator/= (const float Float) { x /= Float; y /= Float; return *this; }
		Vector2 operator+ (const Vector2& InVector2) const
		{
			Vector2 v = Vector2(x, y);
			v.x += InVector2.x; v.y += InVector2.y;
			return v;
		}
		Vector2 operator- (const Vector2& InVector2) const
		{
			Vector2 v = Vector2(x, y);
			v.x -= InVector2.x; v.y -= InVector2.y;
			return v;
		}
		Vector2 operator* (const float Float) const
		{
			Vector2 v = Vector2(x, y);
			v *= Float;
			return v;
		}
		Vector2 operator/ (const float Float) const
		{
			Vector2 v = Vector2(x, y);
			v /= Float;
			return v;
		}
		bool operator== (const Vector2& InVector2) { return (x == InVector2.x) && (y == InVector2.y); }
		bool operator!= (const Vector2& InVector2) { return !(*this == InVector2); }
		Vector2 operator-() const
		{
			return Vector2(-x, -y);
		}
		Vector2 operator+() const
		{
			return Vector2(x, y);
		}

		float Length() const { return std::sqrtf(x * x + y * y); }
		// 長さの比較専用
		float LengthSq() const { return x * x + y * y; }

		// 自身を正規化する
		Vector2 Normalized()
		{
			const float value = 1.0f / Length();
			return (*this *= value);
		}

		// 正規化した値を返す
		static Vector2 Normalize(const Vector2& InVector2)
		{
			const float value = 1.0f / InVector2.Length();
			return (InVector2 * value);
		}

		// 内積
		static float Dot(const Vector2& Vec1, const Vector2& Vec2)
		{
			return (Vec1.x * Vec2.x + Vec1.y * Vec2.y);
		}

		// 外積
		static float Cross(const Vector2& Vec1, const Vector2& Vec2)
		{
			return (Vec1.x * Vec2.y - Vec1.y * Vec2.x);
		}

		// Vector2(1.0f, 1.0f);
		static Vector2 One() { return Vector2(1.0f, 1.0f); }
		// Vector2(0.0f, 0.0f);
		static Vector2 Zero() { return Vector2(0.0f, 0.0f); }
		// Vector2(0.0f, 1.0f);
		static Vector2 Up() { return Vector2(0.0f, 1.0f); }
		// Vector2(0.0f, -1.0f);
		static Vector2 Down() { return Vector2(0.0f, -1.0f); }
		// Vector2(1.0f, 0.0f);
		static Vector2 Right() { return Vector2(1.0f, 0.0f); }
		// Vector2(-1.0f, 0.0f);
		static Vector2 Left() { return Vector2(-1.0f, 0.0f); }
	};

	class Vector3
	{
	public:
		float x, y, z;

		Vector3() : x(0.0f), y(0.0f), z(0.0f) {}
		Vector3(const float X, const float Y, const float Z) : x(X), y(Y), z(Z) {}
		Vector3(const XMFLOAT3& Float3) { *this = Float3; }

		Vector3& operator= (const Vector3& InVector3) { x = InVector3.x; y = InVector3.y; z = InVector3.z; return *this; }
		Vector3& operator= (const Vector2& InVector2) { x = InVector2.x; y = 0.0f; z = InVector2.y; return *this; }
		Vector3& operator= (const XMFLOAT3& Float3) { x = Float3.x; y = Float3.y; z = Float3.z; return *this; }
		Vector3& operator+= (const Vector3& InVector3) { x += InVector3.x; y += InVector3.y; z += InVector3.z; return *this; }
		Vector3& operator-= (const Vector3& InVector3) { x -= InVector3.x; y -= InVector3.y; z -= InVector3.z; return *this; }
		Vector3& operator*= (const float Float) { x *= Float; y *= Float; z *= Float; return *this; }
		Vector3& operator/= (const float Float) { x /= Float; y /= Float; z /= Float; return *this; }
		Vector3 operator+ (const Vector3& InVector3) const
		{
			return Vector3(x + InVector3.x, y + InVector3.y, z + InVector3.z);
		}
		Vector3 operator- (const Vector3& InVector3) const
		{
			return Vector3(x - InVector3.x, y - InVector3.y, z - InVector3.z);
		}
		Vector3 operator* (const Vector3& InVector3) const
		{
			return Vector3(x * InVector3.x, y * InVector3.y, z * InVector3.z);
		}
		Vector3 operator/ (const Vector3& InVector3) const
		{
			return Vector3(x / InVector3.x, y / InVector3.y, z / InVector3.z);
		}
		Vector3 operator* (const float Float) const
		{
			return Vector3(x * Float, y * Float, z * Float);
		}
		Vector3 operator/ (const float Float) const
		{
			return Vector3(x / Float, y / Float, z / Float);
		}
		bool operator== (const Vector3& InVector3) const { return (x == InVector3.x) && (y == InVector3.y) && (z == InVector3.z); }
		bool operator!= (const Vector3& InVector3) const { return (x != InVector3.x) || (y != InVector3.y) || (z != InVector3.z); }

		Vector3 operator-() const
		{
			return Vector3(-x, -y, -z);
		}
		Vector3 operator+() const
		{
			return Vector3(x, y, z);
		}

		operator float* () const
		{
			return (float *)&x;
		}
		operator const float* () const
		{
			return (const float *)&x;
		}

		// XMFLOAT3として返す
		XMFLOAT3 XMFLOAT() const { return XMFLOAT3(x, y, z); }

		// ベクトルの長さ取得
		float Length() const { return std::sqrtf(x * x + y * y + z * z); }
		// ベクトルの長さの比較専用
		float LengthSq() const { return x * x + y * y + z * z; }

		//自身の角度を返す
		float Angle() const { return XMConvertToDegrees(atan2(y, sqrt(pow(z, 2.0f) + pow(x, 2.0f)))); }

		// 自身を正規化する
		Vector3 Normalize() 
		{ 
			float value = Length(); 
			if (value == 1.0f) return *this; // 既に正規化済みなら何もしない
			if (value == 0.0f) return Zero();
			*this /= value;
			return *this;
		}

		// 自身の正規化した値を返す
		Vector3 Normalized() const
		{
			return Normalize(*this);
		}

		// 自身との内積
		float Dot(const Vector3& InVector3) const
		{
			return Dot(*this, InVector3);
		}

		// 自身との外積
		Vector3 Cross(const Vector3& InVector3) const
		{
			return Cross(*this, InVector3);
		}

		// 線形補間 value( 0.0f ～ 1.0f )
		static Vector3 Lerp(const Vector3& From, const Vector3& To, const float Alpha)
		{
			Vector3 returnValue;
			returnValue.x = std::lerp(From.x, To.x, Alpha);
			returnValue.y = std::lerp(From.y, To.y, Alpha);
			returnValue.z = std::lerp(From.z, To.z, Alpha);
			return returnValue;
		}

		// 正規化した値を返す
		static Vector3 Normalize(const Vector3& InVector3)
		{
			const float value = InVector3.Length();
			if (value == 0.0f) return Zero();
			return (InVector3 / value);
		}

		// 内積
		static float Dot(const Vector3& Vec1, const Vector3& Vec2)
		{
			return (Vec1.x * Vec2.x + Vec1.y * Vec2.y + Vec1.z * Vec2.z);
		}

		// 外積
		static Vector3 Cross(const Vector3& Vec1, const Vector3& Vec2)
		{
			Vector3 vec;
			vec.x = Vec1.y * Vec2.z - Vec1.z * Vec2.y;
			vec.y = Vec1.z * Vec2.x - Vec1.x * Vec2.z;
			vec.z = Vec1.x * Vec2.y - Vec1.y * Vec2.x;

			return vec;
		}

		// ベクトルの大きさをワールド座標でのradiusにする
		static Vector3 Scale(const Vector3& InVector3, const float Scale)
		{
			return InVector3 * Scale;
		}

		//マトリクス変換
		static Vector3 TransformCoord(const Vector3& InVector3, const XMMATRIX& Matrix)
		{
			Vector3 out;
			const float norm = Matrix.r[0].m128_f32[3] * InVector3.x + Matrix.r[1].m128_f32[3] * InVector3.y + Matrix.r[2].m128_f32[3] * InVector3.z + Matrix.r[3].m128_f32[3];

			out.x = (Matrix.r[0].m128_f32[0] * InVector3.x + Matrix.r[1].m128_f32[0] * InVector3.y + Matrix.r[2].m128_f32[0] * InVector3.z + Matrix.r[3].m128_f32[0]) / norm;
			out.y = (Matrix.r[0].m128_f32[1] * InVector3.x + Matrix.r[1].m128_f32[1] * InVector3.y + Matrix.r[2].m128_f32[1] * InVector3.z + Matrix.r[3].m128_f32[1]) / norm;
			out.z = (Matrix.r[0].m128_f32[2] * InVector3.x + Matrix.r[1].m128_f32[2] * InVector3.y + Matrix.r[2].m128_f32[2] * InVector3.z + Matrix.r[3].m128_f32[2]) / norm;
			return out;
		}

		static Vector3 TransformNormal(const Vector3& InVector3, const XMMATRIX& Matrix)
		{
			Vector3 out;
			out.x = Matrix.r[0].m128_f32[0] * InVector3.x + Matrix.r[1].m128_f32[0] * InVector3.y + Matrix.r[2].m128_f32[0] * InVector3.z;
			out.y = Matrix.r[0].m128_f32[1] * InVector3.x + Matrix.r[1].m128_f32[1] * InVector3.y + Matrix.r[2].m128_f32[1] * InVector3.z;
			out.z = Matrix.r[0].m128_f32[2] * InVector3.x + Matrix.r[1].m128_f32[2] * InVector3.y + Matrix.r[2].m128_f32[2] * InVector3.z;
			return out;
		}

		static XMMATRIX* MatrixLookAtLH(XMMATRIX& Out, const Vector3& Eye, const Vector3& At, const Vector3& Up)
		{
			Vector3 rightVector, upVector, vector;

			vector = At - Eye;
			vector.Normalize();
			rightVector = Vector3::Cross(Up, vector);
			upVector = Vector3::Cross(vector, rightVector);
			rightVector.Normalize();
			upVector.Normalize();
			Out.r[0].m128_f32[0] = rightVector.x;
			Out.r[1].m128_f32[0] = rightVector.y;
			Out.r[2].m128_f32[0] = rightVector.z;
			Out.r[3].m128_f32[0] = -Vector3::Dot(rightVector, Eye);
			Out.r[0].m128_f32[1] = upVector.x;
			Out.r[1].m128_f32[1] = upVector.y;
			Out.r[2].m128_f32[1] = upVector.z;
			Out.r[3].m128_f32[1] = -Vector3::Dot(upVector, Eye);
			Out.r[0].m128_f32[2] = vector.x;
			Out.r[1].m128_f32[2] = vector.y;
			Out.r[2].m128_f32[2] = vector.z;
			Out.r[3].m128_f32[2] = -Vector3::Dot(vector, Eye);
			Out.r[0].m128_f32[3] = 0.0f;
			Out.r[1].m128_f32[3] = 0.0f;
			Out.r[2].m128_f32[3] = 0.0f;
			Out.r[3].m128_f32[3] = 1.0f;

			return &Out;
		}

		// return Vector3(1.0f, 1.0f, 1.0f);
		static Vector3 One() { return Vector3(1.0f, 1.0f, 1.0f); }
		// return Vector3(0.0f, 0.0f, 0.0f);
		static Vector3 Zero() { return Vector3(0.0f, 0.0f, 0.0f); }
		// return Vector3(0.0f, 1.0f, 0.0f);
		static Vector3 Up() { return Vector3(0.0f, 1.0f, 0.0f); }
		// return Vector3(0.0f, -1.0f, 0.0f);
		static Vector3 Down() { return Vector3(0.0f, -1.0f, 0.0f); }
		// return Vector3(1.0f, 0.0f, 0.0f);
		static Vector3 Right() { return Vector3(1.0f, 0.0f, 0.0f); }
		// return Vector3(-1.0f, 0.0f, 0.0f);
		static Vector3 Left() { return Vector3(-1.0f, 0.0f, 0.0f); }
		// return Vector3(0.0f, 0.0f, 1.0f);
		static Vector3 Forward() { return Vector3(0.0f, 0.0f, 1.0f); }
		// return Vector3(0.0f, 0.0f, -1.0f);
		static Vector3 Back() { return Vector3(0.0f, 0.0f, -1.0f); }
	};

	inline Vector3 operator* (const float Float, const class Vector3 & InVector3)
	{
		return Vector3(Float * InVector3.x, Float * InVector3.y, Float * InVector3.z);
	}

	inline Vector3 operator/ (const float Float, const class Vector3& InVector3)
	{
		return Vector3(Float / InVector3.x, Float / InVector3.y, Float / InVector3.z);
	}
}

