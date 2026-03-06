#include "pch.h"
#include "WidgetTextCache.h"
#include <cwctype>

void WidgetTextCache::UpdateOnPrepass(const std::wstring& text, TextTransformPolicy transformPolicy, const WidgetFontData& font, const Margin& margin, const Vec2& layoutSacle)
{
	if (_mState != State::Dirty)
	{
		return;
	}
	_mState = State::Updating;

	if (_mIsResized == true)
	{
		_mIsResized = false;
		return;
	}

	/* 캐싱 초기화 */

	_mCharLocalCaches.clear();
	_mCharLocalCaches.reserve(text.size());
	_mCharRenderCaches.clear();
	_mCharRenderCaches.reserve(text.size());
	_mScaleLineHeight = 0.f;
	_mScaleAscender = 0.f;
	_mScaleMargin = 0.f;
	_mTransformedText.clear();
	_mDesiredSize = Vec2(0.f);

	if (text.empty() == true || font.IsValid() == false)
	{
		_mDesiredSize = Vec2(0.f);
		return;
	}

	/* 문자열 변환 처리 */

	_mTransformedText = text;
	switch (transformPolicy)
	{
	case TextTransformPolicy::Lowwer:
		std::transform(_mTransformedText.begin(), _mTransformedText.end(), _mTransformedText.begin(), std::towlower);
		break;
	case TextTransformPolicy::Upper:
		std::transform(_mTransformedText.begin(), _mTransformedText.end(), _mTransformedText.begin(), std::towupper);
		break;
	}

	/* 문자 캐싱 저장 */

	const FontAtlasData& fontAtlas = font.GetFontAtlasCahce().GetAtlasData();
	const float minLayoutAxisScale = std::min<float>(layoutSacle.x, layoutSacle.y);
	const float fontScale = minLayoutAxisScale * static_cast<float>(font.GetPixelSize()) / static_cast<float>(fontAtlas.mPixelSize); // (원하는 픽셀 사이즈 / 아틀라스 픽셀 사이즈)
	const float fontSpacing = minLayoutAxisScale * font.GetCharSpacing();

	_mScaleLineHeight = fontAtlas.mLineHeight * fontScale;
	_mScaleAscender = fontAtlas.mAscender * fontScale;
	_mScaleMargin = minLayoutAxisScale * margin;

	// [여백] + [ H ].. 에서 [여백] + [H ]...로 변환 오프셋
	const float startScaleOffset = _mScaleMargin.mLeft - fontAtlas.mGlyphs.at(_mTransformedText.front()).mBearingX * fontScale;

	// 모두 양수로 저장
	Vec2 preRenderScaleOffset = { startScaleOffset, _mScaleMargin.mUp};
	for (const wchar_t& targetChar : _mTransformedText)
	{
		const GlyphData& glyph = fontAtlas.mGlyphs.at(targetChar);

		// 단순히 스케일 처리된 문자 정보
		WidgetCharLocalCache charLocalCache;
		// 랜더시 위치와 크기가 담긴 스케일 처리된 문자 정보
		WidgetCharRenderCache charRenderCache;
		charRenderCache.mChar = targetChar;

		// [ A ]B
		// <--->
		// 스케일 조정된 기본 간격 + 폰트 간격 옵션
		charLocalCache.mScaleAdvance = glyph.mAdvance * fontScale + fontSpacing;

		// 스케일 조정된 오프셋
		charLocalCache.mScaleOffset.x = glyph.mBearingX * fontScale;
		charLocalCache.mScaleOffset.y = _mScaleAscender - glyph.mBearingY * fontScale;
		// 랜더링 시 오프셋
		charRenderCache.mScaleOffset.x = charLocalCache.mScaleOffset.x + preRenderScaleOffset.x;
		charRenderCache.mScaleOffset.y = charLocalCache.mScaleOffset.y + preRenderScaleOffset.y;
		// 스케일 조정 & 랜더링 시 문자 사이즈
		charRenderCache.mScaleSize.x = charLocalCache.mScaleSize.x = glyph.mBaseSize.mX * fontScale;
		charRenderCache.mScaleSize.y = charLocalCache.mScaleSize.y = glyph.mBaseSize.mY * fontScale;

		preRenderScaleOffset.x += charLocalCache.mScaleAdvance;

		_mCharLocalCaches.emplace_back(std::move(charLocalCache));
		_mCharRenderCaches.emplace_back(std::move(charRenderCache));
	}

	// ...[ O ] + [여백] 에서 ...[ O] + [여백]로 변환 오프셋
	const float endScaleOffset = _mScaleMargin.mRight - _mCharLocalCaches.back().mScaleAdvance + _mCharLocalCaches.back().mScaleOffset.x + _mCharLocalCaches.back().mScaleSize.x;

	preRenderScaleOffset.x += -endScaleOffset;
	preRenderScaleOffset.y += _mScaleMargin.mDown + _mScaleLineHeight;

	// 한줄 DesiredSize 크기 계산 완료
	_mDesiredSize = preRenderScaleOffset;
}

