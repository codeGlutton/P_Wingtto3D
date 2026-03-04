#include "pch.h"
#include "DXShader.h"

#include "Manager/PathManager.h"
#include "Manager/DXGraphicSystem.h"

#include "Manager/ResourceManager.h"

#include "Graphics/Buffer/DXConstantBuffer.h"
#include "Graphics/State/DXSamplerState.h"
#include "Graphics/Resource/DXTexture.h"
#include "Graphics/Buffer/DXStructureBuffer.h"
#include "Graphics/Buffer/DXComputeTextureBuffer.h"

DXShaderBase::DXShaderBase()
{
	_mResourceFlags = DXResourceFlag::None;
	_mSharedResourceType = DXSharedResourceType::Shader;
}

DXShaderBase::~DXShaderBase()
{
}

DXShader::DXShader()
{
}

DXShader::~DXShader()
{
}

ComPtr<ID3D11ShaderReflection> DXShader::Reflect() const
{
	ComPtr<ID3D11ShaderReflection> reflection;
	CHECK_WIN_MSG(D3DReflect(
		_mBlob->GetBufferPointer(),
		_mBlob->GetBufferSize(),
		IID_ID3D11ShaderReflection,
		reinterpret_cast<void**>(reflection.ReleaseAndGetAddressOf())
	), "Shader reflection is failed");

	return reflection;
}

void DXShader::Load(const wchar_t* path, const char* entryName, const char* version)
{
	std::wstring fullPath = (PATH_MANAGER->GetEngineResourcePath() / L"HLSL" / path).wstring();
	uint32 compileFlag = 0;

#ifdef _DEBUG

	// 디버그 모드이며, 최적화는 스킵
	compileFlag  = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
	ComPtr<ID3DBlob> errorBlob;

	// D3DCompiler.h에 존재
	// 동적으로 파일을 가져와 컴파일
	HRESULT hr = ::D3DCompileFromFile(
		fullPath.c_str(),					// 파일 wstirng 경로
		nullptr,							// (선택) 셰이더에 필요한 매크로
		D3D_COMPILE_STANDARD_FILE_INCLUDE,	// (선택) 셰이더에 필요한 포함 헤더 옵션
		entryName,							// 함수 이름
		version,							// 셰이더 버전
		compileFlag,						// 컴파일 옵션 설정 플래그
		0,									// Effect 컴파일 시에만 사용되는 추가 플래그 옵션
		_mBlob.GetAddressOf(),				// (출력) 컴파일된 코드를 엑세스할 수 있는 버퍼
		errorBlob.GetAddressOf()			// (선택, 출력) 에러 메세지 
	);

	if (FAILED(hr))
	{
		char errorText[1024] = {};
		strcpy_s(errorText, (const char*)errorBlob->GetBufferPointer());
		strcat_s(errorText, "\n");
		OutputDebugStringA(errorText);

		FAIL_MSG("Shader compile error");
	}

#else

	// D3DCompiler.h에 존재
	// 동적으로 파일을 가져와 컴파일
	CHECK_WIN_MSG(::D3DCompileFromFile(
		fullPath.c_str(),					// 파일 wstirng 경로
		nullptr,							// (선택) 셰이더에 필요한 매크로
		D3D_COMPILE_STANDARD_FILE_INCLUDE,	// (선택) 셰이더에 필요한 포함 헤더 옵션
		entryName,							// 함수 이름
		version,							// 셰이더 버전
		compileFlag,						// 컴파일 옵션 설정 플래그
		0,									// Effect 컴파일 시에만 사용되는 추가 플래그 옵션
		_mBlob.ReleaseAndGetAddressOf(),	// (출력) 컴파일된 코드를 엑세스할 수 있는 버퍼
		nullptr								// (선택, 출력) 에러 메세지 
	), "Shader compile error");

#endif
}

DXVertexShader::DXVertexShader()
{
	_mUsageBit = DXResourceUsageBit::VertexBit;
}

DXVertexShader::~DXVertexShader()
{
}

