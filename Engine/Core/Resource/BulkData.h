#pragma once

/**
 * 변경점이 없는 대용량 데이터 (버텍스, 애니메이션 등)
 */
struct BulkData
{
	GEN_BULK_STRUCT_REFLECTION(BulkData)

public:
	using Bulk = std::true_type;

public:
	PROPERTY(mIsLoad)
	bool mIsLoad = false;
};

template<typename T>
struct BulkWrapper : public BulkData
{
	GEN_BULK_STRUCT_REFLECTION(BulkWrapper<T>)

public:
	PROPERTY(mValue)
	T mValue;
};



