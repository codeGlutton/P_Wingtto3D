#include "pch.h"
#include "BootSystem.h"

#include "Manager/ObjectManager.h"

#include "Reflection/ObjectTypeInfo.h"

BootSystem::BootSystem() :
	_mPhase(BootPhase::None)
{
}

BootSystem::~BootSystem()
{
}

const StructTypeInfo* BootSystem::GetStructTypeInfo(const char* name) const
{
	auto iter = _mStructTypeInfoMap.find(name);
	if (iter != _mStructTypeInfoMap.end())
	{
		return iter->second;
	}
	return nullptr;
}

const ObjectTypeInfo* BootSystem::GetObjectTypeInfo(const char* name) const
{
	auto iter = _mObjectTypeInfoMap.find(name);
	if (iter != _mObjectTypeInfoMap.end())
	{
		return iter->second;
	}
	return nullptr;
}

void BootSystem::AddType(std::function<const StructTypeInfo*()> typeInfoRegister)
{
	_mStructTypeInfoRegisters.push_back(typeInfoRegister);
	if (_mPhase >= BootPhase::TypeInfoReady)
	{
		CreateTypeInfos();
	}
}

void BootSystem::AddType(std::function<const ObjectTypeInfo*()> typeInfoRegister, std::function<void()> cdoCreator)
{
	_mObjectTypeInfoRegisters.push_back(typeInfoRegister);
	_mCDOCreators.push_back(cdoCreator);
	if (_mPhase >= BootPhase::TypeInfoReady)
	{
		CreateTypeInfos();
		if (_mPhase >= BootPhase::CDOReady)
		{
			CreateCDOs();
		}
	}
}

void BootSystem::AddTypeMember(std::function<void()> memberInfoRegister)
{
	_mMemberInfoRegisters.push_back(memberInfoRegister);
	if (_mPhase >= BootPhase::TypeMemberReady)
	{
		CreateTypeMemberInfos();
	}
}

void BootSystem::Boot()
{
	if (_mPhase != BootPhase::None)
	{
		return;
	}
	CreateTypeInfos();
	CreateTypeMemberInfos();
	CreateCDOs();
	CreateInstances();
}

void BootSystem::CreateTypeInfos()
{
	for (const std::function<const StructTypeInfo*()>& typeInfoRegister : _mStructTypeInfoRegisters)
	{
		const StructTypeInfo* structTypeInfo = typeInfoRegister();
		_mStructTypeInfoMap.insert(std::make_pair(structTypeInfo->GetName(), structTypeInfo));
	}
	_mStructTypeInfoRegisters.clear();
	for (const std::function<const ObjectTypeInfo*()>& typeInfoRegister : _mObjectTypeInfoRegisters)
	{
		const ObjectTypeInfo* objectTypeInfo = typeInfoRegister();
		_mObjectTypeInfoMap.insert(std::make_pair(objectTypeInfo->GetName(), objectTypeInfo));
	}
	_mObjectTypeInfoRegisters.clear();
	_mPhase = BootPhase::TypeInfoReady;
}

void BootSystem::CreateTypeMemberInfos()
{
	for (const std::function<void()>& memberInfoRegister : _mMemberInfoRegisters)
	{
		memberInfoRegister();
	}
	_mMemberInfoRegisters.clear();
	_mPhase = BootPhase::TypeMemberReady;
}

void BootSystem::CreateCDOs()
{
	for (const std::function<void()>& cdoCreator : _mCDOCreators)
	{
		cdoCreator();
	}
	_mCDOCreators.clear();
	_mPhase = BootPhase::CDOReady;
}

void BootSystem::CreateInstances()
{
	//OBJECT_MANAGER->CreateObject
}
