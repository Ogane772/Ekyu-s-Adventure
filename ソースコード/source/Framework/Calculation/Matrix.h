#pragma once
#include <Eigen/Geometry>
#include <DirectXMath.h>

#include "Vector.h"

using namespace DirectX;

namespace FrameWork
{
	class Vector3;
	class Quaternion;
	class XMMATRIX4;

	typedef Eigen::Translation<float, 3> Translation3f;
	typedef Eigen::DiagonalMatrix<float, 3> Scaling3f;

	class Matrix4
	{
	private:
	
	public:
		Eigen::Matrix<float, 4, 4, Eigen::RowMajor> Matrix;

		Matrix4() { Matrix = Matrix.Identity(); }
		Matrix4(Eigen::Matrix<float, 4, 4, Eigen::RowMajor> matrix) : Matrix(matrix) {}
		Matrix4(const XMMATRIX& InMatrix) { *this = InMatrix; }
		Matrix4(const XMMATRIX4& InMatrix);

		Matrix4& operator= (const Eigen::Matrix<float, 4, 4, Eigen::RowMajor>& m);
		Matrix4& operator= (const XMMATRIX& m);
		Matrix4& operator= (const XMMATRIX4& m);
		Matrix4& operator*= (const Matrix4& m);
		Matrix4 operator* (const Matrix4& m);
		Vector3 operator* (const Vector3& v);

		// 自身の前方を指すベクトルを取得
		Vector3 Forward();
		// 自身の右方を指すベクトルを取得
		Vector3 Right();
		// 自身の上方を指すベクトルを取得
		Vector3 Up();

		Vector3 Position();
		Vector3 Scale();

		Matrix4 Inverse();
	};

	class XMMATRIX4
	{
	public:
		XMMATRIX Matrix = XMMATRIX();

		XMMATRIX4() {};
		XMMATRIX4(const XMMATRIX& m)
		{
			Matrix = m;
		};
		XMMATRIX4(const Matrix4& m)
		{
			*this = m;
		};

		XMMATRIX4& operator= (const Matrix4& m);
		XMMATRIX4& operator= (const XMMATRIX& m);
	};
}