#include "pch.h"
#include "Core/Object.h"

#include "Core/App/App.h"
#include "Core/App/Game.h"

// 디버그용
#include "TestGame.h"

#ifdef _EDITOR

#include "Editor/App/Editor.h"

#endif // _EDITOR


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	APP->Run<Editor>(hInstance);
	return 0;
}