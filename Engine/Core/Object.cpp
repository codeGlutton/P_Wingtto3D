#include "pch.h"
#include "Object.h"

#include "Core/Archive.h"
#include "Core/Resource/Package/Package.h"

void Object::PostCreate()
{
	_mIsAlive = true;

	// 오브젝트 매니저에 이름 등록
	OBJECT_MANAGER->NotifyToAddObject(GetFullPath());
	if (_mOuter != nullptr)
	{
		// 패키징에 등록
		_mOuter->NotifyToAddChild(shared_from_this());
	}
}

void Object::PostLoad()
{
}

void Object::BeginDestroy()
{
	if (_mOuter != nullptr)
	{
		// 패키징에서 내리기
		_mOuter->NotifyToRemoveChild(GetFullPath());
	}
	// 오브젝트 매니저에 이름 내리기
	OBJECT_MANAGER->NotifyToRemoveObject(GetFullPath());
	
	_mIsAlive = false;
}

void Object::SetName(const std::wstring& name)
{
	_mName = name;
	OBJECT_MANAGER->RequestToRename(_mPath, _mName);
	_mFullPath = _mPath + L'.' + _mName;

	OBJECT_MANAGER->NotifyToRemoveObject(GetFullPath());
	OBJECT_MANAGER->NotifyToAddObject(GetFullPath());
}

void Object::CollectHeaderDatas(const void* inst, OUT std::unordered_map<std::wstring, std::pair<std::string, PackageBuildScope>>& externalPackageDatas, OUT std::vector<std::shared_ptr<BulkData>>& bulkDatas) const
{
	GetTypeInfo().CollectHeaderDatas(inst, externalPackageDatas, bulkDatas);
}

void Object::Serialize(Archive& archive) const
{
	GetTypeInfo().Serialize(archive, this);
}

void Object::Deserialize(Archive& archive)
{
	GetTypeInfo().Deserialize(archive, this);
}

