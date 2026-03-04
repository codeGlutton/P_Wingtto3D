#include "pch.h"
#include "WidgetSlot.h"
#include "Graphics/Widget/Widget.h"

void WidgetSlotContainer::OnResize()
{
	std::shared_ptr<Widget> owner = _mOwner.lock();
	if (owner != nullptr)
	{
		owner->MarkLayoutDirty();
	}
}

EmptySlotContainer& EmptySlotContainer::GetInst()
{
	static EmptySlotContainer inst(nullptr);
	return inst;
}

std::shared_ptr<WidgetSlotBase> EmptySlotContainer::_mNullSlot = nullptr;
