#include "pch.h"
#include "GUID.h"

#include <combaseapi.h>

ObjectGUID ObjectGUID::NewGUID()
{
	GUID winGuid;
	CHECK(CoCreateGuid(&winGuid));

	ObjectGUID out;
	out.mA = winGuid.Data1;										// 4바이트
	out.mB = (uint32(winGuid.Data2) << 16) | winGuid.Data3;		// 2 + 2바이트
	std::memcpy(&out.mC, winGuid.Data4, 8);						// 8바이트
	return out;
}
