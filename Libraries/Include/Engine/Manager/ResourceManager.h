#pragma once

#define RESOURCE_MANAGER ResourceManager::GetInst()

class ResourceManager
{
private:
	ResourceManager();
	~ResourceManager();

public:
	static ResourceManager* GetInst()
	{
		static ResourceManager inst;
		return &inst;
	}
};

