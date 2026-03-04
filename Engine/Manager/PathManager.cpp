#include "pch.h"
#include "PathManager.h"

PathManager::PathManager()
{
#if defined(NDEBUG) && defined(NEDITOR)

	WCHAR exePath[_MAX_PATH];
	GetModuleFileNameW(NULL, exePath, _MAX_PATH);
	_mRootPath = exePath;
	_mRootPath = _mRootPath.parent_path();

#else

	WCHAR exePath[_MAX_PATH];
	GetModuleFileNameW(NULL, exePath, _MAX_PATH);
	_mRootPath = exePath;
	_mRootPath = _mRootPath.parent_path().parent_path().parent_path().parent_path();

#endif // defined(NDEBUG) && defined(NEDITOR)

	_mConfigPath = _mRootPath / _mConfigFolderName;
	std::filesystem::create_directories(_mConfigPath);

	_mResourcePath = _mRootPath / _mResourceFolderName;
	_mIsolationResourcePath = _mResourcePath / BUILD_CONFIG_STR_W;
	_mEngineResourcePath = _mResourcePath / _mEngineResourceFolderName;
	_mExternalResourcePath = _mResourcePath / _mExternalResourceFolderName;
}

PathManager::~PathManager()
{
}

void PathManager::Init()
{
	using Json = nlohmann::json;

	std::ifstream inStream(_mConfigPath / _mPathConfigFileName);
	if (inStream.is_open() == true)
	{
		Json json;
		inStream >> json;

		if (json.contains("ResourcePath") == true && json["ResourcePath"].is_string() == true)
		{
			_mResourcePath = _mRootPath / ConvertUtf8ToWString(json["ResourcePath"].get<std::string>());
		}
		if (json.contains("EngineResourcePath") == true && json["EngineResourcePath"].is_string() == true)
		{
			_mEngineResourcePath = _mRootPath / ConvertUtf8ToWString(json["EngineResourcePath"].get<std::string>());
		}
		if (json.contains("ExternalResourcePath") == true && json["ExternalResourcePath"].is_string() == true)
		{
			_mExternalResourcePath = _mRootPath / ConvertUtf8ToWString(json["ExternalResourcePath"].get<std::string>());
		}
		_mIsolationResourcePath = _mResourcePath / BUILD_CONFIG_STR_W;
	}
	else
	{
		std::ofstream outStream(_mConfigPath / _mPathConfigFileName);

		std::filesystem::path relativePath = _mResourceFolderName;

		Json json;
		json["ResourcePath"] =			ConvertWStringToUtf8(relativePath);
		json["EngineResourcePath"] =	ConvertWStringToUtf8(relativePath / _mEngineResourceFolderName);
		json["ExternalResourcePath"] =	ConvertWStringToUtf8(relativePath / _mExternalResourceFolderName);

		outStream << json;
	}

	std::filesystem::create_directories(_mResourcePath);
	std::filesystem::create_directories(_mIsolationResourcePath);
	std::filesystem::create_directories(_mEngineResourcePath);
	std::filesystem::create_directories(_mExternalResourcePath);
}

void PathManager::Destroy()
{
}

void PathManager::ExtractFromFullPath(const std::wstring& fullPath, OUT std::wstring& name, OUT std::wstring& packagePath) const
{
	std::filesystem::path path(fullPath);
	name = path.extension().wstring();
	if (name.empty() == false && name.front() == L'.')
	{
		name.erase(0, 1);
	}
	packagePath = path.replace_extension().wstring();
}
