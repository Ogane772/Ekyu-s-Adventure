#include "Matrix.h"

using namespace FrameWork;

Matrix4::Matrix4(const XMMATRIX4& InMatrix)
{
	*this = InMatrix; 
}

Matrix4& Matrix4::operator= (const Eigen::Matrix<float, 4, 4, Eigen::RowMajor>& m)
{
	Matrix = m;
	return *this;
}

Matrix4& Matrix4::operator= (const XMMATRIX & m)
{
	Matrix(0, 0) = m.r[0].m128_f32[0];
	Matrix(1, 0) = m.r[0].m128_f32[1];
	Matrix(2, 0) = m.r[0].m128_f32[2];
	Matrix(3, 0) = m.r[0].m128_f32[3];

	Matrix(0, 1) = m.r[1].m128_f32[0];
	Matrix(1, 1) = m.r[1].m128_f32[1];
	Matrix(2, 1) = m.r[1].m128_f32[2];
	Matrix(3, 1) = m.r[1].m128_f32[3];

	Matrix(0, 2) = m.r[2].m128_f32[0];
	Matrix(1, 2) = m.r[2].m128_f32[1];
	Matrix(2, 2) = m.r[2].m128_f32[2];
	Matrix(3, 2) = m.r[2].m128_f32[3];

	Matrix(0, 3) = m.r[3].m128_f32[0];
	Matrix(1, 3) = m.r[3].m128_f32[1];
	Matrix(2, 3) = m.r[3].m128_f32[2];
	Matrix(3, 3) = m.r[3].m128_f32[3];

	return *this;
}

Matrix4& Matrix4::operator= (const XMMATRIX4& m)
{
	*this = m.Matrix;
	return *this;
}

Matrix4 & Matrix4::operator*=(const Matrix4 & m)
{
	Matrix *= m.Matrix;
	return *this;
}

Matrix4 Matrix4::operator*(const Matrix4& m)
{
	Matrix4 m1 = *this;
	m1 *= m;
	return m1;
}

Vector3 Matrix4::operator* (const Vector3& v)
{
	Eigen::Vector3f vec = Eigen::Vector3f(v.x, v.y, v.z);
	Eigen::Affine3f affine;
	affine = Matrix;
	vec = affine * vec;
	return Vector3(vec.x(), vec.y(), vec.z());
}

Vector3 Matrix4::Forward()
{
	return Vector3(Matrix(0, 2), Matrix(1, 2), Matrix(2, 2)).Normalize();
}

Vector3 Matrix4::Right()
{
	return Vector3(Matrix(0, 0), Matrix(1, 0), Matrix(2, 0)).Normalize();
}

Vector3 Matrix4::Up()
{
	return Vector3(Matrix(0, 1), Matrix(1, 1), Matrix(2, 1)).Normalize();
}

Vector3 Matrix4::Position()
{
	return Vector3(Matrix(0, 3), Matrix(1, 3), Matrix(2, 3));
}

Vector3 Matrix4::Scale()
{
	Vector3 x(Matrix(0, 0), Matrix(0, 1), Matrix(0, 2));
	Vector3 y(Matrix(1, 0), Matrix(1, 1), Matrix(1, 2));
	Vector3 z(Matrix(2, 0), Matrix(2, 1), Matrix(2, 2));
	return Vector3(x.Length(), y.Length(), z.Length());
}


Matrix4 Matrix4::Inverse()
{
	Matrix = Matrix.inverse();
	return *this;
}

XMMATRIX4& XMMATRIX4::operator= (const XMMATRIX& m)
{
	Matrix.r[0].m128_f32[0] = m.r[0].m128_f32[0];
	Matrix.r[0].m128_f32[1] = m.r[0].m128_f32[1];
	Matrix.r[0].m128_f32[2] = m.r[0].m128_f32[2];
	Matrix.r[0].m128_f32[3] = m.r[0].m128_f32[3];

	Matrix.r[1].m128_f32[0] = m.r[1].m128_f32[0];
	Matrix.r[1].m128_f32[1] = m.r[1].m128_f32[1];
	Matrix.r[1].m128_f32[2] = m.r[1].m128_f32[2];
	Matrix.r[1].m128_f32[3] = m.r[1].m128_f32[3];

	Matrix.r[2].m128_f32[0] = m.r[2].m128_f32[0];
	Matrix.r[2].m128_f32[1] = m.r[2].m128_f32[1];
	Matrix.r[2].m128_f32[2] = m.r[2].m128_f32[2];
	Matrix.r[2].m128_f32[3] = m.r[2].m128_f32[3];

	Matrix.r[3].m128_f32[0] = m.r[3].m128_f32[0];
	Matrix.r[3].m128_f32[1] = m.r[3].m128_f32[1];
	Matrix.r[3].m128_f32[2] = m.r[3].m128_f32[2];
	Matrix.r[3].m128_f32[3] = m.r[3].m128_f32[3];

	return *this;
}

XMMATRIX4& XMMATRIX4::operator= (const Matrix4& m)
{
	Matrix.r[0].m128_f32[0] = m.Matrix(0,0);
	Matrix.r[0].m128_f32[1] = m.Matrix(1,0);
	Matrix.r[0].m128_f32[2] = m.Matrix(2,0);
	Matrix.r[0].m128_f32[3] = m.Matrix(3,0);
										 
	Matrix.r[1].m128_f32[0] = m.Matrix(0,1);
	Matrix.r[1].m128_f32[1] = m.Matrix(1,1);
	Matrix.r[1].m128_f32[2] = m.Matrix(2,1);
	Matrix.r[1].m128_f32[3] = m.Matrix(3,1);
										 
	Matrix.r[2].m128_f32[0] = m.Matrix(0,2);
	Matrix.r[2].m128_f32[1] = m.Matrix(1,2);
	Matrix.r[2].m128_f32[2] = m.Matrix(2,2);
	Matrix.r[2].m128_f32[3] = m.Matrix(3,2);
										 
	Matrix.r[3].m128_f32[0] = m.Matrix(0,3);
	Matrix.r[3].m128_f32[1] = m.Matrix(1,3);
	Matrix.r[3].m128_f32[2] = m.Matrix(2,3);
	Matrix.r[3].m128_f32[3] = m.Matrix(3,3);

	return *this;
}