std::shared_ptr<DXShader> DXShader::CreateTransient(DXResourceUsageBit::Type type)
{
	switch (type)
	{
	case DXResourceUsageBit::VertexBit:
		return RESOURCE_MANAGER->CreateTransientRenderResource<DXVertexShader>(DXSharedResourceType::Shader);
	case DXResourceUsageBit::PixelBit:
		return RESOURCE_MANAGER->CreateTransientRenderResource<DXPixelShader>(DXSharedResourceType::Shader);
	case DXResourceUsageBit::DomainBit:
		return RESOURCE_MANAGER->CreateTransientRenderResource<DXDomainShader>(DXSharedResourceType::Shader);
	case DXResourceUsageBit::HullBit:
		return RESOURCE_MANAGER->CreateTransientRenderResource<DXHullShader>(DXSharedResourceType::Shader);
	case DXResourceUsageBit::GeometryBit:
		return RESOURCE_MANAGER->CreateTransientRenderResource<DXGeometryShader>(DXSharedResourceType::Shader);
	case DXResourceUsageBit::ComputeBit:
		return RESOURCE_MANAGER->CreateTransientRenderResource<DXComputeShader>(DXSharedResourceType::Shader);
	}

	return std::shared_ptr<DXShader>();
}

void DXVertexShader::Init(const wchar_t* path, const char* entryName)
{	
	Load(path, entryName, DX_VS_VER);

	CHECK_WIN_MSG(DX_DEVICE->CreateVertexShader(
		_mBlob->GetBufferPointer(),				// Shader 코드 위치
		_mBlob->GetBufferSize(),				// Shader 코드 길이
		nullptr,								// 클래스 링크 포인터 (동적으로 셰이더 내 객체 변경해주는 객체)
		_mShader.ReleaseAndGetAddressOf()		// (출력) Shader 객체
	), "Vertex shader creation is failed");
}

void DXVertexShader::PushData() const
{
	DX_DEVICE_CONTEXT->VSSetShader(_mShader.Get(), nullptr, 0);
}

DXPixelShader::DXPixelShader()
{
	_mUsageBit = DXResourceUsageBit::PixelBit;
}

DXPixelShader::~DXPixelShader()
{
}

void DXPixelShader::Init(const wchar_t* path, const char* entryName)
{
	Load(path, entryName, DX_PS_VER);

	CHECK_WIN_MSG(DX_DEVICE->CreatePixelShader(
		_mBlob->GetBufferPointer(),				// Shader 코드 위치
		_mBlob->GetBufferSize(),				// Shader 코드 길이
		nullptr,								// 클래스 링크 포인터 (동적으로 셰이더 내 객체 변경해주는 객체)
		_mShader.ReleaseAndGetAddressOf()		// (출력) Shader 객체
	), "Pixel shader creation is failed");
}

void DXPixelShader::PushData() const
{
	DX_DEVICE_CONTEXT->PSSetShader(_mShader.Get(), nullptr, 0);
}

DXDomainShader::DXDomainShader()
{
	_mUsageBit = DXResourceUsageBit::DomainBit;
}

DXDomainShader::~DXDomainShader()
{
}

void DXDomainShader::Init(const wchar_t* path, const char* entryName)
{
	Load(path, entryName, DX_DS_VER);

	CHECK_WIN_MSG(DX_DEVICE->CreateDomainShader(
		_mBlob->GetBufferPointer(),				// Shader 코드 위치
		_mBlob->GetBufferSize(),				// Shader 코드 길이
		nullptr,								// 클래스 링크 포인터 (동적으로 셰이더 내 객체 변경해주는 객체)
		_mShader.ReleaseAndGetAddressOf()		// (출력) Shader 객체
	), "Domain shader creation is failed");
}

void DXDomainShader::PushData() const
{
	DX_DEVICE_CONTEXT->DSSetShader(_mShader.Get(), nullptr, 0);
}

DXHullShader::DXHullShader()
{
	_mUsageBit = DXResourceUsageBit::HullBit;
}

DXHullShader::~DXHullShader()
{
}

void DXHullShader::Init(const wchar_t* path, const char* entryName)
{
	Load(path, entryName, DX_HS_VER);

	CHECK_WIN_MSG(DX_DEVICE->CreateHullShader(
		_mBlob->GetBufferPointer(),				// Shader 코드 위치
		_mBlob->GetBufferSize(),				// Shader 코드 길이
		nullptr,								// 클래스 링크 포인터 (동적으로 셰이더 내 객체 변경해주는 객체)
		_mShader.ReleaseAndGetAddressOf()		// (출력) Shader 객체
	), "Hull shader creation is failed");
}

void DXHullShader::PushData() const
{
	DX_DEVICE_CONTEXT->HSSetShader(_mShader.Get(), nullptr, 0);
}

