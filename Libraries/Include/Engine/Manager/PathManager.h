#pragma once

#define PATH_MANAGER PathManager::GetInst()

class PathManager
{
private:
	PathManager();
	~PathManager();

public:
	static PathManager* GetInst()
	{
		static PathManager inst;
		return &inst;
	}

public:
	void Init();
	void Destroy();

public:
	const std::filesystem::path& GetRootPath() const
	{
		return _mRootPath;
	}
	const std::filesystem::path& GetConfigPath() const
	{
		return _mConfigPath;
	}

	const std::filesystem::path& GetResourcePath() const
	{
		return _mResourcePath;
	}
	const std::filesystem::path& GetIsolationResourcePath() const
	{
		return _mIsolationResourcePath;
	}
	const std::filesystem::path& GetEngineResourcePath() const
	{
		return _mEngineResourcePath;
	}
	const std::filesystem::path& GetExternalResourcePath() const
	{
		return _mExternalResourcePath;
	}

public:
	const std::wstring& GetConfigFolderName() const
	{
		return _mConfigFolderName;
	}
	const std::wstring& GetResourceFolderName() const
	{
		return _mResourceFolderName;
	}
	const std::wstring& GetEngineResourceFolderName() const
	{
		return _mEngineResourceFolderName;
	}
	const std::wstring& GetExternalResourceFolderName() const
	{
		return _mExternalResourceFolderName;
	}

private:
	std::filesystem::path _mRootPath;
	std::filesystem::path _mConfigPath;

private:
	std::filesystem::path _mResourcePath;
	std::filesystem::path _mIsolationResourcePath;
	std::filesystem::path _mEngineResourcePath;
	std::filesystem::path _mExternalResourcePath;

private:
	const std::wstring _mConfigFolderName = L"Config";
	const std::wstring _mResourceFolderName = L"Resources";
	const std::wstring _mEngineResourceFolderName = L"Engine";
	const std::wstring _mExternalResourceFolderName = L"External";

private:
	const std::wstring _mPathConfigFileName = L"EnginePath.ini";
};

