#pragma once

#include "Utils/TypeUtils.h"
#include "CommonTypeInfo.h"

template<typename T> requires IsChildOfObject<T>
const SubClassTypeInfo<T> SubClassTypeInfo<T>::mStatic;

template<typename T> requires IsChildOfObject<T>
inline bool SubClassTypeInfo<T>::IsInstanceValueEqual(const void* lhsInst, const void* rhsInst) const
{
	const SubClass<T>& lhsInstRef = *reinterpret_cast<const SubClass<T>*>(lhsInst);
	const SubClass<T>& rhsInstRef = *reinterpret_cast<const SubClass<T>*>(rhsInst);
	return lhsInstRef == rhsInstRef;
}

template<typename T> requires IsChildOfObject<T>
inline void SubClassTypeInfo<T>::Serialize(OUT Archive& archive, const void* inst) const
{
	// 클래스 이름 저장

	const SubClass<T>& instRef = *reinterpret_cast<const SubClass<T>*>(inst);
	std::string objectTypeName = "";
	if (instRef.Get() != nullptr)
	{
		objectTypeName = instRef.Get()->GetName();
	}
	TypeInfoResolver<std::string>::Get().Serialize(archive, &objectTypeName);
}

template<typename T> requires IsChildOfObject<T>
inline void SubClassTypeInfo<T>::Deserialize(Archive& archive, OUT void* inst) const
{
	// 클래스 이름 로드해 대입

	SubClass<T>& instRef = *reinterpret_cast<SubClass<T>*>(inst);
	std::string objectTypeName = "";
	TypeInfoResolver<std::string>::Get().Deserialize(archive, &objectTypeName);
	if (objectTypeName.empty() == false)
	{
		instRef = BOOT_SYSTEM->GetObjectTypeInfo(objectTypeName.c_str());
	}
}

template<typename T> requires IsChildOfObject<T>
const SoftRefTypeInfo<T> SoftRefTypeInfo<T>::mStatic;

template<typename T> requires IsChildOfObject<T>
inline bool SoftRefTypeInfo<T>::IsInstanceValueEqual(const void* lhsInst, const void* rhsInst) const
{
	const SoftObjectPtr<T>& lhsInstRef = *reinterpret_cast<const SoftObjectPtr<T>*>(lhsInst);
	const SoftObjectPtr<T>& rhsInstRef = *reinterpret_cast<const SoftObjectPtr<T>*>(rhsInst);
	return lhsInstRef == rhsInstRef;
}

template<typename T> requires IsChildOfObject<T>
inline void SoftRefTypeInfo<T>::Serialize(OUT Archive& archive, const void* inst) const
{
	const SoftObjectPtr<T>& instRef = *reinterpret_cast<const SoftObjectPtr<T>*>(inst);
	TypeInfoResolver<std::wstring>::Get().Serialize(archive, &instRef.GetPath());
}

template<typename T> requires IsChildOfObject<T>
inline void SoftRefTypeInfo<T>::Deserialize(Archive& archive, OUT void* inst) const
{
	std::wstring objectPath;
	TypeInfoResolver<std::wstring>::Get().Deserialize(archive, &objectPath);

	SoftObjectPtr<T>& instRef = *reinterpret_cast<SoftObjectPtr<T>*>(inst);
	instRef = objectPath;
}

template<typename T> requires IsBulk<T>
const BulkTypeInfo<T> BulkTypeInfo<T>::mStatic;

template<typename T> requires IsBulk<T>
inline bool BulkTypeInfo<T>::SerializeBulkData(OUT Archive& archive, const void* inst) const
{
	const std::shared_ptr<T>& instRef = *reinterpret_cast<const std::shared_ptr<T>*>(inst);
	return T::GetStaticTypeInfo().Serialize(archive, instRef.get());
}

template<typename T> requires IsBulk<T>
inline bool BulkTypeInfo<T>::IsInstanceValueEqual(const void* lhsInst, const void* rhsInst) const
{
	const std::shared_ptr<T>& lhsInstRef = *reinterpret_cast<const std::shared_ptr<T>*>(lhsInst);
	const std::shared_ptr<T>& rhsInstRef = *reinterpret_cast<const std::shared_ptr<T>*>(rhsInst);
	return T::GetStaticTypeInfo().IsInstanceValueEqual(lhsInstRef.get(), rhsInstRef.get());
}

template<typename T> requires IsBulk<T>
inline void BulkTypeInfo<T>::Serialize(OUT Archive& archive, const void* inst) const
{
	// 헤더에 쓰여질 대상이 자신임을 알리기

	// 패키징 명 쓰기
	const std::wstring& packagePath = archive.GetPackagePath();
	TypeInfoResolver<std::wstring>::Get().Serialize(archive, &packagePath);

	// 인덱스 쓰기
	const std::shared_ptr<BulkData>& instRef = *reinterpret_cast<const std::shared_ptr<BulkData>*>(inst);
	std::size_t index = archive.GetLinkData().mBulkDataIndexMap[instRef.get()];
	TypeInfoResolver<std::size_t>::Get().Serialize(archive, &index);
}

template<typename T> requires IsBulk<T>
inline void BulkTypeInfo<T>::Deserialize(Archive& archive, OUT void* inst) const
{
	// 헤더로 생성된 실제 데이터를 가져오기

	// 패키징 명 읽기
	std::wstring packagePath;
	TypeInfoResolver<std::wstring>::Get().Deserialize(archive, &packagePath);

	// 인덱스 읽기
	std::size_t index;
	TypeInfoResolver<std::size_t>::Get().Deserialize(archive, &index);

	// 리소스에 대입
	std::shared_ptr<BulkData> bulkData = archive.GetBulkDatas(packagePath)[index];
	std::shared_ptr<BulkData>& instRef = *reinterpret_cast<std::shared_ptr<BulkData>*>(inst);
	instRef = bulkData;
}