DXGeometryShader::DXGeometryShader()
{
	_mUsageBit = DXResourceUsageBit::GeometryBit;
}

DXGeometryShader::~DXGeometryShader()
{
}

void DXGeometryShader::Init(const wchar_t* path, const char* entryName)
{
	Load(path, entryName, DX_GS_VER);

	CHECK_WIN_MSG(DX_DEVICE->CreateGeometryShader(
		_mBlob->GetBufferPointer(),					// Shader 코드 위치
		_mBlob->GetBufferSize(),					// Shader 코드 길이
		nullptr,									// 클래스 링크 포인터 (동적으로 셰이더 내 객체 변경해주는 객체)
		_mShader.ReleaseAndGetAddressOf()			// (출력) Shader 객체
	), "Geometry shader creation is failed");
}

void DXGeometryShader::PushData() const
{
	DX_DEVICE_CONTEXT->GSSetShader(_mShader.Get(), nullptr, 0);
}

DXComputeShader::DXComputeShader()
{
	_mUsageBit = DXResourceUsageBit::ComputeBit;
}

DXComputeShader::~DXComputeShader()
{
}

inline void DXComputeShader::Bind(const std::string& name, std::shared_ptr<const DXConstantBuffer> buffer) const
{
	const std::unordered_map<std::string, uint32>& slotMap = _mMetaData.mBindingSlots[DXShaderBindingType::CBuffer];
	ASSERT_MSG(slotMap.find(name) != slotMap.end(), "Constant buffer can't be found in compute shader");

	uint32 slotIndex = slotMap.at(name);
	buffer->SetSlot(slotIndex);
	buffer->SetUsageFlags(DXResourceUsageFlag::Compute);
	buffer->PushData();
}

void DXComputeShader::Bind(const std::string& name, std::shared_ptr<const DXTextureBase> texture) const
{
	const std::unordered_map<std::string, uint32>& slotMap = _mMetaData.mBindingSlots[DXShaderBindingType::Texture];
	uint32 slotIndex = slotMap.at(name);
	texture->SetSRVSlot(slotIndex);
	texture->SetSRVUsageFlags(DXResourceUsageFlag::Compute);
	texture->PushData();
}

void DXComputeShader::Bind(const std::string& name, std::shared_ptr<const DXSamplerState> sampler) const
{
	const std::unordered_map<std::string, uint32>& slotMap = _mMetaData.mBindingSlots[DXShaderBindingType::Sampler];
	uint32 slotIndex = slotMap.at(name);
	sampler->SetSlot(slotIndex);
	sampler->SetUsageFlags(DXResourceUsageFlag::Compute);
	sampler->PushData();
}

void DXComputeShader::Bind(const std::string& inputName, const std::string& outputName, std::shared_ptr<const DXStructureBuffer> buffer) const
{
	{
		const std::unordered_map<std::string, uint32>& slotMap = _mMetaData.mBindingSlots[DXShaderBindingType::Structure];
		uint32 slotIndex = slotMap.at(inputName);
		buffer->SetInputSlot(slotIndex);
	}
	{
		const std::unordered_map<std::string, uint32>& slotMap = _mMetaData.mBindingSlots[DXShaderBindingType::RWStructure];
		uint32 slotIndex = slotMap.at(outputName);
		buffer->SetOutputSlot(slotIndex);
	}
	buffer->PushData();
}

void DXComputeShader::Bind(const std::string& inputName, const std::string& outputName, std::shared_ptr<const DXComputeTextureBuffer> buffer) const
{
	{
		const std::unordered_map<std::string, uint32>& slotMap = _mMetaData.mBindingSlots[DXShaderBindingType::Texture];
		uint32 slotIndex = slotMap.at(inputName);
		buffer->SetInputSlot(slotIndex);
	}
	{
		const std::unordered_map<std::string, uint32>& slotMap = _mMetaData.mBindingSlots[DXShaderBindingType::RWTexture];
		uint32 slotIndex = slotMap.at(outputName);
		buffer->SetOutputSlot(slotIndex);
	}
	buffer->PushData();
}

void DXComputeShader::Init(const wchar_t* path, const char* entryName)
{
	Load(path, entryName, DX_CS_VER);

	CHECK_WIN_MSG(DX_DEVICE->CreateComputeShader(
		_mBlob->GetBufferPointer(),					// Shader 코드 위치
		_mBlob->GetBufferSize(),					// Shader 코드 길이
		nullptr,									// 클래스 링크 포인터 (동적으로 셰이더 내 객체 변경해주는 객체)
		_mShader.ReleaseAndGetAddressOf()			// (출력) Shader 객체
	), "Compute shader creation is failed");

	MakeMetaData();
}

