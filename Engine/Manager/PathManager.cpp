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

#endif // NDEBUG

	_mConfigPath = _mRootPath / _mConfigFolderName;
	std::filesystem::create_directories(_mConfigPath);

	_mResourcePath = _mRootPath / _mResourceFolderName;
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
			_mResourcePath = _mRootPath / json["ResourcePath"].get<std::wstring>();
		}
		if (json.contains("EngineResourcePath") == true && json["EngineResourcePath"].is_string() == true)
		{
			_mEngineResourcePath = _mRootPath / json["EngineResourcePath"].get<std::wstring>();
		}
		if (json.contains("ExternalResourcePath") == true && json["ExternalResourcePath"].is_string() == true)
		{
			_mExternalResourcePath = _mRootPath / json["ExternalResourcePath"].get<std::wstring>();
		}
	}
	else
	{
		std::ofstream outStream(_mConfigPath / _mPathConfigFileName);

		Json json;
		json["ResourcePath"] = _mResourcePath.wstring();
		json["EngineResourcePath"] = _mEngineResourcePath.wstring();
		json["ExternalResourcePath"] = _mExternalResourcePath.wstring();

		outStream << json;
	}

	std::filesystem::create_directories(_mResourcePath);
	std::filesystem::create_directories(_mEngineResourcePath);
	std::filesystem::create_directories(_mExternalResourcePath);
}

void PathManager::Destroy()
{
}
