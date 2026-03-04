#pragma once

#include "Graphics/Widget/Type/WidgetBrush.h"
#include "Core/Object.h"

class WidgetStyle abstract : public Object
{
	GEN_ABSTRACT_REFLECTION(WidgetStyle)
	friend class WidgetStyleManager;

protected:
	WidgetStyle() = default;
	virtual ~WidgetStyle() = default;

public:
	bool operator==(const WidgetStyle& other) const = default;

protected:
	virtual void PostLoad() override;
};

