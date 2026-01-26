#pragma once

#define	DIRECTINPUT_VERSION	0x0800

#ifdef _EDITOR
#ifdef _DEBUG

#define BUILD_CONFIG_STR		"EditorDebug"
#define BUILD_CONFIG_STR_W		L"EditorDebug"

#else

#define BUILD_CONFIG_STR		"EditorRelease"
#define BUILD_CONFIG_STR_W		L"EditorRelease"

#endif
#else

#ifdef _DEBUG

#define BUILD_CONFIG_STR		"Debug"
#define BUILD_CONFIG_STR_W		L"Debug"

#else

#define BUILD_CONFIG_STR		"Release"
#define BUILD_CONFIG_STR_W		L"Release"

#endif
#endif

#define STATIC_ASSERT_MSG(expr, msg)	static_assert((expr) && (msg))
#define STATIC_FAIL_MSG(msg)			STATIC_ASSERT_MSG(false, msg)
#define ASSERT_MSG(expr, msg)			assert((expr) && (msg))
#define FAIL_MSG(msg)					ASSERT_MSG(false, msg)

#define ASSERT(expr)					assert(expr)
#define FAIL(expr)						assert(false)

/* Window */

#define CHECK_WIN(hr)					assert(SUCCEEDED(hr))

/* Thread */

#define ASSERT_THREAD(type)				ASSERT_MSG(LThreadId == type, "Thread isn't allowed")