void DXComputeShader::PushData() const
{
	DX_DEVICE_CONTEXT->CSSetShader(_mShader.Get(), nullptr, 0);
}

void DXComputeShader::MakeMetaData()
{
	/* 리플렉션 저장 */

	_mMetaData.Clear();
	if (_mShader == nullptr)
	{
		return;
	}

	ComPtr<ID3D11ShaderReflection> reflection = Reflect();

	D3D11_SHADER_DESC desc;
	memset(&desc, 0, sizeof(desc));
	CHECK_WIN_MSG(reflection->GetDesc(&desc), "Shader reflection desc access is failed");

	// 바인딩 항목들
	const uint32 boundCount = desc.BoundResources;
	for (uint32 i = 0; i < boundCount; ++i)
	{
		D3D11_SHADER_INPUT_BIND_DESC bindDesc;
		CHECK_WIN_MSG(reflection->GetResourceBindingDesc(i, &bindDesc), "Bound resource reflection desc access is failed");

		switch (bindDesc.Type)
		{
		case D3D_SIT_CBUFFER:			// 상수 버퍼 (cbuffer)
			_mMetaData.mBindingSlots[DXShaderBindingType::CBuffer][bindDesc.Name] = bindDesc.BindPoint;
			break;
		case D3D_SIT_TEXTURE:			// 텍스처 (Texture2D, Texture2DArray, ...)
			_mMetaData.mBindingSlots[DXShaderBindingType::Texture][bindDesc.Name] = bindDesc.BindPoint;
			break;
		case D3D_SIT_UAV_RWTYPED:		// RW 텍스처 (RWTexture2D, RWTexture2DArray, ...)
			_mMetaData.mBindingSlots[DXShaderBindingType::RWTexture][bindDesc.Name] = bindDesc.BindPoint;
			break;
		case D3D_SIT_SAMPLER:			// 샘플러 (SamplerState)
			_mMetaData.mBindingSlots[DXShaderBindingType::Sampler][bindDesc.Name] = bindDesc.BindPoint;
			break;
		case D3D_SIT_STRUCTURED:		// 구조화된 버퍼 (StructuredBuffer)
			_mMetaData.mBindingSlots[DXShaderBindingType::Structure][bindDesc.Name] = bindDesc.BindPoint;
			break;
		case D3D_SIT_UAV_RWSTRUCTURED:	// RW 구조화된 버퍼 (RWStructuredBuffer)
			_mMetaData.mBindingSlots[DXShaderBindingType::RWStructure][bindDesc.Name] = bindDesc.BindPoint;
			break;
		}
	}
}

DXMinimumGraphicShader::DXMinimumGraphicShader()
{
}

DXMinimumGraphicShader::~DXMinimumGraphicShader()
{
}

void DXMinimumGraphicShader::Bind(const std::string& name, std::shared_ptr<const DXConstantBuffer> buffer) const
{
	const std::unordered_map<std::string, DXBindingMetaData>& slotMap = _mMetaData.mBindingSlots[DXShaderBindingType::CBuffer];
	ASSERT_MSG(slotMap.find(name) != slotMap.end(), "Constant buffer can't be found in graphic shader");

	const DXBindingMetaData& metaData = slotMap.at(name);
	buffer->SetSlot(metaData.mSlotIndex);
	buffer->SetUsageFlags(metaData.mUsageFlags);
	buffer->PushData();
}

void DXMinimumGraphicShader::Bind(const std::string& name, std::shared_ptr<const DXTextureBase> texture) const
{
	const std::unordered_map<std::string, DXBindingMetaData>& slotMap = _mMetaData.mBindingSlots[DXShaderBindingType::Texture];
	const DXBindingMetaData& metaData = slotMap.at(name);
	texture->SetSRVSlot(metaData.mSlotIndex);
	texture->SetSRVUsageFlags(metaData.mUsageFlags);
	texture->PushData();
}

