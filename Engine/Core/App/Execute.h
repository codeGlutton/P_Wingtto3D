#pragma once

#include "Core/App/AppWindow/AppWindow.h"

enum class AppBuildTargetFlag : uint8
{
	None = 0,

	Debug = 1 << 0,
	Release = 1 << 1,
	Game = 1 << 2,
	Editor = 1 << 3,

	DebugAndRelease = Debug | Release,
	GameAndEditor = Game | Editor,
	ALL = 0xFF,
};

constexpr AppBuildTargetFlag operator&(AppBuildTargetFlag lhs, AppBuildTargetFlag rhs)
{
	using Base = std::underlying_type_t<AppBuildTargetFlag>;
	return static_cast<AppBuildTargetFlag>(static_cast<Base>(lhs) & static_cast<Base>(rhs));
}

constexpr AppBuildTargetFlag operator|(AppBuildTargetFlag lhs, AppBuildTargetFlag rhs)
{
	using Base = std::underlying_type_t<AppBuildTargetFlag>;
	return static_cast<AppBuildTargetFlag>(static_cast<Base>(lhs) | static_cast<Base>(rhs));
}

template<AppBuildTargetFlag FLAG>
struct AppBuildConstraint
{
	static constexpr const AppBuildTargetFlag mFlag = FLAG;
};

class IExecute abstract
{
public:
	using BuildConstraint = AppBuildConstraint<AppBuildTargetFlag::ALL>;
	using DefaultWindow = AppWindow;

public:
	virtual ~IExecute() = default;

public:
	virtual void Init() = 0;
	virtual void Update() = 0;
	virtual void End() = 0;
};

