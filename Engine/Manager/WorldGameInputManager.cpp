#include "pch.h"
#include "WorldGameInputManager.h"

#include "Utils/InputUtils.h"

WorldGameInputManager::WorldGameInputManager()
{
}

WorldGameInputManager::~WorldGameInputManager()
{
}

void WorldGameInputManager::Init(std::shared_ptr<World> world)
{
    WorldManagerBase::Init(world);

    _mState = std::make_shared<GameInputState>();
}

void WorldGameInputManager::Update(float deltaTime)
{
    _mState->mAccInputTime += deltaTime;
    UpdateKeyBindingCallbacks();
}

void WorldGameInputManager::Destroy()
{
    _mMappingContexts.clear();
    _mActionMap.clear();
    _mBindingMap.clear();

    _mState.reset();

    WorldManagerBase::Destroy();
}

std::shared_ptr<const InputBinding> WorldGameInputManager::Bind(KeyType::Type bindKey, bool withCtrl, bool withAlt, bool withShift, std::shared_ptr<Object> object, OnCallInputBinding::Binder binder)
{
    std::shared_ptr<InputBinding> binding = std::make_shared<InputBinding>();

    binding->mCtrl = withCtrl;
    binding->mAlt = withAlt;
    binding->mShift = withShift;
    binding->mOnCallInputBinding.Bind(object, binder);

    _mBindingMap[bindKey].push_back(binding);

    return binding;
}

DelegateHandle WorldGameInputManager::BindAction(SubClass<InputAction> action, KeyState::Type eventState, std::shared_ptr<Object> object, OnTiggerInputAction::Binder binder)
{
    if (action.IsValid() == false)
    {
        return 0;
    }

    std::shared_ptr<const InputAction> cdo = std::static_pointer_cast<const InputAction>(action->GetDefaultObject());
    DelegateHandle handle = _mActionMap[cdo.get()][eventState].Bind(object, binder);

    return handle;
}

bool WorldGameInputManager::Unbind(std::shared_ptr<const InputBinding> target)
{
    BindingVector& bindings = _mBindingMap[target->mKeyType];
    for (std::size_t i = 0; i < bindings.size(); ++i)
    {
        std::shared_ptr<InputBinding>& binding = bindings[i];
        if (binding == target)
        {
            bindings[i] = std::move(bindings.back());
            bindings.pop_back();
            if (bindings.empty() == true)
            {
                _mBindingMap.erase(target->mKeyType);
            }
            return true;
        }
    }
    return false;
}

bool WorldGameInputManager::UnbindAction(SubClass<InputAction> action, KeyState::Type eventState, DelegateHandle handle)
{
    if (action.IsValid() == false)
    {
        return false;
    }
    std::shared_ptr<const InputAction> cdo = std::static_pointer_cast<const InputAction>(action->GetDefaultObject());
    return _mActionMap[cdo.get()][eventState].Unbind(handle);
}

void WorldGameInputManager::AddMappingContext(SubClass<InputMappingContext> context, uint8 priority)
{
    if (context.IsValid() == false)
    {
        return;
    }
    std::shared_ptr<const InputMappingContext> contextCdo = std::static_pointer_cast<const InputMappingContext>(context->GetDefaultObject());

    auto iter = _mMappingContexts.find(priority);
    if (iter == _mMappingContexts.end() || iter->second.find(contextCdo.get()) != iter->second.end())
    {
        return;
    }

    uint8 prePriority = _mMappingContexts.begin() != _mMappingContexts.end() ? 0 : _mMappingContexts.begin()->first;
    for (const InputMapping& mapping : contextCdo->_mMappings)
    {
        std::shared_ptr<const InputAction> actionCdo = std::static_pointer_cast<const InputAction>(context->GetDefaultObject());
        for (const InputBinding& binding : mapping.mBindings)
        {
            std::shared_ptr<InputBinding> bindingInst = std::make_shared<InputBinding>(binding);
            bindingInst->mOnCallInputBinding.BindLambda([actionCdo, this](const InputValue* value, KeyType::Type key) {
                auto iter = _mActionMap.find(actionCdo.get());
                if (iter != _mActionMap.end())
                {
                    InputValue filtered = value;

                    switch (actionCdo->_mValueType)
                    {
                    case KeyValueType::Bool:
                        filtered.Convert<bool>();
                        break;
                    case KeyValueType::Float:
                        filtered.Convert<float>();
                        break;
                    case KeyValueType::Vec2:
                        filtered.Convert<Vec2>();
                        break;
                    }

                    iter->second[GetKeyState(key)].Multicast(&filtered);
                }
                });

            _mMappingContexts[priority][contextCdo.get()].push_back(bindingInst);
        }
    }

    if (prePriority <= priority)
    {
        ChangeTopMappingContextBindings(prePriority, priority);
    }
}