void DXMinimumGraphicShader::Bind(const std::string& name, std::shared_ptr<const DXSamplerState> sampler) const
{
	const std::unordered_map<std::string, DXBindingMetaData>& slotMap = _mMetaData.mBindingSlots[DXShaderBindingType::Sampler];
	const DXBindingMetaData& metaData = slotMap.at(name);
	sampler->SetSlot(metaData.mSlotIndex);
	sampler->SetUsageFlags(metaData.mUsageFlags);
	sampler->PushData();
}

void DXMinimumGraphicShader::Init(const DXInputElementList& elementList, const std::vector<std::shared_ptr<DXShader>>& shaders)
{
	_mShaders.fill(nullptr);

	_mUsageFlags = DXResourceUsageFlag::None;
	for (const std::shared_ptr<DXShader>& shader : shaders)
	{
		_mShaders[shader->GetUsageBit()] = shader;
		_mUsageFlags |= shader->GetUsageFlag();
	}

	std::shared_ptr<DXVertexShader> vs = std::static_pointer_cast<DXVertexShader>(_mShaders[DXResourceUsageBit::VertexBit]);
	ASSERT_MSG(vs != nullptr, "Shader can't initialize with null vs");

	MakeMetaData();
	_mLayout.Init(elementList, *vs);
}

void DXMinimumGraphicShader::Init(const DXInputElementList& elementList, const std::vector<ShaderCompileDesc>& shaderDescs, const wchar_t* path)
{
	ASSERT_MSG(path != nullptr, "Transient shader can't be created and initialize with null path");

	_mShaders.fill(nullptr);

	_mUsageFlags = DXResourceUsageFlag::None;
	for (const ShaderCompileDesc& desc : shaderDescs)
	{
		std::shared_ptr<DXShader> transientShader = DXShader::CreateTransient(desc.mUsageBit);

		_mShaders[transientShader->GetUsageBit()] = transientShader;
		_mUsageFlags |= transientShader->GetUsageFlag();
		transientShader->Init(path, desc.mEntryName.c_str());
	}

	std::shared_ptr<DXVertexShader> vs = std::static_pointer_cast<DXVertexShader>(_mShaders[DXResourceUsageBit::VertexBit]);
	ASSERT_MSG(vs != nullptr, "Shader can't initialize with null vs");

	MakeMetaData();
	_mLayout.Init(elementList, *vs);
}

void DXMinimumGraphicShader::Init(const DXInputElementList& elementList, std::shared_ptr<DXVertexShader> vs, std::shared_ptr<DXPixelShader> ps)
{
	ASSERT_MSG(vs != nullptr, "Shader can't initialize with null vs");

	_mShaders.fill(nullptr);

	std::vector<std::shared_ptr<DXShader>> shaders = { vs };
	if (ps != nullptr)
	{
		shaders.push_back(ps);
	}

	Init(elementList, shaders);
}

void DXMinimumGraphicShader::Init(const DXInputElementList& elementList, const wchar_t* path, const char* vsEntryName, const char* psEntryName)
{
	ASSERT_MSG(vsEntryName != nullptr, "Shader can't initialize with null vs");

	_mShaders.fill(nullptr);

	std::vector<ShaderCompileDesc> descs = {
		ShaderCompileDesc{ vsEntryName, DXResourceUsageBit::VertexBit}
	};

	if (psEntryName != nullptr)
	{
		descs.push_back(
			ShaderCompileDesc{ psEntryName, DXResourceUsageBit::PixelBit }
		);
	}

	Init(elementList, descs, path);
}

void DXMinimumGraphicShader::PushData() const
{
	_mShaders[DXResourceUsageBit::VertexBit]->PushData();
	_mShaders[DXResourceUsageBit::PixelBit]->PushData();
	_mLayout.PushData();
}

