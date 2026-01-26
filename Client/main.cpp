#include "pch.h"
#include "Core/Object.h"

#include "Core/App/App.h"
#include "Core/App/Game.h"

// 디버그용
#include "TestGame.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	APP->Run<TestGame>(hInstance);
	return 0;
}