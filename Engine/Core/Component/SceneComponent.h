#pragma once

#include "Core/Component/ActorComponent.h"
#include "Core/Transform.h"

class SceneComponent : public ActorComponent
{
	GEN_REFLECTION(SceneComponent)

private:
	PROPERTY(mAttachParent)
	std::weak_ptr<SceneComponent> mAttachParent;

	std::vector<std::shared_ptr<SceneComponent>> mAttachChildren;

private:
	Transform mTransform;
};

