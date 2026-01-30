#include "pch.h"
#include "Editor.h"

#include "Input/DefaultEditorInputMappingContext.h"

#include "Manager/InputManager.h"

Editor::Editor()
{
}

Editor::~Editor()
{
}

void Editor::Init()
{
	AppModeBase::Init();
}

void Editor::Update()
{
	AppModeBase::Update();
}

void Editor::End()
{
	AppModeBase::End();
}

void Editor::BeginThread()
{
	INPUT_MANAGER->AddMappingContext(&DefaultEditorInputMappingContext::GetStaticTypeInfo(), 0);

	AppModeBase::BeginThread();
}

