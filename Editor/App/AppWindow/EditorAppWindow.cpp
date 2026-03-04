#include "pch.h"
#include "EditorAppWindow.h"

#include <shellapi.h>

#include "Utils/WidgetUtils.h"

#include "Manager/WidgetStyleManager.h"

#include "Graphics/Widget/VirtualWindowSplitter.h"
#include "Graphics/Widget/VirtualWindowContent.h"

void EditorAppWindow::PostLoad()
{
	Super::PostLoad();

	DragAcceptFiles(GetDesc().mHWndRef->mData, TRUE);
}

void EditorAppWindow::CreateRootVirtualWindow()
{
	std::shared_ptr<VirtualWindowSplitter> rootSplitter = NEW_EDIT_WIDGET(VirtualWindowSplitter);
	{
		AddSlot()
		[
			rootSplitter
		];

		std::shared_ptr<VirtualWindowSplitter> mainSplitter = NEW_EDIT_WIDGET(VirtualWindowSplitter);
		mainSplitter->SetOrientation(WidgetOrientation::Vertical);
		{
			std::shared_ptr<VirtualWindowContent> viewportContent = NEW_EDIT_WIDGET(VirtualWindowContent)
				.ShowHeader(false);
			viewportContent->SetSizeRate(0.7f);
			std::shared_ptr<VirtualWindowContent> assetContent = NEW_EDIT_WIDGET(VirtualWindowContent);
			assetContent->SetSizeRate(0.3f);

			mainSplitter->AddChildVirtualWindow(
				viewportContent
			);
			mainSplitter->AddChildVirtualWindow(
				assetContent
			);
		}
		mainSplitter->SetSizeRate(0.75f);

		std::shared_ptr<VirtualWindowSplitter> sideSplitter = NEW_EDIT_WIDGET(VirtualWindowSplitter);
		sideSplitter->SetOrientation(WidgetOrientation::Vertical);
		{
			std::shared_ptr<VirtualWindowContent> worldContent = NEW_EDIT_WIDGET(VirtualWindowContent);
			worldContent->SetSizeRate(0.5f);
			std::shared_ptr<VirtualWindowContent> propertyContent = NEW_EDIT_WIDGET(VirtualWindowContent);
			propertyContent->SetSizeRate(0.5f);

			sideSplitter->AddChildVirtualWindow(
				worldContent
			);
			sideSplitter->AddChildVirtualWindow(
				propertyContent
			);
		}
		sideSplitter->SetSizeRate(0.25f);

		rootSplitter->AddChildVirtualWindow(
			mainSplitter
		);
		rootSplitter->AddChildVirtualWindow(
			sideSplitter
		);
	}
}
