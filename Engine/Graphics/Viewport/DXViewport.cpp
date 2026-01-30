#include "pch.h"
#include "DXViewport.h"

DXViewport::DXViewport(float width, float height, float x, float y, float minDepth, float maxDepth)
{
	_mDesc.TopLeftX = x;
	_mDesc.TopLeftY = y;
	_mDesc.Width = width;
	_mDesc.Height = height;
	_mDesc.MinDepth = minDepth;
	_mDesc.MaxDepth = maxDepth;
}

DXViewport::~DXViewport()
{
}

void DXViewport::Init()
{
}

void DXViewport::Resize(float width, float height, float x, float y, float minDepth, float maxDepth)
{
	_mDesc.TopLeftX = x;
	_mDesc.TopLeftY = y;
	_mDesc.Width = width;
	_mDesc.Height = height;
	_mDesc.MinDepth = minDepth;
	_mDesc.MaxDepth = maxDepth;
}

Vec3 DXViewport::Project(const Vec3& pos, const Matrix& w, const Matrix& v, const Matrix& p) const
{
	Matrix wvp = w * v * p;
	return Project(pos, wvp);
}

Vec3 DXViewport::Project(const Vec3& pos, const Matrix& wvp) const
{
	Vec3 projPoint = Vec3::Transform(pos, wvp);

	// 스크린 좌표로 변환
	//|width/2		0			0					0|
	//|0			-height/2	0					0|
	//|0			0			maxDepth - minDepth	0|
	//|width/2+x	width/2+y	minDepth			1|
	projPoint.x = (projPoint.x + 1.f) * (_mDesc.Width / 2) + _mDesc.TopLeftX;
	projPoint.y = (-projPoint.y + 1.f) * (_mDesc.Height / 2) + _mDesc.TopLeftY;
	projPoint.z = projPoint.z * (_mDesc.MaxDepth - _mDesc.MinDepth) + _mDesc.MinDepth;

	return projPoint;
}

Vec3 DXViewport::Unproject(const Vec3& pos, const Matrix& w, const Matrix& v, const Matrix& p) const
{
	Matrix wvp = w * v * p;
	return Unproject(pos, wvp);
}

Vec3 DXViewport::Unproject(const Vec3& pos, const Matrix& wvp) const
{
	Vec3 screenPoint = pos;
	// 스크린 좌표로 변환
	//|width/2		0			0					0|
	//|0			-height/2	0					0|
	//|0			0			maxDepth - minDepth	0|
	//|width/2+x	width/2+y	minDepth			1|
	// 역으로 구해주기
	screenPoint.x = 2.f * (screenPoint.x - _mDesc.TopLeftX) / _mDesc.Width - 1.f;
	screenPoint.y = -2.f * (screenPoint.y - _mDesc.TopLeftY) / _mDesc.Height + 1.f;
	screenPoint.z = (screenPoint.z - _mDesc.MinDepth) / (_mDesc.MaxDepth - _mDesc.MinDepth);
	return Vec3::Transform(screenPoint, wvp.Invert());
}
