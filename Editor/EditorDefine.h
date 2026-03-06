#pragma once

/* Log */

#ifdef _EDITOR

#define EDITOR_LOG(format, ...)  \
DEBUG_LOG(format, ##__VA_ARGS__)

#else

#define EDITOR_LOG(format, ...)  \
DEBUG_LOG(format, ##__VA_ARGS__)

#endif