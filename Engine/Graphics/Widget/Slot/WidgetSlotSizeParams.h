#pragma once

#include "Graphics/Widget/Type/Attribute.h"

enum class WidgetSlotSizeRule
{
	ContentSize,		// 콘텐츠 크기 맞춤
	AllottedRate,		// 부모의 할당 비율 맞춤
	DetailAllottedRate,	// 부모의 증감 및 수축 할당 비율 맞춤
};

/**
 * 위젯 슬롯 생성용 사이즈 연관 전달 인자
 */
struct WidgetSlotSizeParams
{
public:
	WidgetSlotSizeParams()
	{
	}

protected:
	WidgetSlotSizeParams(WidgetSlotSizeRule sizeRule, const Attribute<float>& stretchRate, const Attribute<float>& shrinkRate) :
		mSizeRule(sizeRule),
		mStretchRate(stretchRate),
		mShrinkRate(shrinkRate)
	{
	}

public:
	// 사이즈 방식
	WidgetSlotSizeRule mSizeRule;
	// 증감 비율
	Attribute<float> mStretchRate;
	// 수축 비율
	Attribute<float> mShrinkRate;
};

struct ContentSizeParams : public WidgetSlotSizeParams
{
public:
	ContentSizeParams() : 
		WidgetSlotSizeParams(WidgetSlotSizeRule::ContentSize, 0.f, 0.f)
	{
	}
};

struct AllottedRateParams : public WidgetSlotSizeParams
{
public:
	AllottedRateParams() :
		WidgetSlotSizeParams(WidgetSlotSizeRule::AllottedRate, 1.f, 1.f)
	{
	}

	AllottedRateParams(const Attribute<float>& stretchRate) :
		WidgetSlotSizeParams(WidgetSlotSizeRule::AllottedRate, stretchRate, stretchRate)
	{
	}
};

struct DetailAllottedRateParams : public WidgetSlotSizeParams
{
public:
	DetailAllottedRateParams() :
		WidgetSlotSizeParams(WidgetSlotSizeRule::DetailAllottedRate, 1.f, 1.f)
	{
	}

	DetailAllottedRateParams(const Attribute<float>& stretchRate) :
		WidgetSlotSizeParams(WidgetSlotSizeRule::DetailAllottedRate, stretchRate, stretchRate)
	{
	}

	DetailAllottedRateParams(const Attribute<float>& stretchRate, const Attribute<float>& shrinkRate) :
		WidgetSlotSizeParams(WidgetSlotSizeRule::DetailAllottedRate, stretchRate, shrinkRate)
	{
	}
};