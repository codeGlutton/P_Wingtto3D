#pragma once

#include <fstream>
#include <filesystem>

#include "Core/GUID.h"

struct ObjectLinker;
struct PackageLinkData;
struct BulkData;

enum class ArchiveMemoryTarget : uint8
{
	Data = 0,
	Header,
};

class Archive
{
public:
	Archive(const std::filesystem::path& packagePath, ArchiveMemoryTarget memoryTarget = ArchiveMemoryTarget::Data);

public:
	void LoadPackage();
	void SavePackage();

public:
	template<typename T>
	Archive& operator<<(T&& inst);
	template<typename T>
	Archive& operator>>(T&& inst);

public:
	void ChangeTarget(ArchiveMemoryTarget memoryTarget);

public:
	void BindGUIDFinder(std::function<ObjectGUID(const Object*)> finder)
	{
		_mGUIDFinder = finder;
	}
	void BindObjectLinker(std::shared_ptr<ObjectLinker> linker)
	{
		_mObjLinker = linker;
	}

public:
	ObjectGUID ConvertGUID(const Object* object) const
	{
		return _mGUIDFinder(object);
	}
	std::shared_ptr<ObjectLinker>& GetObjectLinker()
	{
		return _mObjLinker;
	}
	const std::shared_ptr<const ObjectLinker>& GetObjectLinker() const
	{
		return _mObjLinker;
	}

public:
	const std::filesystem::path& GetPackagePath() const
	{
		return _mPackagePath;
	}
	PackageLinkData& GetLinkData();
	PackageLinkData& GetLinkData(const std::wstring& packagePath);
	const PackageLinkData& GetLinkData() const;
	const PackageLinkData& GetLinkData(const std::wstring& packagePath) const;

	std::vector<std::shared_ptr<BulkData>>& GetBulkDatas();
	std::vector<std::shared_ptr<BulkData>>& GetLinkDatas(const std::wstring& packagePath);
	const std::vector<std::shared_ptr<BulkData>>& GetBulkDatas() const;
	const std::vector<std::shared_ptr<BulkData>>& GetLinkDatas(const std::wstring& packagePath) const;

private:
	std::fstream _mStream;
	std::vector<char> _mHeader;
	std::vector<char> _mData;

private:
	std::filesystem::path _mPackagePath;
	std::vector<char>* _mCurrentMemory;
	std::size_t _mCurrentIndex;

private:
	std::function<ObjectGUID(const Object*)> _mGUIDFinder;
	std::shared_ptr<ObjectLinker> _mObjLinker;
};

template<typename T>
inline Archive& Archive::operator<<(T&& inst)
{
	std::size_t dataSize = sizeof(T);
	_mCurrentMemory->resize(_mCurrentIndex + dataSize);
	std::memcpy(_mCurrentMemory->data() + _mCurrentIndex, &inst, dataSize);
	_mCurrentIndex += dataSize;

	return *this;
}

template<typename T>
inline Archive& Archive::operator>>(T&& inst)
{
	std::size_t dataSize = sizeof(T);
	std::memcpy(&inst, _mCurrentMemory->data() + _mCurrentIndex, dataSize);
	_mCurrentIndex += dataSize;

	return *this;
}


