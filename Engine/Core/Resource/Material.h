#pragma once

#include "Core/Resource/Resource.h"
#include "Graphics/Resource/DXMaterial.h"

#include "Core/Resource/ConstantData.h"
#include "Core/Resource/Texture.h"

#include "Utils/Thread/RefCounting.h"

class Material : public Resource
{
	GEN_REFLECTION(Material)

public:
	using ProxyType = ThreadSafeRefCounting<std::shared_ptr<DXMaterial>, MainThreadType::Render>;

public:
	Material();
	~Material();

protected:
	virtual void PostCreate() override;
	virtual void PostLoad() override;

public:
	const ProxyType& GetProxy() const;

#ifdef _EDITOR

public:
	void UpdateProxy(
		std::shared_ptr<MaterialBulkData> newBulkData,
		const std::vector<std::pair<std::string, std::shared_ptr<ConstantDataBase>>>& newMatConstantDatas,
		const std::vector<std::pair<std::string, std::shared_ptr<Texture2D>>>& newBoundTextures
	);

#endif

private:
	PROPERTY(_mMatConstantDatas)
	std::vector<std::pair<std::string, std::shared_ptr<ConstantDataBase>>> _mMatConstantDatas;
	PROPERTY(_mBoundTextures)
	std::vector<std::pair<std::string, std::shared_ptr<Texture2D>>> _mBoundTextures;

private:
	PROPERTY(_mBulkData)
	std::shared_ptr<MaterialBulkData> _mBulkData;

	// 렌더 스레드에서 안전 제거되는 메테리얼 데이터
	ProxyType _mProxy;
	mutable bool _mNeedInitProxy = false;
};

