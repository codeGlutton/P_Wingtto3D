#pragma once

#ifdef _EDITOR

#include "EditorPch.h"
#pragma comment(lib, "Editor/Editor.lib")

#else

#include "EnginePch.h"
#pragma comment(lib, "Engine/Engine.lib")

#endif // _EDITOR