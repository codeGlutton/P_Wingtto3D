#pragma once

struct CameraConstantData
{
public:
	CameraConstantData() :
		_mView(Matrix::Identity),
		_mProj(Matrix::Identity),
		_mVP(Matrix::Identity),
		_mCameraPos(Vec3::Zero)
	{
	}

public:
	void UpdateData(const Matrix& view, const Matrix& proj, const Vec3& cameraPos)
	{
		_mView = view;
		_mProj = proj;
		_mVP = _mView * _mProj;
		_mCameraPos = cameraPos;
	}

private:
	Matrix _mView;
	Matrix _mProj;
	Matrix _mVP;

	Vec3 _mCameraPos;

private:
	float mPadding = 0.f;
};

struct ScreenConstantData
{
public:
	ScreenConstantData(const Vec2& screenSize) :
		_mScreenSize(screenSize)
	{
	}

public:
	const Vec2& GetScreenSize() const
	{
		return _mScreenSize;
	}
	void SetScreenSize(const Vec2& screenSize)
	{
		_mScreenSize = screenSize;
	}

private:
	Vec2 _mScreenSize;

private:
	float _mPadding[2] = {};
};

struct WidgetConstantData
{
public:
	WidgetConstantData(bool isText) :
		_mIsText(isText == true ? 1.f : 0.f)
	{
	}

public:
	bool IsText() const
	{
		return _mIsText > 0.5f;
	}
	void SetIsText(bool isText)
	{
		_mIsText = isText == true ? 1.f : 0.f;
	}

private:
	float _mIsText;

private:
	float _mPadding[3] = {};
};

