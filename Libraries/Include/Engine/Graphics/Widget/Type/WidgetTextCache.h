#pragma once

#include "Graphics/Widget/Type/WidgetFontData.h"

struct WidgetCharCache
{
public:
	Vec2 mScaleOffset;
	Vec2 mScaleSize;
};

struct WidgetCharLocalCache : public WidgetCharCache
{
	// 여기서는 로컬 문자 좌표계 기준으로
	// mScaleOffset -> 문자 시작점 기준 오프셋
	// mScaleSize -> 랜더링 문자 크기

public:
	// 문자 간격
	float mScaleAdvance;
};

struct WidgetCharRenderCache : public WidgetCharCache
{
	// 여기서는 로컬 화면 좌표계 기준으로
	// mScaleOffset -> "^>기준" 랜더링 좌하단 위치. 이후 y 부호를 바꿔줄 것
	// mScaleSize -> 랜더링 문자 크기

public:
	// 사용 문자
	wchar_t mChar;
};

struct WidgetTextCache
{
public:
	enum class State
	{
		Dirty,
		Updating,
		OnCache
	};

public:
	void UpdateOnPrepass(
		const std::wstring& text,
		TextTransformPolicy transformPolicy,
		const WidgetFontData& font,
		const Margin& margin,
		const Vec2& layoutSacle
	);

	void UpdateOnPaint(
		const Vec2& allottedSize,
		TextJustifyPolicy justifyPolicy,
		bool autoWrap
	);

public:
	void MarkLayoutDirty()
	{
		_mState = State::Dirty;
	}
	void MarkPaintDirty()
	{
		_mState = State::Updating;
	}

public:
	bool IsDirty() const
	{
		return _mState != State::OnCache;
	}
	bool IsResized() const
	{
		return _mIsResized;
	}
	State GetState() const
	{
		return _mState;
	}
	const std::wstring& GetTransformedText() const
	{
		return _mTransformedText;
	}
	const Vec2& GetDesiredSize() const
	{
		return _mDesiredSize;
	}
	const Vec2& GetBoxSize() const
	{
		return _mBoxSize;
	}
	const Vec2& GetBoxOffset() const
	{
		return _mBoxOffset;
	}
	const WidgetCharLocalCache& GetCharCache(std::size_t textIndex) const
	{
		return _mCharLocalCaches[textIndex];
	}
	const std::vector<WidgetCharRenderCache>& GetCharRenderCaches() const
	{
		return _mCharRenderCaches;
	}

private:
	float ComputeTextLineOffset(
		float desiredLineSizeX,
		float allottedSizeX,
		TextJustifyPolicy justifyPolicy
	);
	float AlignTextLine(
		float desiredLineSizeX, 
		float allottedSizeX, 
		TextJustifyPolicy justifyPolicy,
		std::size_t startIndex,
		std::size_t endIndex
	);

private:
	State _mState = State::Dirty;
	float _mScaleLineHeight;
	float _mScaleAscender;
	Margin _mScaleMargin;

private:
	std::wstring _mTransformedText;
	Vec2 _mDesiredSize;

private:
	Vec2 _mBoxSize;
	Vec2 _mBoxOffset;

private:
	bool _mIsResized = false;
	std::vector<WidgetCharLocalCache> _mCharLocalCaches;
	std::vector<WidgetCharRenderCache> _mCharRenderCaches;
};

