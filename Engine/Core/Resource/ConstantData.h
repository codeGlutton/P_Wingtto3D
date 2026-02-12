#pragma once

#include "Core/Object.h"
#include "Graphics/Buffer/DXConstantBuffer.h"

#include "Utils/Thread/RefCounting.h"

class ConstantDataBase abstract : public Object
{
	GEN_ABSTRACT_REFLECTION(ConstantDataBase)

	friend class Material;

public:
	ConstantDataBase();
	~ConstantDataBase();

protected:
	PROPERTY(_mBulkData)
	std::shared_ptr<ConstantBufferBulkData> _mBulkData;
};

template<typename T>
class ConstantData : public ConstantDataBase
{
	GEN_REFLECTION(ConstantData<T>)

protected:
	virtual void Serialize(Archive& archive) const override;
	virtual void Deserialize(Archive& archive) override;

private:
	T _mData;
};

template<typename T>
inline void ConstantData<T>::Serialize(Archive& archive) const
{
	std::byte* rawPtr = reinterpret_cast<std::byte*>(&_mData);
	_mBulkData->mRawBytes.insert(_mBulkData->mRawBytes.end(), rawPtr, rawPtr + sizeof(_mData));
	_mBulkData->mSize = _mBulkData->mRawBytes.size();

	Super::Serialize(archive);
}

template<typename T>
inline void ConstantData<T>::Deserialize(Archive& archive)
{
	Super::Deserialize(archive);

	std::memcpy(&_mData, _mBulkData->mRawBytes.data(), _mBulkData->mSize);
}
