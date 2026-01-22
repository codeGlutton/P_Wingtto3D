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

///* Assimp */
//
//#include <Assimp/Importer.hpp>
//#include <Assimp/scene.h>
//#include <Assimp/postprocess.h>

/* nlohmann Json */

#include "nlohmann/json.hpp"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")

#pragma comment(lib, "DirectXTex/DirectXTex.lib")
//#pragma comment(lib, "Assimp/assimp-vc143-mt.lib")

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
#include "Utils/StringUtils.h"
#include "Utils/Debugger/Logger.h"

/* Others */

#include "Core/CommonType/Delegate.h"
#include "Core/CommonType/SoftObjectPtr.h"
#include "Core/CommonType/SubClass.h"
