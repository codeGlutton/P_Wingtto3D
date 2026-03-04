#include "pch.h"
#include "WidgetPath.h"

#include "Core/App/AppWindow/AppWindow.h"
#include "Graphics/Widget/Widget.h"

WidgetPath WidgetPath::GetPathDownTo(std::shared_ptr<Widget> widget)
{
	std::stack<std::shared_ptr<Widget>> widgetStack;
	while (widget != nullptr)
	{
		widgetStack.push(widget);
		widget = widget->GetParent();
	}

	WidgetPath path;
	if (widgetStack.empty() == false)
	{
		path.mRootWindow = std::static_pointer_cast<AppWindow>(widgetStack.top());
		while (widgetStack.empty() == false)
		{
			std::shared_ptr<Widget> topWidget = widgetStack.top();
			path.mWidgets.push_back(ArrangedWidget(topWidget, topWidget->GetScreenGeometry()));
			widgetStack.pop();
		}
	}

	return path;
}
