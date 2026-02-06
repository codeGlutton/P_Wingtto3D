#pragma once

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

struct Margin
{
	GEN_MINIMUM_STRUCT_REFLECTION(Margin)

public:
	Vec2 GetDesiredSize() const
	{
		return Vec2(mLeft + mRight, mUp + mDown);
	}

public:
	PROPERTY(mLeft)
	float mLeft;
	PROPERTY(mRight)
	float mRight;
	PROPERTY(mUp)
	float mUp;
	PROPERTY(mDown)
	float mDown;
};

namespace VisibilityType
{
	enum Flag : uint8
	{
		None = 0,
		VisibleFlag = 1 << 0,			// 보여짐
		TakesSpaceFlag = 1 << 1,		// 공간 차지
		HittableFlag = 1 << 2,			// 충돌 가능
		EffectChildrenFlag = 1 << 3,	// 자식에 영향
	};

	inline static Flag Visible = Flag(VisibleFlag | TakesSpaceFlag);
	inline static Flag Collapsed = Flag(EffectChildrenFlag);
	inline static Flag Hidden = Flag(TakesSpaceFlag | EffectChildrenFlag);
	inline static Flag HitTestInvisible = Flag(VisibleFlag | TakesSpaceFlag | EffectChildrenFlag);
	inline static Flag SelfHitTestInvisible = Flag(VisibleFlag | TakesSpaceFlag);
	inline static Flag ALL = Flag(VisibleFlag | TakesSpaceFlag | HittableFlag | EffectChildrenFlag);
}
