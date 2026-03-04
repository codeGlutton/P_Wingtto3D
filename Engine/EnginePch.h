#pragma once

#define WIN32_LEAN_AND_MEAN   

/* STL */

#include <memory>
#include <iostream>
#include <array>
#include <vector>
#include <list>
#include <stack>
#include <queue>
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
#include <wrl.h>
using namespace Microsoft::WRL;

/* DirectX */

#include <d3d11.h>
#include <d3dcompiler.h>
#include <d3d11shader.h>

#include <DirectXMath.h>
#include "DirectXTex/DirectXTex.h"
#include "DirectXTex/DirectXTex.inl"

/* Simple Math */

#include "SimpleMath/SimpleMath.h"

/* nlohmann Json */

#include "nlohmann/json.hpp"


/* lib */

#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "xinput.lib")
#pragma comment(lib, "dinput8.lib")

#pragma comment(lib, "DirectXTex/DirectXTex.lib")
#pragma comment(lib, "Assimp/assimp-vc143-mt.lib")
#pragma comment(lib, "FreeType/freetype.lib")


/* Headers */

#include "Types.h"
#include "Defines.h"
#include "TLS.h"

/* Managers */

#include "Manager/BootSystem.h"

/* Reflection */

#include "Reflection/Reflection.h"
#include "Reflection/InterfaceReflector.h"
#include "Reflection/StructTypeInfo.h"
#include "Reflection/ObjectTypeInfo.h"
#include "Reflection/Method.h"
#include "Reflection/Property.h"
#include "Reflection/DefaultTypeInfo.h"
#include "Reflection/CommonTypeInfo.h"

/* Utils */

#include "Utils/TypeUtils.h"
#include "Utils/Wrapper/SimpleMathWrapper.h"
#include "Utils/StringUtils.h"

/* CommonType */

#include "Core/CommonType/Delegate.h"
#include "Core/CommonType/SoftObjectPtr.h"
#include "Core/CommonType/SubClass.h"

