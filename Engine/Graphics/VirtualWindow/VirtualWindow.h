#pragma once

#include "Graphics/Widget/CompoundWidget.h"

/**
 * 윈도우 창 내부 공간 영역 베이스 클래스
 */
class VirtualWindow abstract : public CompoundWidget
{
	GEN_ABSTRACT_REFLECTION(VirtualWindow)

	friend class AppWindow;
};

