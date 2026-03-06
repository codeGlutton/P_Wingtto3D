#pragma once

#include "Core/Object.h"

class ResourceHeader;

class Resource abstract : public Object
{
	GEN_ABSTRACT_REFLECTION(Resource)

	friend class ResourceManager;

protected:
	virtual void PostLoad() override;
	virtual void BeginDestroy() override;

public:
	virtual bool HasBulkData() const;

public:
	virtual void Save() const;
	virtual void SaveAsync(std::function<void()> callback) const;

private:
	/**
	 * 초기 리소스 생성 시에 헤더 제작 함수
	 */
	void MakeHeader();

private:
	PROPERTY(_mHeader)
	std::shared_ptr<ResourceHeader> _mHeader;
};

