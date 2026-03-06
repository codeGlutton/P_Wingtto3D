#pragma once

#include "Graphics/Resource/DXResourceInclude.h"

class DXResource : public std::enable_shared_from_this<DXResource>
{
public:
	virtual ~DXResource() = default;

public:
	inline bool IsUpdatable() const
	{
		return _mResourceFlags & DXResourceFlag::Updatable;
	}
	inline bool HasOutput() const
	{
		return _mResourceFlags & DXResourceFlag::HasOutput;
	}

protected:
	DXResourceFlag::Type _mResourceFlags = DXResourceFlag::None;
};

class DXSharedResource : public DXResource
{
	friend class ResourceManager;

protected:
	DXSharedResource();
	~DXSharedResource();

protected:
	virtual void PostCreate();

public:
	DXSharedResourceType::Type GetSharedResourceType() const
	{
		return _mSharedResourceType;
	}
	const std::wstring& GetName() const
	{
		return _mName;
	}
	const std::wstring& GetPath() const
	{
		return _mPath;
	}

protected:
	DXSharedResourceType::Type _mSharedResourceType = DXSharedResourceType::None;

private:
	std::wstring _mName;
	std::wstring _mPath;
};


