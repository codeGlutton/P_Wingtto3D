#include "pch.h"
#include "Attribute.h"

#include "Graphics/Widget/Widget.h"
#include "Graphics/Widget/Slot/WidgetSlot.h"

void LayoutAttributeBase::OnChangeLayout(const Widget* widget) const
{
	widget->MarkLayoutDirty();
}

void LayoutAttributeBase::OnChangeLayout(const WidgetSlotBase* slot) const
{
	slot->GetOwnerContainer()->GetOwner()->MarkLayoutDirty();
}
