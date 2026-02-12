#pragma once

#include "Graphics/Resource/DXResource.h"
#include "Graphics/Resource/DXInputLayout.h"

#include "Graphics/DXReflectionMetaData.h"

constexpr auto DX_VS_VER = "vs_5_0";
constexpr auto DX_PS_VER = "ps_5_0";
constexpr auto DX_DS_VER = "ds_5_0";
constexpr auto DX_HS_VER = "hs_5_0";
constexpr auto DX_GS_VER = "gs_5_0";
constexpr auto DX_CS_VER = "cs_5_0";

class DXConstantBuffer;
class DXSamplerState;
class DXTextureBase;
class DXStructureBuffer;
class DXComputeTextureBuffer;

class DXShaderBase abstract : public DXSharedResource
{
	friend class ResourceManager;

public:
	using ResourceType = EnumValueTag<DXSharedResourceType::Shader>;

protected:
	DXShaderBase();
	~DXShaderBase();

public:
	virtual void PushData() const = 0;
};

class DXShader abstract : public DXShaderBase
{
	friend class ResourceManager;

protected:
	DXShader();
	~DXShader();

public:
	DXResourceUsageBit::Type GetUsageBit() const
	{
		return _mUsageBit;
	}
	DXResourceUsageFlag::Type GetUsageFlag() const
	{
		return static_cast<DXResourceUsageFlag::Type>(1 << _mUsageBit);
	}

public:
	static std::shared_ptr<DXShader> CreateTransient(DXResourceUsageBit::Type type);
	ComPtr<ID3D11ShaderReflection> Reflect() const;

public:
	virtual void Init(const wchar_t* path, const char* entryName) = 0;

protected:
	void Load(const wchar_t* path, const char* entryName, const char* version);

protected:
	DXResourceUsageBit::Type _mUsageBit;
	ComPtr<ID3DBlob> _mBlob;
};

class DXVertexShader : public DXShader
{
	friend class ResourceManager;
	friend class DXInputLayout;

protected:
	DXVertexShader();
	~DXVertexShader();

public:
	virtual void Init(const wchar_t* path, const char* entryName) final override;

public:
	virtual void PushData() const final override;

private:
	ComPtr<ID3D11VertexShader> _mShader;
};

class DXPixelShader : public DXShader
{
	friend class ResourceManager;

protected:
	DXPixelShader();
	~DXPixelShader();

public:
	virtual void Init(const wchar_t* path, const char* entryName) final override;

public:
	virtual void PushData() const final override;

private:
	ComPtr<ID3D11PixelShader> _mShader;
};

class DXDomainShader : public DXShader
{
	friend class ResourceManager;

protected:
	DXDomainShader();
	~DXDomainShader();

public:
	virtual void Init(const wchar_t* path, const char* entryName) final override;

public:
	virtual void PushData() const final override;

private:
	ComPtr<ID3D11DomainShader> _mShader;
};

class DXHullShader : public DXShader
{
	friend class ResourceManager;

protected:
	DXHullShader();
	~DXHullShader();

public:
	virtual void Init(const wchar_t* path, const char* entryName) final override;

public:
	virtual void PushData() const final override;

private:
	ComPtr<ID3D11HullShader> _mShader;
};

class DXGeometryShader : public DXShader
{
	friend class ResourceManager;

protected:
	DXGeometryShader();
	~DXGeometryShader();

public:
	virtual void Init(const wchar_t* path, const char* entryName) final override;

public:
	virtual void PushData() const final override;

private:
	ComPtr<ID3D11GeometryShader> _mShader;
};

class DXComputeShader : public DXShader
{
	friend class ResourceManager;

public:
	struct MetaData
	{
		friend class DXComputeShader;
		
	protected:
		MetaData() = default;

	protected:
		void Clear()
		{
			for (auto& slot : mBindingSlots)
			{
				slot.clear();
			}
		}

