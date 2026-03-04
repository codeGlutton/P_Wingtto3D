#include "pch.h"
#include "WidgetFontData.h"

#include "Core/Resource/Font.h"

const FontAtlasDataRef& WidgetFontData::GetFontAtlasCahce() const
{
	if (_mIsDirty == true)
	{
		_mIsDirty = false;
		if (_mFont != nullptr)
		{
			_mCache = _mFont->GetFontDataRef(_mPixelSize);
		}
	}
	return _mCache;
}
