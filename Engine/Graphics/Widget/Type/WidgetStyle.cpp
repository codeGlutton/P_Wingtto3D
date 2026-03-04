#include "pch.h"
#include "WidgetStyle.h"

#include "Manager/WidgetStyleManager.h"

void WidgetStyle::PostLoad()
{
	Super::PostLoad();

	WIDGET_STYLE_MANAGER->NotifyToAddStyle(std::static_pointer_cast<WidgetStyle>(shared_from_this()));
}