void WorldGameInputManager::RemoveMappingContext(SubClass<InputMappingContext> context)
{
    if (context.IsValid() == false)
    {
        return;
    }
    std::shared_ptr<const InputMappingContext> contextCdo = std::static_pointer_cast<const InputMappingContext>(context->GetDefaultObject());

    for (auto& contextPair : _mMappingContexts)
    {
        auto iter = contextPair.second.find(contextCdo.get());
        if (iter != contextPair.second.end())
        {
            uint8 prePriority = _mMappingContexts.begin() != _mMappingContexts.end() ? 0 : _mMappingContexts.begin()->first;
            if (prePriority == contextPair.first)
            {
                RemoveTopMappingContextBindings(prePriority);
                contextPair.second.erase(contextCdo.get());
                AddTopMappingContextBindings(prePriority);
            }
            else
            {
                contextPair.second.erase(contextCdo.get());
            }

            return;
        }
    }
}

void WorldGameInputManager::UpdateKeyBindingCallbacks()
{
    /*for (const InputEventLog& keyEventLog : _mState->mKeyEventLogs)
    {
        if (keyEventLog.mEvent->IsKeyEvent() == true)
        {

        }
        else
        {

        }
        keyEventLog.

    for (const auto& bindingPair : _mBindingMap)
    {
        const KeyType::Type& keyType = bindingPair.first;

        uint32 maxPriority = 0;
        for (const std::shared_ptr<InputBinding>& binding : bindingPair.second)
        {
            // 가장 우선 순위 높은 바인딩만 호출 
            // (ex. "컨트롤 + I" 입력 시, "컨트롤 + I" 바인딩 > "I" 바인딩)
            uint8 priority = binding->GetPriority(_mUseCtrl, _mUseAlt, _mUseShift);
            if (maxPriority < priority)
            {
                maxPriority = priority;
            }
        }

        for (const std::shared_ptr<InputBinding>& binding : bindingPair.second)
        {
            uint8 priority = binding->GetPriority(_mUseCtrl, _mUseAlt, _mUseShift);

            if (maxPriority == priority)
            {
                if (_mScaleValues.find(keyType) == _mScaleValues.end())
                {
                    InputValue inputValue(_mKeyStates[keyType] != KeyState::None);
                    binding->mOnCallInputBinding.ExecuteIfBound(&inputValue, _mKeyStates[keyType]);
                }
                else
                {
                    binding->mOnCallInputBinding.ExecuteIfBound(&_mScaleValues[keyType], _mKeyStates[keyType]);
                }
            }
        }
    }
    }*/
}

void WorldGameInputManager::ChangeTopMappingContextBindings(uint8 prePriority, uint8 curPriority)
{
    RemoveTopMappingContextBindings(prePriority);
    AddTopMappingContextBindings(curPriority);
}

void WorldGameInputManager::AddTopMappingContextBindings(uint8 priority)
{
    // 상위 컨텍스트 바인딩 적용
    if (_mMappingContexts.find(priority) != _mMappingContexts.end())
    {
        for (auto& contextPair : _mMappingContexts[priority])
        {
            for (const std::shared_ptr<InputBinding>& binding : contextPair.second)
            {
                _mBindingMap[binding->mKeyType].push_back(binding);
            }
        }
    }
}

void WorldGameInputManager::RemoveTopMappingContextBindings(uint8 priority)
{
    // 상위 컨텍스트 바인딩 제거
    if (_mMappingContexts.find(priority) != _mMappingContexts.end())
    {
        // 상위 맵핑 컨텍스트마다
        for (auto& contextPair : _mMappingContexts[priority])
        {
            // 컨텍스트 내 키 바인딩마다
            for (const std::shared_ptr<InputBinding>& target : contextPair.second)
            {
                // 후보 탐색
                BindingVector& bindings = _mBindingMap[target->mKeyType];
                for (std::size_t i = 0; i < bindings.size(); ++i)
                {
                    if (target == bindings[i])
                    {
                        bindings[i] = std::move(bindings.back());
                        bindings.pop_back();
                        if (bindings.empty() == true)
                        {
                            _mBindingMap.erase(target->mKeyType);
                        }

                        break;
                    }
                }
            }
        }
    }
}
