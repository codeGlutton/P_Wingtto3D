#include "pch.h"
#include "BootSystem.h"

BootSystem::BootSystem() :
	_mPhase(BootPhase::None)
{
}

BootSystem::~BootSystem()
{
}

void BootSystem::AddType(std::function<void()>&& typeInfoRegisrer, std::function<void()>&& cdoCreator)
{
	_mCDOCreators.push_back(cdoCreator);
	_mTypeInfoRegisters.push_back(typeInfoRegisrer);
	if (_mPhase >= BootPhase::TypeInfoReady)
	{
		CreateTypeInfos();
		if (_mPhase >= BootPhase::CDOReady)
		{
			CreateCDOs();
		}
	}
}

void BootSystem::AddTypeMember(std::function<void()>&& memberInfoRegister)
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
}

void BootSystem::CreateTypeInfos()
{
	for (const std::function<void()>& typeInfoRegister : _mTypeInfoRegisters)
	{
		typeInfoRegister();
	}
	_mTypeInfoRegisters.clear();
}

void BootSystem::CreateTypeMemberInfos()
{
	for (const std::function<void()>& memberInfoRegister : _mMemberInfoRegisters)
	{
		memberInfoRegister();
	}
	_mMemberInfoRegisters.clear();
}

void BootSystem::CreateCDOs()
{
	for (const std::function<void()>& cdoCreator : _mCDOCreators)
	{
		cdoCreator();
	}
	_mCDOCreators.clear();
}
