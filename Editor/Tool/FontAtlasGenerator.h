#pragma once

struct FontAtlasBulkData;

class FontAtlasGenerator
{
public:
	struct Result
	{
		// 에셋 이름 뒤에 붙을 스타일 이름
		std::wstring mSuffixName;
		std::shared_ptr<FontAtlasBulkData> mBulkData;
	};

	/**
	 * 폰트 파일을 파싱하여, 폰트 에셋 핵심 데이터로 변환하는 함수
	 * \param fileFullPath 폰트 파일 절대 경로
	 * \param pixelSizes 캐싱해놓을 픽셀 크기들
	 * \param atlasXSize 아틀라스 텍스처 픽셀 x 크기
	 * \return 결과 데이터
	 */
	static std::vector<Result> Create(const std::wstring& fileFullPath, const std::vector<uint32>& pixelSizes, int32 atlasXSize);
};

