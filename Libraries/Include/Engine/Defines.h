#pragma once

#define STATIC_ASSERT_MSG(expr, msg) static_assert((expr) && (msg))
#define STATIC_FAIL_MSG(msg) STATIC_ASSERT_MSG(false, msg)
#define ASSERT_MSG(expr, msg) assert((expr) && (msg))
#define FAIL_MSG(msg) ASSERT_MSG(false, msg)

/* Window */

#define CHECK(hr)	assert(SUCCEEDED(hr))

