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

	/* 주소 */
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

	/* 폴더명 */
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
	// 메인 폴더
	std::filesystem::path _mRootPath;
	// 설정 폴더
	std::filesystem::path _mConfigPath;

private:
	// 리소스 폴더
	std::filesystem::path _mResourcePath;
	// 빌드 설정에 따른 리소스 폴더
	std::filesystem::path _mIsolationResourcePath;
	// 엔진 리소스 폴더
	std::filesystem::path _mEngineResourcePath;
	// 외부 리소스 폴더
	std::filesystem::path _mExternalResourcePath;

private:
	const std::wstring _mConfigFolderName = L"Config";
	const std::wstring _mResourceFolderName = L"Resources";
	const std::wstring _mEngineResourceFolderName = L"Engine";
	const std::wstring _mExternalResourceFolderName = L"External";

private:
	const std::wstring _mPathConfigFileName = L"EnginePath.ini";
};

