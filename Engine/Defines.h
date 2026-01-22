#pragma once

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