void WidgetTextCache::UpdateOnPaint(const Vec2& allottedSize, TextJustifyPolicy justifyPolicy, bool autoWrap)
{
	if (_mState != State::Updating)
	{
		_mIsResized = false;
		return;
	}
	_mState = State::OnCache;

	_mBoxSize = Vec2::Zero;
	_mBoxOffset = Vec2::Zero;

	// 텍스트가 비어있거나, Wrap을 요구하지 않는 경우, 사이즈가 충분한 경우 한줄로 출력
	if (_mTransformedText.empty() == true || autoWrap == false || allottedSize.x >= _mDesiredSize.x)
	{
		_mBoxOffset.x = ComputeTextLineOffset(_mDesiredSize.x, allottedSize.x, justifyPolicy);
		_mBoxSize = _mDesiredSize;

		_mIsResized = false;
		_mCharLocalCaches.clear();
		return;
	}

	/* 줄바꿈으로 인한 조정 처리 */

	std::size_t lineIndex = 0;
	for (std::size_t firstCharIndex = 0; firstCharIndex < _mTransformedText.size(); ++lineIndex)
	{
		// [여백] + [ H ].. 에서 [여백] + [H ]...로 변환 오프셋
		const float startScaleOffset = _mScaleMargin.mLeft - _mCharLocalCaches[firstCharIndex].mScaleOffset.x;

		// 줄마다 누적 오프셋
		Vec2 preRenderScaleOffset = { startScaleOffset, _mScaleMargin.mUp + lineIndex * _mScaleLineHeight };
		float curLineMaxSize = preRenderScaleOffset.x + _mScaleMargin.mRight + _mCharLocalCaches[firstCharIndex].mScaleOffset.x + _mCharLocalCaches[firstCharIndex].mScaleSize.x;

		// 첫번째는 무조건 넣기
		_mCharRenderCaches[firstCharIndex].mScaleOffset.x = _mCharLocalCaches[firstCharIndex].mScaleOffset.x + preRenderScaleOffset.x;
		_mCharRenderCaches[firstCharIndex].mScaleOffset.y = _mCharLocalCaches[firstCharIndex].mScaleOffset.y + preRenderScaleOffset.y;
		preRenderScaleOffset.x += _mCharLocalCaches[firstCharIndex].mScaleAdvance;

		std::size_t charIndex = firstCharIndex + 1;
		for (; charIndex < _mTransformedText.size(); ++charIndex)
		{
			const WidgetCharLocalCache& localCache = _mCharLocalCaches[charIndex];
			WidgetCharRenderCache& renderCache = _mCharRenderCaches[charIndex];

			// ...[ O ] + [여백] 에서 ...[ O] + [여백]로 변환 오프셋
			const float endScaleOffset = _mScaleMargin.mRight + localCache.mScaleOffset.x + localCache.mScaleSize.x;
			// 마지막이라 가정한 경우 x 사이즈
			const float postLineMaxSize = preRenderScaleOffset.x + endScaleOffset;
			if (postLineMaxSize > allottedSize.x)
			{
				// 다음 줄 넘어감
				break;
			}
			curLineMaxSize = postLineMaxSize;

			renderCache.mScaleOffset.x = localCache.mScaleOffset.x + preRenderScaleOffset.x;
			renderCache.mScaleOffset.y = localCache.mScaleOffset.y + preRenderScaleOffset.y;
			
			preRenderScaleOffset.x += localCache.mScaleAdvance;
		}

		/* 줄 정렬 맞추기 */

		AlignTextLine(curLineMaxSize, allottedSize.x, justifyPolicy, firstCharIndex, charIndex);
		firstCharIndex = charIndex;
	}

	_mBoxSize.x = allottedSize.x;
	_mBoxSize.y = _mScaleMargin.mUp + lineIndex * _mScaleLineHeight + _mScaleMargin.mDown;
	_mDesiredSize = _mBoxSize;
	
	_mIsResized = true;
}

float WidgetTextCache::ComputeTextLineOffset(float desiredLineSizeX, float allottedSizeX, TextJustifyPolicy justifyPolicy)
{
	float lineOffset = 0.f;
	switch (justifyPolicy)
	{
	case TextJustifyPolicy::Center:
		lineOffset = (allottedSizeX - desiredLineSizeX) / 2.f;
		break;
	case TextJustifyPolicy::Right:
		lineOffset = allottedSizeX - desiredLineSizeX;
		break;
	}

	return lineOffset;
}

float WidgetTextCache::AlignTextLine(float desiredLineSizeX, float allottedSizeX, TextJustifyPolicy justifyPolicy, std::size_t startIndex, std::size_t endIndex)
{
	/* 줄 정렬 맞추기 */

	float lineOffset = ComputeTextLineOffset(desiredLineSizeX, allottedSizeX, justifyPolicy);
	for (; startIndex < endIndex; ++startIndex)
	{
		WidgetCharRenderCache& renderCache = _mCharRenderCaches[startIndex];
		renderCache.mScaleOffset.x += lineOffset;
	}

	return lineOffset;
}

