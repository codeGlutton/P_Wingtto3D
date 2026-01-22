#pragma once

#include "Graphics/Viewport/Viewport.h"

/**
 * Scene을 보여주는 View 데이터 (렌더 스레드에서 Scene + Viewport 조합으로 드로우콜)
 */
class SceneViewport : public Viewport
{
	GEN_REFLECTION(SceneViewport)

public:

private:
	virtual void Render() override;

private:
	//std::map<int8, std::vector<>> _mLayer;
};

