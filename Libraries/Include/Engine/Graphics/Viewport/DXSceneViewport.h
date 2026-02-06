#pragma once

#include "Graphics/Viewport/DXViewport.h"

/**
 * 렌더 스레드에서 Scene + Camera 조합으로 드로우콜
 */
struct DXSceneViewport : public DXViewport
{
	GEN_STRUCT_REFLECTION(DXSceneViewport)

public:
};

