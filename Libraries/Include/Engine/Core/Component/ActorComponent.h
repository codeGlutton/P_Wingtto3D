#pragma once

#include "Core/Object.h"

#include "Manager/TimeManager.h"

class Actor;

class ActorComponent : public InterfaceReflector<Object, IUpdatable>
{
	GEN_REFLECTION(ActorComponent)

public:
	ActorComponent();
	virtual ~ActorComponent() override;

protected:
	virtual void PostCreate() override;
	virtual void PostLoad() override;
	virtual void InitializeComponent();
	virtual void BeginPlay();
	virtual void Update(float deltaTime) override;
	virtual void FixedUpdate() override;
	virtual void EndPlay();
	virtual void BeginDestroy() override;

private:
	// 부모 객체
	PROPERTY(_mOwner)
	std::weak_ptr<Actor> _mOwner;

private:
	std::shared_ptr<UpdateTargetContext> _mUpdateContext;
};

