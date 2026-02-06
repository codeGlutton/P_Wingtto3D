#pragma once

#include "Manager/WorldManagerBase.h"

#include "Input/InputInclude.h"
#include "Input/InputBinding.h"
#include "Input/InputAction.h"
#include "Input/InputMapping.h"

#include "Manager/InputManager.h"

#define WORLD_INPUT_MANAGER WorldGameInputManager::GetInst()

/**
 * 월드 내 게임 입력 관리자
 */
class WorldGameInputManager : public WorldManagerBase
{
	using BindingVector = std::vector<std::shared_ptr<InputBinding>>;

private:
	WorldGameInputManager();
	~WorldGameInputManager();

public:
	static WorldGameInputManager* GetInst()
	{
		static WorldGameInputManager inst;
		return &inst;
	}

public:
	virtual void Init(std::shared_ptr<World> world) override;
	virtual void Update(float deltaTime);
	virtual void Destroy() override;

public:
	KeyState::Type GetKeyState(KeyType::Type key) const
	{
		return INPUT_MANAGER->GetKeyState(key);
	}
	const POINT& GetMousePos() const
	{
		return INPUT_MANAGER->GetMousePos();
	}
	template<typename T>
	const T& GetKeyScale(KeyType::Type key) const
	{
		return INPUT_MANAGER->GetKeyScale<T>(key);
	}

public:
	std::shared_ptr<const InputBinding> Bind(KeyType::Type bindKey, bool withCtrl, bool withAlt, bool withShift, std::shared_ptr<Object> object, OnCallInputBinding::Binder binder);
	DelegateHandle BindAction(SubClass<InputAction> action, KeyState::Type eventState, std::shared_ptr<Object> object, OnTiggerInputAction::Binder binder);
	bool Unbind(std::shared_ptr<const InputBinding> target);
	bool UnbindAction(SubClass<InputAction> action, KeyState::Type eventState, DelegateHandle handle);

public:
	void AddMappingContext(SubClass<InputMappingContext> context, uint8 priority = 0);
	void RemoveMappingContext(SubClass<InputMappingContext> context);

private:
	void UpdateKeyBindingCallbacks();

private:
	void ChangeTopMappingContextBindings(uint8 prePriority, uint8 curPriority);
	void AddTopMappingContextBindings(uint8 priority);
	void RemoveTopMappingContextBindings(uint8 priority);

private:
	std::shared_ptr<GameInputState> _mState;

	/* 키 상태에 따른 함수 바인딩 */
private:
	std::map<uint8, std::unordered_map<const InputMappingContext*, std::vector<std::shared_ptr<InputBinding>>>, std::greater<uint8>> _mMappingContexts;
	std::unordered_map<const InputAction*, std::array<OnTiggerInputAction, KeyState::Count>> _mActionMap;
	std::unordered_map<KeyType::Type, BindingVector> _mBindingMap;
};

