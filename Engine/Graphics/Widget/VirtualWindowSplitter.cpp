#include "pch.h"
#include "VirtualWindowSplitter.h"

#include "Utils/WidgetUtils.h"
#include "Graphics/Widget/Splitter.h"

#include "Manager/WidgetStyleManager.h"

VirtualWindowSplitter::VirtualWindowSplitter()
{
	_mType = VirtualWindowType::Splitter;
	_mVisibility.Init(VisibilityType::SelfHitTestInvisible);
}

void VirtualWindowSplitter::OnConstruct(const Arguments& args)
{
	Widget::OnConstruct(args);
	
	AddSlot()
		[
			ASSIGN_EDIT_WIDGET(Splitter, _mSplitter)
				.Style(WIDGET_STYLE_MANAGER->GetStyle<SplitterStyle>(L"WindowSplitterStyle"))
				.Visibility(VisibilityType::Visible)
		];
}

SplitSizeRule VirtualWindowSplitter::GetSplitSizeRule() const
{
	if (_mChildVirtualWindows.size() > 0)
	{
		for (const std::shared_ptr<VirtualWindow>& child : _mChildVirtualWindows)
		{
			if (child->GetSplitSizeRule() == SplitSizeRule::AllottedRate)
			{
				return SplitSizeRule::AllottedRate;
			}
		}
		return SplitSizeRule::ContentSize;
	}
	return SplitSizeRule::AllottedRate;
}

WidgetOrientation VirtualWindowSplitter::GetOrientation() const
{
	return _mSplitter->GetSplitOrient();
}

void VirtualWindowSplitter::SetOrientation(WidgetOrientation orient)
{
	_mSplitter->SetSplitOrient(orient);
}

const std::vector<std::shared_ptr<VirtualWindow>>& VirtualWindowSplitter::GetChildVirtualWindows() const
{
	return _mChildVirtualWindows;
}

std::vector<std::shared_ptr<VirtualWindow>> VirtualWindowSplitter::GetAllChildVirtualWindows() const
{
	std::vector<std::shared_ptr<VirtualWindow>> collection;
	for (const std::shared_ptr<VirtualWindow>& virtualWindow : _mChildVirtualWindows)
	{
		collection.push_back(virtualWindow);
		if (virtualWindow->GetVirtualWindowType() == VirtualWindowType::Splitter)
		{
			std::shared_ptr<VirtualWindowSplitter> splitter = std::static_pointer_cast<VirtualWindowSplitter>(virtualWindow);
			std::vector<std::shared_ptr<VirtualWindow>> childCollection = splitter->GetAllChildVirtualWindows();
			collection.insert(collection.end(), childCollection.begin(), childCollection.end());
		}
	}
	return collection;
}

void VirtualWindowSplitter::AddChildVirtualWindow(const std::shared_ptr<VirtualWindow>& child, int32 targetIndex)
{
	_mSplitter->AddSlot(targetIndex)
		.Size(Attribute<float>::OnGet::Binder::BIND_DYNAMIC(child, GetSizeRate))
		.SizeRule(Attribute<SplitSizeRule>::OnGet::Binder::BIND_DYNAMIC(child, GetSplitSizeRule))
		.OnChangeSlotSize(OnChangeSlotSizeEvent::Binder::BIND_DYNAMIC(child, SetSizeRate))
		[
			child
		];

	if (targetIndex == -1)
	{
		_mChildVirtualWindows.push_back(child);
	}
	else
	{
		_mChildVirtualWindows.insert(_mChildVirtualWindows.begin() + targetIndex, child);
	}
	child->SetParentSplitter(std::static_pointer_cast<VirtualWindowSplitter>(shared_from_this()));
}

