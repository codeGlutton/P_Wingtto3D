#include "pch.h"
#include "WidgetPath.h"

#include "Core/App/AppWindow/AppWindow.h"
#include "Graphics/Widget/Widget.h"

WidgetPath WidgetPath::GetPathDownTo(std::shared_ptr<Widget> widget)
{
	WidgetPath path;
	while (widget != nullptr)
	{
		path.mWidgets.push_back(ArrangedWidget(widget, widget->GetScreenGeometry()));
		widget = widget->GetParent();
	}
	if (path.mWidgets.empty() == false)
	{
		path.mRootWindow = std::static_pointer_cast<AppWindow>(path.mWidgets.back().mWidget);
	}

	return path;
}
