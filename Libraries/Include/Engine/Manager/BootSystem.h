#pragma once

#define BOOT_SYSTEM BootSystem::GetInst()

enum class BootPhase : uint8
{
	None = 0,
	TypeInfoReady,
	TypeMemberReady,
	CDOReady,
	Complete
};

class BootSystem
{
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
	void AddType(std::function<void()>&& typeInfoRegisrer, std::function<void()>&& cdoCreator);
	void AddTypeMember(std::function<void()>&& memberInfoRegister);

public:
	void Boot();

private:
	void CreateTypeInfos();
	void CreateTypeMemberInfos();
	void CreateCDOs();

private:
	std::vector<std::function<void()>> _mTypeInfoRegisters;
	std::vector<std::function<void()>> _mMemberInfoRegisters;
	std::vector<std::function<void()>> _mCDOCreators;

private:
	BootPhase _mPhase;
};

