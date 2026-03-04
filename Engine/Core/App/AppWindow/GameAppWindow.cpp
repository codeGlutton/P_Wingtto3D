#include "pch.h"
#include "GameAppWindow.h"

#include "Utils/WidgetUtils.h"

#include "Manager/WidgetStyleManager.h"
#include "Manager/AppWindowManager.h"

#include "Graphics/Viewport/Viewport.h"

#include "Graphics/Widget/VirtualWindowSplitter.h"
#include "Graphics/Widget/VirtualWindowContent.h"

void GameAppWindow::CreateRootVirtualWindow()
{
	std::shared_ptr<VirtualWindowSplitter> splitter = NEW_EDIT_WIDGET(VirtualWindowSplitter);
	std::shared_ptr<VirtualWindowContent> content = NEW_EDIT_WIDGET(VirtualWindowContent)
		.ShowHeader(false);

	AddSlot()
		[
			splitter
		];

	splitter->AddChildVirtualWindow(
		content
	);
	
	/*content->SetContent(
		NEW
	);*/
}
