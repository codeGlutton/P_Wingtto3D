#include "pch.h"
#include "WidgetStyleManager.h"

#include "Graphics/Widget/Type/WidgetStyle.h"
#include "Core/Resource/Package/Package.h"

#include "Manager/PathManager.h"
#include "Manager/PackageManager.h"
#include "Manager/ResourceManager.h"

#include "Graphics/Widget/VirtualWindow.h"
#include "Graphics/Widget/Splitter.h"
#include "Graphics/Widget/TextBlock.h"

WidgetStyleManager::WidgetStyleManager()
{
}

WidgetStyleManager::~WidgetStyleManager()
{
}

void WidgetStyleManager::Init()
{
	Load();
	if (_mStyles.empty() == true)
	{
		CreateDefaultStyles();
	}
}

void WidgetStyleManager::Destroy()
{
	Save();
	_mStyles.clear();
	_mPackage = nullptr;
}

void WidgetStyleManager::CreateDefaultStyles()
{
	WidgetColor windowMainColor;
	WidgetColor windowSubColor;
	WidgetColor windowBackgroundColor;

	WidgetColor windowTextColor;

	WidgetColor windowSplitterMainColor;
	WidgetColor windowSplitterHighlightColor;

	if (_mUseDarkMode == true)
	{
		windowMainColor = WidgetColor::SpecificInst(Color(0.125f, 0.125f, 0.125f));
		windowSubColor = WidgetColor::SpecificInst(Color(0.23f, 0.23f, 0.23f));
		windowBackgroundColor = WidgetColor::SpecificInst(Color(0.08f, 0.08f, 0.08f));

		windowTextColor = WidgetColor::SpecificInst(Color::White);

		windowSplitterMainColor = WidgetColor::SpecificInst(Color::White);
		windowSplitterHighlightColor = WidgetColor::SpecificInst(Color(0.f, 0.f, 0.6f));
	}
	else
	{
		windowMainColor = WidgetColor::SpecificInst(Color(0.7f, 0.9f, 0.9f));
		windowSubColor = WidgetColor::SpecificInst(Color(0.87f, 0.95f, 0.98f));
		windowBackgroundColor = WidgetColor::SpecificInst(Color(0.95f, 0.95f, 0.95f));

		windowTextColor = WidgetColor::SpecificInst(Color::Black);

		windowSplitterMainColor = WidgetColor::SpecificInst(Color::Black);
		windowSplitterHighlightColor = WidgetColor::SpecificInst(Color::Blue);
	}

	WidgetBrush windowMainBrush;
	{
		windowMainBrush.mType = WidgetBrushType::Box;
		windowMainBrush.mTint = windowMainColor;
		windowMainBrush.mImageSize = Vec2(1.f);
		windowMainBrush.mTexture = nullptr;
		windowMainBrush.mMargin = Margin(0.f);
		windowMainBrush.mTiling = TilingType::None;
	}
	WidgetBrush windowSubBrush;
	{
		windowSubBrush.mType = WidgetBrushType::Box;
		windowSubBrush.mTint = windowSubColor;
		windowSubBrush.mImageSize = Vec2(1.f);
		windowSubBrush.mTexture = nullptr;
		windowSubBrush.mMargin = Margin(0.f);
		windowSubBrush.mTiling = TilingType::None;
	}
	WidgetBrush windowBackgroundBrush;
	{
		windowBackgroundBrush.mType = WidgetBrushType::Box;
		windowBackgroundBrush.mTint = windowBackgroundColor;
		windowBackgroundBrush.mImageSize = Vec2(1.f);
		windowBackgroundBrush.mTexture = nullptr;
		windowBackgroundBrush.mMargin = Margin(0.f);
		windowBackgroundBrush.mTiling = TilingType::None;
	}
	std::shared_ptr<TextBlockStyle> windowTextStyle = CreateStyle<TextBlockStyle>(L"WindowTextStyle");
	{
		const std::wstring resourcePath = PATH_MANAGER->GetEngineResourceFolderName() + L"\\Font\\F_Cafe24Decobox_Regular";
		std::shared_ptr<Font> defaultFont = RESOURCE_MANAGER->LoadOrGetResource<Font>(resourcePath);

		windowTextStyle->mFont = defaultFont;
		windowTextStyle->mCharSpacing = 0;
		windowTextStyle->mBackgroundColor = windowTextColor;
	}
	std::shared_ptr<VirtualWindowStyle> windowStyle = CreateStyle<VirtualWindowStyle>(L"WindowStyle");
	{
		windowStyle->mMainBrush = windowMainBrush;
		windowStyle->mSubBrush = windowSubBrush;
		windowStyle->mBackgroundBrush = windowBackgroundBrush;
		windowStyle->mTextBlockStyle = windowTextStyle;
	}
	std::shared_ptr<SplitterStyle> windowSplitterStyle = CreateStyle<SplitterStyle>(L"WindowSplitterStyle");
	{
		windowSplitterStyle->mNormalBrush = windowSubBrush;
		windowSplitterStyle->mNormalBrush.mTint = windowSplitterMainColor;
		windowSplitterStyle->mHighlightBrush = windowSubBrush;
		windowSplitterStyle->mHighlightBrush.mTint = windowSplitterHighlightColor;
	}
};

std::shared_ptr<WidgetStyle> WidgetStyleManager::CreateStyle(const std::wstring& name, const ObjectTypeInfo* typeInfo, ObjectCreateFlag::Type flags)
{
	ASSERT_MSG(typeInfo->IsChildOf<WidgetStyle>() == true, "CreateStyle func is not allowed to create non WidgetStyle class");

	if (_mStyles.find(name) != _mStyles.end())
	{
		DEBUG_LOG("CreateStyle func call is failed because of already exiested style");
		return nullptr;
	}

	std::shared_ptr<WidgetStyle> style = std::static_pointer_cast<WidgetStyle>(NewObject(_mPackage, typeInfo, name, flags));
	if (style == nullptr)
	{
		return nullptr;
	}
	return style;
}

void WidgetStyleManager::RegisterPackage(std::shared_ptr<Package> package)
{
	_mPackage = CastSharedPointer<WidgetStylePackage>(package);
}

void WidgetStyleManager::Save() const
{
	std::wstring packagePath = PATH_MANAGER->GetEngineResourceFolderName();
	packagePath.append(L"\\WidgetStyles");
	PACKAGE_MANAGER->SavePackage(packagePath);
}

void WidgetStyleManager::Load()
{
	std::wstring packagePath = PATH_MANAGER->GetEngineResourceFolderName();
	packagePath.append(L"\\WidgetStyles");
	PACKAGE_MANAGER->LoadPackage<WidgetStylePackage>(packagePath);
}

void WidgetStyleManager::NotifyToAddStyle(std::shared_ptr<WidgetStyle> style)
{
	ASSERT_MSG(_mStyles.find(style->GetName()) == _mStyles.end(), "Trying to register same widget style");
	_mStyles.insert({ style->GetName(), style });
}

void WidgetStyleManager::NotifyToRemoveStyle(const std::wstring& name)
{
	ASSERT_MSG(_mStyles.find(name) != _mStyles.end(), "Trying to remove empty widget style");
	_mStyles.erase(name);
}