void VirtualWindowSplitter::AddChildVirtualWindow(const std::shared_ptr<VirtualWindow>& child, const std::shared_ptr<VirtualWindow>& existedChild, VirtualWindowAddDirection dir)
{
	// 같은 방향으로 배치
	const bool isMatchDir = IsMatchOrientToAddVirtualWindow(dir, GetOrientation());
	// 단일 자식 스플리터 여부
	const bool hasOneChild = (_mSlotContainer.Size() == 1);

	// 다른 방향으로 추가하려할때
	if (isMatchDir == false)
	{
		const WidgetOrientation childDesiredOrient = (GetOrientation() == WidgetOrientation::Horizon) ? WidgetOrientation::Vertical : WidgetOrientation::Horizon;
		if (hasOneChild == true)
		{
			// 단일 자식이라면, 단순히 현 스플리터의 방향을 변경
			SetOrientation(childDesiredOrient);
		}
		else
		{
			// 다중 자식이라면, 새로운 스플리터로 Wrapping
			// [Splitter [][!]] -> [Splitter [][NewSplitter[!]]]

			std::shared_ptr<VirtualWindowSplitter> newSplitter = NEW_EDIT_WIDGET(VirtualWindowSplitter)
				.Visibility(GetVisibility());

			ReplaceChild(newSplitter, existedChild);
			newSplitter->SetOrientation(childDesiredOrient);
			newSplitter->AddChildVirtualWindow(existedChild);
			return newSplitter->AddChildVirtualWindow(child, existedChild, dir);
		}
	}
}

void VirtualWindowSplitter::RemoveChildVirtualWindow(const std::shared_ptr<VirtualWindow>& child)
{
	const std::size_t size = _mChildVirtualWindows.size();
	std::size_t index = 0;
	for (; index < size; ++index)
	{
		if (_mChildVirtualWindows[index] == child)
		{
			break;
		}
	}
	ASSERT_MSG(index < size, "Virtual window can't be found in this splitter");
	RemoveChildVirtualWindow(static_cast<int32>(index));
}

void VirtualWindowSplitter::RemoveChildVirtualWindow(int32 targetIndex)
{
	_mChildVirtualWindows.erase(_mChildVirtualWindows.begin() + targetIndex);
	_mSplitter->RemoveSlot(targetIndex);
}

void VirtualWindowSplitter::ReplaceChild(const std::shared_ptr<VirtualWindow>& child, const std::shared_ptr<VirtualWindow>& existedChild)
{
	std::size_t existedChildIndex = 0;
	for (std::size_t existedChildIndex = 0; existedChildIndex < _mChildVirtualWindows.size(); ++existedChildIndex)
	{
		if (_mChildVirtualWindows[existedChildIndex] == existedChild)
		{
			break;
		}
	}

	/* 자식 캐싱 교체 */

	ASSERT_MSG(existedChildIndex != _mChildVirtualWindows.size(), "Replacing target child is not contained");
	_mChildVirtualWindows[existedChildIndex] = child;
	child->SetSizeRate(existedChild->GetSizeRate());

	/* 실제 슬롯 설정 교체 */

	Splitter::Slot& existedSlot = static_cast<Splitter::Slot&>(_mSplitter->GetChildren().GetSlotRef(existedChildIndex));
	existedSlot.SetSize(Attribute<float>::OnGet::Binder::BIND_DYNAMIC(child, GetSizeRate));
	existedSlot.SetSizeRule(Attribute<SplitSizeRule>::OnGet::Binder::BIND_DYNAMIC(child, GetSplitSizeRule));
	existedSlot.OnChangeSlotSize().BIND_DYNAMIC(child, SetSizeRate);
	existedSlot[child];

	child->SetParentSplitter(std::static_pointer_cast<VirtualWindowSplitter>(shared_from_this()));
}

bool VirtualWindowSplitter::IsMatchOrientToAddVirtualWindow(VirtualWindowAddDirection addDir, WidgetOrientation splitOrient)
{
	return
		((addDir == VirtualWindowAddDirection::Left || addDir == VirtualWindowAddDirection::Right) && splitOrient == WidgetOrientation::Horizon) ||
		((addDir == VirtualWindowAddDirection::Up || addDir == VirtualWindowAddDirection::Down) && splitOrient == WidgetOrientation::Vertical);
}
