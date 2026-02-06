#include "pch.h"
#include "Transform.h"

Matrix2D Transform2D::MakeLocalMatrix() const
{
	Matrix2D matScale = Matrix2D::CreateScale(_mLocalScale);
	Matrix2D matRotation = Matrix2D::CreateRotation(_mLocalRotation);
	Matrix2D matTranslation = Matrix2D::CreateTranslation(_mLocalPosition);

	return matScale * matRotation * matTranslation;
}

Matrix2D Transform2D::MakeLocalSTMatrix() const
{
	Matrix2D matScale = Matrix2D::CreateScale(_mLocalScale);
	Matrix2D matTranslation = Matrix2D::CreateTranslation(_mLocalPosition);

	return matScale * matTranslation;
}

Matrix Transform::MakeLocalMatrix() const
{
	Matrix matScale = Matrix::CreateScale(_mLocalScale);
	Matrix matRotation = Matrix::CreateRotationX(_mLocalRotation.x);
	matRotation *= Matrix::CreateRotationY(_mLocalRotation.y);
	matRotation *= Matrix::CreateRotationZ(_mLocalRotation.z);
	Matrix matTranslation = Matrix::CreateTranslation(_mLocalPosition);

	return matScale * matRotation * matTranslation;
}
