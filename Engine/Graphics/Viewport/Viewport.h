#pragma once

#include "GraphicMinimum.h"

/**
 * 창을 표기할 viewport 베이스 클래스 (렌더 스레드에서 드로우콜)
 */
class Viewport
{
public:
	Viewport(float width, float height, float x = 0, float y = 0, float minDepth = 0, float maxDepth = 1);
	~Viewport();

private:
	D3D11_VIEWPORT _mDesc = { 0 };
};

