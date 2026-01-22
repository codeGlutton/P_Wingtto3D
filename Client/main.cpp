#include "pch.h"
#include "Core/Object.h"

#include "Core/App/App.h"
#include "Core/App/Game.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	APP->Run<Game>();
	return 0;
}