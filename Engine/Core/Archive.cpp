#include "pch.h"
#include "Archive.h"

#include "Manager/PathManager.h"

#include "Core/ObjectLinker.h"
#include "Core/Resource/Package.h"

Archive::Archive(const std::filesystem::path& packagePath, ArchiveMemoryTarget memoryTarget) :
	_mStream(),
	_mPackagePath(packagePath),
	_mCurrentMemory(memoryTarget == ArchiveMemoryTarget::Header ? &_mHeader : &_mData),
	_mCurrentIndex(0ull),
	_mObjLinker(nullptr)
{
}

void Archive::LoadPackage()
{
	std::shared_ptr<PackageHeader> packageHeader = std::make_shared<PackageHeader>();

	std::wstring filePath = (PATH_MANAGER->GetResourcePath() / _mPackagePath).replace_extension(L".pak").wstring();
	_mStream.open(filePath, std::ios::in | std::ios::binary);
	if (_mStream.is_open() == true)
	{
		// 파일 읽기
		{
			std::size_t headerSize = 0ull;
			_mStream.read(reinterpret_cast<char*>(&headerSize), sizeof(std::size_t));
			_mHeader.resize(headerSize);

			// 헤더까지 읽기
			_mStream.read(_mHeader.data(), _mHeader.size());
		}
		{
			std::size_t dataSize = 0ull;
			_mStream.read(reinterpret_cast<char*>(&dataSize), sizeof(std::size_t));
			_mData.resize(dataSize);

			// 이후 데이터 읽기
			_mStream.read(_mData.data(), _mData.size());
		}
		_mStream.close();

		// 헤더 읽어두기
		ChangeTarget(ArchiveMemoryTarget::Header);
		packageHeader->Deserialize(*this);
	}

	if (_mObjLinker != nullptr)
	{
		_mObjLinker->mLinkDataMap[_mPackagePath].mPackageHeader = packageHeader;
	}
}

void Archive::SavePackage()
{
	if (_mObjLinker == nullptr || _mObjLinker->mLinkDataMap[_mPackagePath].mPackageHeader == nullptr)
	{
		return;
	}

	// 헤더 쓰기
	ChangeTarget(ArchiveMemoryTarget::Header);
	_mObjLinker->mLinkDataMap[_mPackagePath].mPackageHeader->Serialize(*this);

	std::wstring filePath = (PATH_MANAGER->GetResourcePath() / _mPackagePath).replace_extension(L".pak").wstring();
	_mStream.open(filePath, std::ios::out | std::ios::trunc | std::ios::binary);
	ASSERT_MSG(_mStream.is_open() == true, "Fail to save package");

	// 파일 쓰기
	{
		std::size_t headerSize = _mHeader.size();
		_mStream.write(reinterpret_cast<char*>(&headerSize), sizeof(std::size_t));

		// 헤더까지 쓰기
		_mStream.write(_mHeader.data(), _mHeader.size());
	}
	{
		std::size_t dataSize = _mData.size();
		_mStream.write(reinterpret_cast<char*>(&dataSize), sizeof(std::size_t));

		// 이후 데이터 쓰기
		_mStream.write(_mData.data(), _mData.size());
	}
	_mStream.close();
}

void Archive::ChangeTarget(ArchiveMemoryTarget memoryTarget)
{
	_mCurrentMemory = memoryTarget == ArchiveMemoryTarget::Header ? &_mHeader : &_mData;
	_mCurrentIndex = 0ull;
}

PackageLinkData& Archive::GetLinkData()
{
	return _mObjLinker->mLinkDataMap[_mPackagePath];
}

PackageLinkData& Archive::GetLinkData(const std::wstring& packagePath)
{
	return _mObjLinker->mLinkDataMap[packagePath];
}

const PackageLinkData& Archive::GetLinkData() const
{
	return _mObjLinker->mLinkDataMap[_mPackagePath];
}

const PackageLinkData& Archive::GetLinkData(const std::wstring& packagePath) const
{
	return _mObjLinker->mLinkDataMap[packagePath];
}

std::vector<std::shared_ptr<BulkData>>& Archive::GetBulkDatas()
{
	return GetLinkData().mPackageHeader->mBulkDatas;
}

std::vector<std::shared_ptr<BulkData>>& Archive::GetLinkDatas(const std::wstring& packagePath)
{
	return GetLinkData(packagePath).mPackageHeader->mBulkDatas;
}

const std::vector<std::shared_ptr<BulkData>>& Archive::GetBulkDatas() const
{
	return GetLinkData().mPackageHeader->mBulkDatas;
}

const std::vector<std::shared_ptr<BulkData>>& Archive::GetLinkDatas(const std::wstring& packagePath) const
{
	return GetLinkData(packagePath).mPackageHeader->mBulkDatas;
}

