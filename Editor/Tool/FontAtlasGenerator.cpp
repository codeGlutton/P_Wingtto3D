#include "pch.h"
#include "FontAtlasGenerator.h"

#include "Manager/PathManager.h"
#include "Graphics/Resource/DXTexture.h"

#include "ft2build.h"

#include FT_FREETYPE_H

#define CHECK_FT(err)					assert((err == FT_Err_Ok))
#define CHECK_FT_MSG(err, msg)			assert((err == FT_Err_Ok) && (msg))

std::vector<FontAtlasGenerator::Result> FontAtlasGenerator::Create(const std::wstring& fileFullPath, const std::vector<uint32>& pixelSizes, int32 atlasXSize)
{
    std::string fileFullPathStr = ConvertWStringToUtf8(fileFullPath);

    FT_Library ftLib;
    CHECK_FT_MSG(FT_Init_FreeType(&ftLib), "FT_Library init error");

    // 글씨체 (bold, italic등은 별도로 취급)
    FT_Face ftFace;
    CHECK_FT_MSG(FT_New_Face(ftLib, fileFullPathStr.c_str(), -1 /* 갯수 파악 */, &ftFace), "FT_Face init error for checking count");
    FT_Long faceCount = ftFace->num_faces;

    // 글씨체별로 별도 Font 에셋화
    std::vector<FontAtlasGenerator::Result> genResults(faceCount);
    for (FT_Long faceId = 0; faceId < faceCount; ++faceId)
    {
        FT_Error err = FT_New_Face(ftLib, fileFullPathStr.c_str(), faceId, &ftFace);
        if (err != FT_Err_Ok)
        {
            if (err == FT_Err_Unknown_File_Format)
            {
                FAIL_MSG("Font file is valid. But it is unsupported font format");
            }
            else
            {
                FAIL_MSG("Font file is invalid");
            }
        }

        std::string styleName = ftFace->style_name ? ftFace->style_name : "Regular";
        genResults[faceId].mSuffixName = ConvertUtf8ToWString(styleName);

        std::shared_ptr<FontAtlasBulkData>& fontData = genResults[faceId].mBulkData;
        fontData = std::make_shared<FontAtlasBulkData>();
        fontData->mValue.reserve(pixelSizes.size());

        // 픽셀 지정 크기별로 캐싱
        for (const uint32 pixelSize : pixelSizes)
        {
            FT_Set_Pixel_Sizes(ftFace, 0, pixelSize);

            FontAtlasData atlasData;
            atlasData.mPixelSize = pixelSize;
            atlasData.mAscender = INT_MIN;
            atlasData.mDescender = INT_MIN;

            int32 accTexX = 0;
            int32 accTexY = 0;
            int32 curRowMaxY = 0;

            // 일단 영문만 글자별로 아틀라스에 그리기
            // TODO : 한글
            for (uint32 charCode = 32; charCode < 127; ++charCode)
            {
                if (FT_Load_Char(ftFace, charCode, FT_LOAD_RENDER) != FT_Err_Ok)
                {
                    continue;
                }

                FT_GlyphSlot ftSlot = ftFace->glyph;
                const int32 charWidth = static_cast<int32>(ftSlot->bitmap.width);
                const int32 charHeight = static_cast<int32>(ftSlot->bitmap.rows);

                // 텍스처 X 방향으로 한줄 채움
                if (accTexX + charWidth >= atlasXSize)
                {
                    accTexX = 0;
                    accTexY += curRowMaxY;
                    curRowMaxY = 0;
                }

                // 비트맵 복사
                // TODO : SRC 방식으로 변환 + skyline 방식 채우기
                for (int32 charY = 0; charY < charHeight; ++charY)
                {
                    for (int32 charX = 0; charX < charWidth; ++charX)
                    {
                        int32 texIndex = (accTexX + charX) + (accTexY + charY) * atlasXSize;
                        int32 charIndex = charX + charY * charWidth;
                        atlasData.mData[texIndex] = ftSlot->bitmap.buffer[charIndex];
                    }
                }

                std::pair<wchar_t, GlyphData> glyphPair;
                {
                    glyphPair.first = static_cast<wchar_t>(charCode);
                    GlyphData& glyph = glyphPair.second;

                    glyph.mBaseSize = IntPoint<int32>(charWidth, charHeight);
                    glyph.mUVStart = Vec2(
                        static_cast<float>(accTexX),
                        static_cast<float>(accTexY)
                    );
                    glyph.mUVSize = Vec2(
                        static_cast<float>(accTexX + charWidth),
                        static_cast<float>(accTexY + charHeight)
                    );
                    glyph.mBearingX = ftSlot->bitmap_left;
                    glyph.mBearingY = ftSlot->bitmap_top;
                    glyph.mAdvance = ftSlot->advance.x >> 6;

                    atlasData.mAscender = std::max<int32>(atlasData.mAscender, glyph.mBearingY);
                    atlasData.mDescender = std::max<int32>(atlasData.mDescender, charHeight - glyph.mBearingY);
                }
                atlasData.mGlyphs.insert(std::move(glyphPair));

                accTexX += ftSlot->bitmap.width + 1; // 겹침 방지
                if (curRowMaxY < charHeight)
                {
                    curRowMaxY = charHeight;
                }
            }
            accTexY += curRowMaxY;

            // 폰트 파싱 결과로 마지막 아틀라스 데이터 채우기
            atlasData.mLineHeight = (++atlasData.mAscender) + (++atlasData.mDescender); // 겹침 방지
            atlasData.mWidth = atlasXSize;
            atlasData.mHeight = accTexY;
            for (auto& glyphPair : atlasData.mGlyphs)
            {
                glyphPair.second.mUVStart.x /= static_cast<float>(atlasData.mWidth);
                glyphPair.second.mUVStart.y /= static_cast<float>(atlasData.mHeight);

                glyphPair.second.mUVSize.x /= static_cast<float>(atlasData.mWidth);
                glyphPair.second.mUVSize.y /= static_cast<float>(atlasData.mHeight);
            }

            fontData->mValue.emplace_back(std::move(atlasData));
        }
    }

    FT_Done_Face(ftFace);
    FT_Done_FreeType(ftLib);

    return genResults;
}
