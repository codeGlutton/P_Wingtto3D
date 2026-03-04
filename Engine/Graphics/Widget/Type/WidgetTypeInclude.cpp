#include "pch.h"
#include "WidgetTypeInclude.h"

WidgetInheritedColor& WidgetInheritedColor::SetForegroundColor(const WidgetColor& color)
{
	mForegroundColor = color.GetColor(*this);
	return *this;
}
