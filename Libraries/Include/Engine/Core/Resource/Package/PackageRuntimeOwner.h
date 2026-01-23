#pragma once

class Package;

class IPackageRuntimeOwner abstract
{
public:
	virtual void RegisterPackage(std::shared_ptr<Package> package) = 0;

	virtual void Save() = 0;
	virtual void Load() = 0;
};
