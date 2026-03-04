#pragma once

#include "Graphics/Widget/VirtualWindow.h"

class Splitter;

/**
 * 가상 윈도우 창을 나누는 공간 분할 컴포짓 클래스
 */
class VirtualWindowSplitter : public VirtualWindow
{
	GEN_REFLECTION(VirtualWindowSplitter)

public:
	VirtualWindowSplitter();

	/* 생성 시, 초기화 인자 구조체 정의 */
public:
	DECALRE_WIDGET_ARGS_BEGIN(VirtualWindowSplitter)
	DECALRE_WIDGET_ARGS_END()

public:
	void OnConstruct(const Arguments& args);

public:
	virtual SplitSizeRule GetSplitSizeRule() const override;
	WidgetOrientation GetOrientation() const;
	void SetOrientation(WidgetOrientation orient);

	const std::vector<std::shared_ptr<VirtualWindow>>& GetChildVirtualWindows() const;
	std::vector<std::shared_ptr<VirtualWindow>> GetAllChildVirtualWindows() const;

public:
	void AddChildVirtualWindow(const std::shared_ptr<VirtualWindow>& child, int32 targetIndex = -1);
	void AddChildVirtualWindow(const std::shared_ptr<VirtualWindow>& child, const std::shared_ptr<VirtualWindow>& existedChild, VirtualWindowAddDirection dir);
	void RemoveChildVirtualWindow(const std::shared_ptr<VirtualWindow>& child);
	void RemoveChildVirtualWindow(int32 targetIndex);

	void ReplaceChild(const std::shared_ptr<VirtualWindow>& child, const std::shared_ptr<VirtualWindow>& existedChild);

protected:
	static bool IsMatchOrientToAddVirtualWindow(VirtualWindowAddDirection addDir, WidgetOrientation splitOrient);

protected:
	PROPERTY(_mSplitter)
	std::shared_ptr<Splitter> _mSplitter;
	PROPERTY(_mChildVirtualWindows)
	std::vector<std::shared_ptr<VirtualWindow>> _mChildVirtualWindows;
};

