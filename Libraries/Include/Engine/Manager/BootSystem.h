#pragma once

#define BOOT_SYSTEM BootSystem::GetInst()

class StructTypeInfo;
class ObjectTypeInfo;

enum class BootPhase : uint8
{
	None = 0,
	TypeInfoReady,
	TypeMemberReady,
	CDOReady,
};

class BootSystem
{
	using StructTypeInfoMap = std::unordered_map<std::string_view, const StructTypeInfo*>;
	using ObjectTypeInfoMap = std::unordered_map<std::string_view, const ObjectTypeInfo*>;

private:
	BootSystem();
	~BootSystem();

public:
	static BootSystem* GetInst()
	{
		static BootSystem inst;
		return &inst;
	}

public:
	const StructTypeInfo* GetStructTypeInfo(const char* name) const;
	const ObjectTypeInfo* GetObjectTypeInfo(const char* name) const;

public:
	void AddType(std::function<const StructTypeInfo*()> typeInfoRegister);
	void AddType(std::function<const ObjectTypeInfo*()> typeInfoRegister, std::function<void()> cdoCreator);
	void AddTypeMember(std::function<void()> memberInfoRegister);

public:
	void Boot();

private:
	void CreateTypeInfos();
	void CreateTypeMemberInfos();
	void CreateCDOs();
	void CreateInstances();

private:
	std::vector<std::function<const StructTypeInfo*()>> _mStructTypeInfoRegisters;
	std::vector<std::function<const ObjectTypeInfo*()>> _mObjectTypeInfoRegisters;
	std::vector<std::function<void()>> _mMemberInfoRegisters;
	std::vector<std::function<void()>> _mCDOCreators;

private:
	StructTypeInfoMap _mStructTypeInfoMap;
	ObjectTypeInfoMap _mObjectTypeInfoMap;

private:
	BootPhase _mPhase;
};

