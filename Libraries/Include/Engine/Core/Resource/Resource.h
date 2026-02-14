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

public:
	virtual void Save() const;
	virtual void SaveAsync(std::function<void()> callback) const;

private:
	void MakeHeader();

private:
	PROPERTY(_mHeader)
	std::shared_ptr<ResourceHeader> _mHeader;
};

