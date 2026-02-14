#include "pch.h"
#include "WidgetBrush.h"

#include "Utils/Thread/RefCounting.h"

Color Tint::GetColor(const WidgetInheritedColor& inheritedColor) const
{
    if (mUseInheritedColor == true)
    {
        return inheritedColor.mForegroundColor * inheritedColor.mInheritedColor;
    }
    else
    {
        return mColor * inheritedColor.mInheritedColor;
    }
}
