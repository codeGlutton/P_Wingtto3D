#pragma once

#define WIN32_LEAN_AND_MEAN   

/* STL */

#include <memory>
#include <iostream>
#include <array>
#include <vector>
#include <list>
#include <set>
#include <map>
#include <unordered_set>
#include <unordered_map>

#include <algorithm>
#include <type_traits>

#include <optional>
#include <functional>

#include <format>

/* Window */

#include <windows.h>
#include <assert.h>

///* Assimp */
//
//#include <Assimp/Importer.hpp>
//#include <Assimp/scene.h>
//#include <Assimp/postprocess.h>

///* ImGui */
//
//#include "imgui.h"
//#include "imgui_impl_win32.h"
//#include "imgui_impl_dx11.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")

//#ifdef _DEBUG
//#pragma comment(lib, "DirectXTex/DirectXTex_debug.lib")
//#pragma comment(lib, "FX11/Effects11d.lib")
//#pragma comment(lib, "Assimp/assimp-vc143-mtd.lib")
//#else
//#pragma comment(lib, "DirectXTex/DirectXTex.lib")
//#pragma comment(lib, "FX11/Effects11.lib")
//#pragma comment(lib, "Assimp/assimp-vc143-mt.lib")
//#endif

/* Headers */

#include "Types.h"
#include "Defines.h"

/* Managers */

#include "Manager/BootSystem.h"

/* Reflection */

#include "Reflection/Reflection.h"
#include "Reflection/InterfaceReflector.h"
#include "Reflection/ObjectTypeInfo.h"
#include "Reflection/DefaultTypeInfo.h"
#include "Reflection/Method.h"
#include "Reflection/Property.h"
#include "Reflection/SubClass.h"

/* Utils */

#include "Utils/TypeUtils.h"