	public:
		std::array<std::unordered_map<std::string, uint32>, DXShaderBindingType::Count> mBindingSlots;
	};

protected:
	DXComputeShader();
	~DXComputeShader();

public:
	void Bind(const std::string& name, std::shared_ptr<const DXConstantBuffer> buffer) const;
	void Bind(const std::string& name, std::shared_ptr<const DXTextureBase> texture) const;
	void Bind(const std::string& name, std::shared_ptr<const DXSamplerState> sampler) const;
	void Bind(const std::string& inputName, const std::string& outputName, std::shared_ptr<const DXStructureBuffer> buffer) const;
	void Bind(const std::string& inputName, const std::string& outputName, std::shared_ptr<const DXComputeTextureBuffer> buffer) const;

public:
	virtual void Init(const wchar_t* path, const char* entryName) final override;

public:
	virtual void PushData() const final override;

private:
	void MakeMetaData();

protected:
	// 리플렉션 데이터
	MetaData _mMetaData;

private:
	ComPtr<ID3D11ComputeShader> _mShader;
};

struct ShaderCompileDesc
{
	std::string mEntryName;
	DXResourceUsageBit::Type mUsageBit;
};

/**
 * 그래픽 파이프라인의 최소 단위 쉐이더 묶음
 */
class DXMinimumGraphicShader : public DXShaderBase
{
	friend class ResourceManager;

public:
	struct MetaData
	{
		friend class DXMinimumGraphicShader;

	protected:
		MetaData() = default;

	protected:
		void Clear()
		{
			mInputLayout.mParams.clear();
			for (auto& slot : mBindingSlots)
			{
				slot.clear();
			}
		}

	public:
		DXShaderLayoutMetaData mInputLayout;
		std::array<std::unordered_map<std::string, DXBindingMetaData>, DXShaderBindingType::Count> mBindingSlots;
	};

protected:
	DXMinimumGraphicShader();
	~DXMinimumGraphicShader();

public:
	void Bind(const std::string& name, std::shared_ptr<const DXConstantBuffer> buffer) const;
	void Bind(const std::string& name, std::shared_ptr<const DXTextureBase> texture) const;
	void Bind(const std::string& name, std::shared_ptr<const DXSamplerState> sampler) const;

protected:
	void Init(const DXInputElementList& elementList, const std::vector<std::shared_ptr<DXShader>>& shaders);
	void Init(const DXInputElementList& elementList, const std::vector<ShaderCompileDesc>& shaderDescs, const wchar_t* path);

public:
	void Init(const DXInputElementList& elementList, std::shared_ptr<DXVertexShader> vs, std::shared_ptr<DXPixelShader> ps = nullptr);
	void Init(const DXInputElementList& elementList, const wchar_t* path, const char* vsEntryName, const char* psEntryName = nullptr);

public:
	virtual void PushData() const override;

private:
	void MakeMetaData();

protected:
	// 리플렉션 데이터
	MetaData _mMetaData;

protected:
	// 입력 관련
	DXInputLayout _mLayout;

	// 등록된 각기 종류의 쉐이더
	std::array<std::shared_ptr<DXShader>, DXResourceUsageBit::Count> _mShaders;
	DXResourceUsageFlag::Type _mUsageFlags = DXResourceUsageFlag::None;
};

/**
 * 그래픽 파이프라인의 사용할 쉐이더 묶음
 */
class DXGraphicShader : public DXMinimumGraphicShader
{
	friend class ResourceManager;

protected:
	DXGraphicShader();
	~DXGraphicShader();

public:
	DXResourceUsageFlag::Type GetUsageFlags() const
	{
		return _mUsageFlags;
	}

public:
	void Init(const DXInputElementList& elementList, const std::vector<std::shared_ptr<DXShader>>& shaders);
	void Init(const DXInputElementList& elementList, const std::vector<ShaderCompileDesc>& shaderDescs, const wchar_t* path);

public:
	virtual void PushData() const final override;
};

