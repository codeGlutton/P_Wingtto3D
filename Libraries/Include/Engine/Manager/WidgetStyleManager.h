#pragma once

#include "Core/Resource/Package/PackageRuntimeOwner.h"

#define WIDGET_STYLE_MANAGER WidgetStyleManager::GetInst()

class WidgetStyle;
class WidgetStylePackage;

class WidgetStyleManager : public IPackageRuntimeOwner
{
private:
	WidgetStyleManager();
	~WidgetStyleManager();

public:
	static WidgetStyleManager* GetInst()
	{
		static WidgetStyleManager inst;
		return &inst;
	}

public:
	void Init();
	void Destroy();

public:
	void CreateDefaultStyles();

public:
	template<typename T> requires std::is_base_of_v<WidgetStyle, T>
	std::shared_ptr<T> CreateStyle(const std::wstring& name, ObjectCreateFlag::Type flags = ObjectCreateFlag::None);
	std::shared_ptr<WidgetStyle> CreateStyle(const std::wstring& name, const ObjectTypeInfo* typeInfo, ObjectCreateFlag::Type flags = ObjectCreateFlag::None);

	template<typename T = WidgetStyle> requires std::is_base_of_v<WidgetStyle, T>
	std::shared_ptr<const T> GetStyle(const std::wstring& name);

public:
	void RegisterPackage(std::shared_ptr<Package> package) override;
	void Save() const override;
	void Load() override;

public:
	void NotifyToAddStyle(std::shared_ptr<WidgetStyle> style);
	void NotifyToRemoveStyle(const std::wstring& name);

private:
	std::shared_ptr<WidgetStylePackage> _mPackage;
	std::unordered_map<std::wstring, std::shared_ptr<WidgetStyle>> _mStyles;

private:
	const bool _mUseDarkMode = true;
};

template<typename T> requires std::is_base_of_v<WidgetStyle, T>
inline std::shared_ptr<T> WidgetStyleManager::CreateStyle(const std::wstring& name, ObjectCreateFlag::Type flags)
{
	return std::static_pointer_cast<T>(CreateStyle(name, &T::GetStaticTypeInfo(), flags));
}

template<typename T> requires std::is_base_of_v<WidgetStyle, T>
inline std::shared_ptr<const T> WidgetStyleManager::GetStyle(const std::wstring& name)
{
	auto iter = _mStyles.find(name);
	if (iter == _mStyles.end())
	{
		return nullptr;
	}
	return std::static_pointer_cast<T>(iter->second);
}
