#include "pch.h"
#include "DefaultEditorInputMappingContext.h"

#include "Input/DefaultEditorAction.h"

DefaultEditorInputMappingContext::DefaultEditorInputMappingContext()
{
	// 피킹
	{
		InputMapping mapping;
		mapping.mAction = &PickingEditorAction::GetStaticTypeInfo();
		{
			InputBinding binding;
			binding.mCtrl = binding.mAlt = binding.mShift = false;
			binding.mKeyType = KeyType::Mouse::LButton;
			mapping.mBindings.push_back(binding);
		}
		_mMappings.push_back(mapping);
	}

	// 옵션
	{
		InputMapping mapping;
		mapping.mAction = &OptionEditorAction::GetStaticTypeInfo();
		{
			InputBinding binding;
			binding.mCtrl = binding.mAlt = binding.mShift = false;
			binding.mKeyType = KeyType::Mouse::RButton;
			mapping.mBindings.push_back(binding);
		}
		_mMappings.push_back(mapping);
	}
}
