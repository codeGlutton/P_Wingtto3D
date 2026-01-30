#pragma once

#include <string>
#include "Types.h"

namespace CollisionMobility
{
	enum Type
	{
		Static = 0,
		Dynamic,
		Count
	};
}

namespace CollisionEnabledFlag
{
	enum Type : uint8
	{
		NoCollision = 0,
		QueryOnly = 1 << 0,
		PhishysOnly = 1 << 1,
		All = QueryOnly | PhishysOnly
	};
}

namespace CollisionInteraction
{
	enum Type : uint8
	{
		Ignore = 0,
		Overlap,
		Block,
		Count
	};
}

namespace ECollisionChannel
{
	enum Type
	{
		Static,
		Player,
		Monster,
		Custom1,
		Custom2,
		Custom3,
		Custom4,
		Custom5,
		Custom6,
		Custom7,
		Custom8,
		Custom9,
		Custom10,
		End
	};
};

struct CollisionChannel
{
	std::string	Name;
	ECollisionChannel::Type	Channel = ECollisionChannel::Static;
};

struct CollisionPreset
{
	//CollisionChannel 
};

enum class SimpleColliderType
{
	Box2D,
	Sphere2D,
};