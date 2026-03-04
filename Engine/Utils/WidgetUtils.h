#pragma once

#include "Manager/AppWindowManager.h"
#include "Graphics/Widget/Widget.h"

#define NEW_WIDGET(type) WidgetBuilder<type>(true, nullptr) <<= type::Arguments()
#define NEW_EDIT_WIDGET(type) WidgetBuilder<type>(false, nullptr) <<= type::Arguments()

#define ASSIGN_WIDGET(type, target) WidgetBuilder<type>(true, nullptr).Assign(target) <<= type::Arguments()
#define ASSIGN_EDIT_WIDGET(type, target) WidgetBuilder<type>(false, nullptr).Assign(target) <<= type::Arguments()

template<typename T> requires std::is_base_of_v<Widget, T>
struct WidgetBuilder
{
public:
	WidgetBuilder(bool inGame, std::shared_ptr<Widget> parent)
	{
		if (inGame == true)
		{
			// TODO
			_mWidget = APP_WIN_MANAGER->CreateAppWindowWidget<T>(parent);
		}
		else
		{
			_mWidget = APP_WIN_MANAGER->CreateAppWindowWidget<T>(parent);
		}
	}

public:
	WidgetBuilder&& Assign(std::weak_ptr<T>& output)&&
	{
		output = _mWidget;
		return std::move(*this);
	}

	WidgetBuilder&& Assign(std::shared_ptr<T>& output) &&
	{
		output = _mWidget;
		return std::move(*this);
	}

	std::shared_ptr<T> operator<<=(const typename T::Arguments& args) &&
	{
		_mWidget->OnConstruct(args);

		return std::move(_mWidget);
	}

private:
	std::shared_ptr<T> _mWidget;
};

