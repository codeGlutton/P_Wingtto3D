#pragma once

class DXViewport
{
public:
	DXViewport(float width, float height, float x = 0, float y = 0, float minDepth = 0, float maxDepth = 1);
	~DXViewport();

public:
	void Init();

public:
	float GetWidth() const 
	{ 
		return _mDesc.Width;
	}
	float GetHeight() const 
	{ 
		return _mDesc.Height;
	}
	const D3D11_VIEWPORT& GetDesc() const
	{
		return _mDesc;
	}

public:
	void Resize(float width, float height, float x = 0, float y = 0, float minDepth = 0, float maxDepth = 1);

	/**
	 * 월드 좌표계 위치를 화면 상의 좌표로
	 * \param pos 월드 좌표계 위치
	 * \param w world 행렬
	 * \param v view 행렬
	 * \param p projection 행렬
	 * \return 화면 상의 좌표 값
	 */
	Vec3 Project(const Vec3& pos, const Matrix& w, const Matrix& v, const Matrix& p) const;
	Vec3 Project(const Vec3& pos, const Matrix& wvp) const;
	/**
	 * 화면 좌표계 위치를 월드 상의 좌표로
	 * \param pos 화면 좌표계 위치
	 * \param w world 행렬
	 * \param v view 행렬
	 * \param p projection 행렬
	 * \return 월드 상의 좌표 값
	 */
	Vec3 Unproject(const Vec3& pos, const Matrix& w, const Matrix& v, const Matrix& p) const;
	Vec3 Unproject(const Vec3& pos, const Matrix& wvp) const;

private:
	D3D11_VIEWPORT _mDesc = { 0 };
};

