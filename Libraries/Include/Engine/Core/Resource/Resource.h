#pragma once

#include "Core/Object.h"

class ResourceHeader;

class Resource abstract : public Object
{
	GEN_ABSTRACT_REFLECTION(Resource)

	friend class ResourceManager;

protected:
	virtual void PostCreate() override;
	virtual void BeginDestroy() override;

private:
	std::shared_ptr<ResourceHeader> _mHeader;
};

