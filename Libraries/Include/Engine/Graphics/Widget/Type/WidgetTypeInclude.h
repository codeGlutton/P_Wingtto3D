#pragma once

struct WidgetInheritedColor;
struct WidgetColor;

/**
 * 분할 방향
 */
enum class WidgetOrientation
{
	Horizon,
	Vertical,
};

enum class TilingType
{
	None,
	Horizon,
	Vertical,
	All
};

enum class ClippingType
{
	Inherit,
	Bound
};

enum class WidgetColorSelectType
{
	Specific,
	Inherited,
	Foreground,
};

struct Margin
{
	GEN_MINIMUM_STRUCT_REFLECTION(Margin)

public:
	Margin() = default;
	Margin(float offset) :
		mLeft(offset),
		mRight(offset),
		mUp(offset),
		mDown(offset)
	{
	}
	Margin(float horizon, float vertical) :
		mLeft(horizon),
		mRight(horizon),
		mUp(vertical),
		mDown(vertical)
	{
	}
	Margin(float left, float right, float up, float down) :
		mLeft(left),
		mRight(right),
		mUp(up),
		mDown(down)
	{
	}

public:
	bool operator==(const Margin&) const = default;

	friend Margin operator*(const Margin& margin, float scale);
	friend Margin operator*(float scale, const Margin& margin);
	friend Margin operator*(const Margin& margin, const Vec2& scale);
	friend Margin operator*(const Vec2& scale, const Margin& margin);

public:
	Vec2 GetDesiredSize() const
	{
		return Vec2(mLeft + mRight, mUp + mDown);
	}
	float GetDesiredSize(WidgetOrientation orient) const
	{
		return (orient == WidgetOrientation::Horizon) ? mLeft + mRight : mUp + mDown;
	}

public:
	PROPERTY(mLeft)
	float mLeft = 0.f;
	PROPERTY(mRight)
	float mRight = 0.f;
	PROPERTY(mUp)
	float mUp = 0.f;
	PROPERTY(mDown)
	float mDown = 0.f;
};

Margin operator*(const Margin& margin, float scale);
Margin operator*(float scale, const Margin& margin);
Margin operator*(const Margin& margin, const Vec2& scale);
Margin operator*(const Vec2& scale, const Margin& margin);

inline Margin operator*(const Margin& margin, float scale)
{
	return Margin(margin.mLeft * scale, margin.mRight * scale, margin.mUp * scale, margin.mDown * scale);
}
inline Margin operator*(float scale, const Margin& margin)
{
	return Margin(margin.mLeft * scale, margin.mRight * scale, margin.mUp * scale, margin.mDown * scale);
}
inline Margin operator*(const Margin& margin, const Vec2& scale)
{
	return Margin(margin.mLeft * scale.x, margin.mRight * scale.x, margin.mUp * scale.y, margin.mDown * scale.y);
}
inline Margin operator*(const Vec2& scale, const Margin& margin)
{
	return Margin(margin.mLeft * scale.x, margin.mRight * scale.x, margin.mUp * scale.y, margin.mDown * scale.y);
}

/**
 * 부모까지의 색상 혼합 결과 데이터
 */
struct WidgetInheritedColor
{
public:
	bool operator==(const WidgetInheritedColor& other) const = default;

public:
	WidgetInheritedColor& BlendColor(const Color& color)
	{
		mInheritedColor *= color;
		return *this;
	}
	WidgetInheritedColor& BlendOpacity(float alpha)
	{
		mInheritedColor.w *= alpha;
		return *this;
	}
	WidgetInheritedColor& SetForegroundColor(const WidgetColor& color);

public:
	// 반드시 적용할 결과적 상속 색
	Color mInheritedColor = Color::White;
	// 선택적 적용할 부모 전달 색
	Color mForegroundColor = Color::White;
};

/**
 * 색상 추출 방식을 변경할 수 있도록 확장성을 열어둔
 * 색상 값 데이터 
 */
struct WidgetColor
{
	GEN_MINIMUM_STRUCT_REFLECTION(WidgetColor)

public:
	WidgetColor(WidgetColorSelectType selectType = WidgetColorSelectType::Specific, Color specificColor = Color::White) :
		mSelectType(selectType),
		mSpecificColor(specificColor)
	{
	}

public:
	bool operator==(const WidgetColor& other) const = default;

public:
	static WidgetColor SpecificInst(Color specificColor = Color::White)
	{
		return WidgetColor(WidgetColorSelectType::Specific, specificColor);
	}
	static WidgetColor InheritInst()
	{
		return WidgetColor(WidgetColorSelectType::Inherited);
	}
	static WidgetColor ForegroundInst()
	{
		return WidgetColor(WidgetColorSelectType::Foreground);
	}

public:
	const Color& GetColor(const WidgetInheritedColor& color) const
	{
		switch (mSelectType)
		{
		case WidgetColorSelectType::Specific:
			return mSpecificColor;
		case WidgetColorSelectType::Inherited:
			return color.mInheritedColor;
		case WidgetColorSelectType::Foreground:
			return color.mForegroundColor;
		}
		return Color::White;
	}

public:
	PROPERTY(mSelectType)
	WidgetColorSelectType mSelectType;
	PROPERTY(mSpecificColor)
	Color mSpecificColor = Color::White;
};

namespace VisibilityType
{
	enum Flag : uint8
	{
		None = 0,
		VisibleFlag = 1 << 0,				// 보여짐
		TakesSpaceFlag = 1 << 1,			// 공간 차지
		HittableFlag = 1 << 2,				// 충돌 가능
		EffectChildrenHittablelag = 1 << 3,	// 자식에 충돌 영향
	};

	inline static Flag Visible = Flag(VisibleFlag | TakesSpaceFlag | HittableFlag);
	inline static Flag Collapsed = Flag(EffectChildrenHittablelag);
	inline static Flag Hidden = Flag(TakesSpaceFlag | EffectChildrenHittablelag);
	inline static Flag HitTestInvisible = Flag(VisibleFlag | TakesSpaceFlag | EffectChildrenHittablelag);
	inline static Flag SelfHitTestInvisible = Flag(VisibleFlag | TakesSpaceFlag);
	inline static Flag ALL = Flag(VisibleFlag | TakesSpaceFlag | HittableFlag | EffectChildrenHittablelag);
}
