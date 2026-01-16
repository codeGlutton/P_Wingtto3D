#pragma once

#include "Core/Object.h"
#include "Core/Component/ActorComponent.h"

#include "Manager/TimeManager.h"

class Actor : public InterfaceReflector<Object, IUpdatable>
{
	GEN_REFLECTION(Actor)

public:
	Actor();
	virtual ~Actor() override;

protected:
	virtual void PostCreate() override;
	virtual void PostLoad() override;
	virtual void BeginPlay();
	virtual void Update(float deltaTime) override;
	virtual void EndPlay();
	virtual void BeginDestroy() override;

public:
	std::shared_ptr<ActorComponent> FindComponentByClass(const SubClass<ActorComponent> targetClass);

private:
	std::shared_ptr<ActorComponent> _mRootComp;
	std::unordered_set<std::shared_ptr<ActorComponent>> _mOwnedComps;

private:
	std::shared_ptr<UpdateTargetContext> _mUpdateContext;
};