void DXMinimumGraphicShader::MakeMetaData()
{
	/* 리플렉션 저장 */

	_mMetaData.Clear();
	for (std::shared_ptr<DXShader> shader : _mShaders)
	{
		if (shader == nullptr)
		{
			continue;
		}

		ComPtr<ID3D11ShaderReflection> reflection = shader->Reflect();

		D3D11_SHADER_DESC desc;
		memset(&desc, 0, sizeof(desc));
		CHECK_WIN_MSG(reflection->GetDesc(&desc), "Shader reflection desc access is failed");
		
		if (shader->GetUsageBit() == DXResourceUsageBit::VertexBit)
		{
			// 입력 레이아웃
			_mMetaData.mInputLayout.mParams.resize(desc.InputParameters);
			const uint32 inputParamCount = desc.InputParameters;
			for (uint32 i = 0; i < inputParamCount; ++i)
			{
				DXParameterMetaData& paramMeta = _mMetaData.mInputLayout.mParams[i];

				D3D11_SIGNATURE_PARAMETER_DESC paramDesc;
				CHECK_WIN_MSG(reflection->GetInputParameterDesc(i, &paramDesc), "Shader input param reflection desc access is failed");
				paramMeta.mSemanticIndex = paramDesc.SemanticIndex;
				paramMeta.mSemanticName = paramDesc.SemanticName;
			}
		}

		// 바인딩 항목들
		const uint32 boundCount = desc.BoundResources;
		for (uint32 i = 0; i < boundCount; ++i)
		{
			D3D11_SHADER_INPUT_BIND_DESC bindDesc;
			CHECK_WIN_MSG(reflection->GetResourceBindingDesc(i, &bindDesc), "Bound resource reflection desc access is failed");

			switch (bindDesc.Type)
			{
			case D3D_SIT_CBUFFER:			// 상수 버퍼 (cbuffer)
				_mMetaData.mBindingSlots[DXShaderBindingType::CBuffer][bindDesc.Name].mSlotIndex = bindDesc.BindPoint;
				_mMetaData.mBindingSlots[DXShaderBindingType::CBuffer][bindDesc.Name].mUsageFlags |= shader->GetUsageFlag();
				break;
			case D3D_SIT_TEXTURE:			// 텍스처 (Texture2D, Texture2DArray, ...)
				_mMetaData.mBindingSlots[DXShaderBindingType::Texture][bindDesc.Name].mSlotIndex = bindDesc.BindPoint;
				_mMetaData.mBindingSlots[DXShaderBindingType::Texture][bindDesc.Name].mUsageFlags |= shader->GetUsageFlag();
				break;
			case D3D_SIT_UAV_RWTYPED:		// RW 텍스처 (RWTexture2D, RWTexture2DArray, ...)
				_mMetaData.mBindingSlots[DXShaderBindingType::RWTexture][bindDesc.Name].mSlotIndex = bindDesc.BindPoint;
				_mMetaData.mBindingSlots[DXShaderBindingType::RWTexture][bindDesc.Name].mUsageFlags |= shader->GetUsageFlag();
				break;
			case D3D_SIT_SAMPLER:			// 샘플러 (SamplerState)
				_mMetaData.mBindingSlots[DXShaderBindingType::Sampler][bindDesc.Name].mSlotIndex = bindDesc.BindPoint;
				_mMetaData.mBindingSlots[DXShaderBindingType::Sampler][bindDesc.Name].mUsageFlags |= shader->GetUsageFlag();
				break;
			case D3D_SIT_STRUCTURED:		// 구조화된 버퍼 (StructuredBuffer)
				_mMetaData.mBindingSlots[DXShaderBindingType::Structure][bindDesc.Name].mSlotIndex = bindDesc.BindPoint;
				_mMetaData.mBindingSlots[DXShaderBindingType::Structure][bindDesc.Name].mUsageFlags |= shader->GetUsageFlag();
				break;
			case D3D_SIT_UAV_RWSTRUCTURED:	// RW 구조화된 버퍼 (RWStructuredBuffer)
				_mMetaData.mBindingSlots[DXShaderBindingType::RWStructure][bindDesc.Name].mSlotIndex = bindDesc.BindPoint;
				_mMetaData.mBindingSlots[DXShaderBindingType::RWStructure][bindDesc.Name].mUsageFlags |= shader->GetUsageFlag();
				break;
			}
		}
	}
}

DXGraphicShader::DXGraphicShader()
{
}

DXGraphicShader::~DXGraphicShader()
{
}

void DXGraphicShader::Init(const DXInputElementList& elementList, const std::vector<std::shared_ptr<DXShader>>& shaders)
{
	DXMinimumGraphicShader::Init(elementList, shaders);
}

void DXGraphicShader::Init(const DXInputElementList& elementList, const std::vector<ShaderCompileDesc>& shaderDescs, const wchar_t* path)
{
	DXMinimumGraphicShader::Init(elementList, shaderDescs, path);
}

void DXGraphicShader::PushData() const
{
	for (const std::shared_ptr<DXShader>& shader : _mShaders)
	{
		if (shader != nullptr)
		{
			shader->PushData();
		}
	}
	_mLayout.PushData();
}

