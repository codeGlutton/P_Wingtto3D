#pragma once

#include "Graphics/DXReflectionMetaData.h"

class DXVertexShader;

struct DXInputElementList : public std::vector<D3D11_INPUT_ELEMENT_DESC>
{
private:
	using vector<D3D11_INPUT_ELEMENT_DESC>::vector;
	DXInputElementList();

public:
	static DXInputElementList Make(std::size_t reserve = 0)
	{
		return DXInputElementList(reserve);
	}

public:
	inline DXInputElementList& Push(D3D11_INPUT_ELEMENT_DESC&& desc)
	{
		push_back(desc);
		return *this;
	}
	inline DXInputElementList& Push(const D3D11_INPUT_ELEMENT_DESC& desc)
	{
		push_back(desc);
		return *this;
	}
	inline DXInputElementList& Push(const char* segmentName, uint32 segmentIndex, DXGI_FORMAT format, uint32 slotIndex = 0, bool isInstancing = false, uint32 instancingStep = 0)
	{
		push_back(
			D3D11_INPUT_ELEMENT_DESC{
				segmentName, 
				segmentIndex, 
				format, 
				slotIndex,
				D3D11_APPEND_ALIGNED_ELEMENT,
				isInstancing == false ? D3D11_INPUT_PER_VERTEX_DATA : D3D11_INPUT_PER_INSTANCE_DATA, 
				instancingStep
			}
		);
		return *this;
	}

	/* float */

	inline DXInputElementList& PushFloat(const char* segmentName, uint32 segmentIndex, uint32 slotIndex = 0, bool isInstancing = false, uint32 instancingStep = 0)
	{
		return Push(segmentName, segmentIndex, DXGI_FORMAT_R32_FLOAT, slotIndex, isInstancing, instancingStep);
	}
	inline DXInputElementList& Push2Float(const char* segmentName, uint32 segmentIndex, uint32 slotIndex = 0, bool isInstancing = false, uint32 instancingStep = 0)
	{
		return Push(segmentName, segmentIndex, DXGI_FORMAT_R32G32_FLOAT, slotIndex, isInstancing, instancingStep);
	}
	inline DXInputElementList& Push3Float(const char* segmentName, uint32 segmentIndex, uint32 slotIndex = 0, bool isInstancing = false, uint32 instancingStep = 0)
	{
		return Push(segmentName, segmentIndex, DXGI_FORMAT_R32G32B32_FLOAT, slotIndex, isInstancing, instancingStep);
	}
	inline DXInputElementList& Push4Float(const char* segmentName, uint32 segmentIndex, uint32 slotIndex = 0, bool isInstancing = false, uint32 instancingStep = 0)
	{
		return Push(segmentName, segmentIndex, DXGI_FORMAT_R32G32B32A32_FLOAT, slotIndex, isInstancing, instancingStep);
	}

	/* int8 */

	inline DXInputElementList& PushInt8(const char* segmentName, uint32 segmentIndex, uint32 slotIndex = 0, bool isInstancing = false, uint32 instancingStep = 0)
	{
		return Push(segmentName, segmentIndex, DXGI_FORMAT_R8_SINT, slotIndex, isInstancing, instancingStep);
	}
	inline DXInputElementList& Push2Int8(const char* segmentName, uint32 segmentIndex, uint32 slotIndex = 0, bool isInstancing = false, uint32 instancingStep = 0)
	{
		return Push(segmentName, segmentIndex, DXGI_FORMAT_R8G8_SINT, slotIndex, isInstancing, instancingStep);
	}
	inline DXInputElementList& Push4Int8(const char* segmentName, uint32 segmentIndex, uint32 slotIndex = 0, bool isInstancing = false, uint32 instancingStep = 0)
	{
		return Push(segmentName, segmentIndex, DXGI_FORMAT_R8G8B8A8_SINT, slotIndex, isInstancing, instancingStep);
	}
	inline DXInputElementList& PushUInt8(const char* segmentName, uint32 segmentIndex, uint32 slotIndex = 0, bool isInstancing = false, uint32 instancingStep = 0)
	{
		return Push(segmentName, segmentIndex, DXGI_FORMAT_R8_UINT, slotIndex, isInstancing, instancingStep);
	}
	inline DXInputElementList& Push2UInt8(const char* segmentName, uint32 segmentIndex, uint32 slotIndex = 0, bool isInstancing = false, uint32 instancingStep = 0)
	{
		return Push(segmentName, segmentIndex, DXGI_FORMAT_R8G8_UINT, slotIndex, isInstancing, instancingStep);
	}
	inline DXInputElementList& Push4UInt8(const char* segmentName, uint32 segmentIndex, uint32 slotIndex = 0, bool isInstancing = false, uint32 instancingStep = 0)
	{
		return Push(segmentName, segmentIndex, DXGI_FORMAT_R8G8B8A8_UINT, slotIndex, isInstancing, instancingStep);
	}

	/* int16 */

	inline DXInputElementList& PushInt16(const char* segmentName, uint32 segmentIndex, uint32 slotIndex = 0, bool isInstancing = false, uint32 instancingStep = 0)
	{
		return Push(segmentName, segmentIndex, DXGI_FORMAT_R16_SINT, slotIndex, isInstancing, instancingStep);
	}
	inline DXInputElementList& Push2Int16(const char* segmentName, uint32 segmentIndex, uint32 slotIndex = 0, bool isInstancing = false, uint32 instancingStep = 0)
	{
		return Push(segmentName, segmentIndex, DXGI_FORMAT_R16G16_SINT, slotIndex, isInstancing, instancingStep);
	}
	inline DXInputElementList& Push4Int16(const char* segmentName, uint32 segmentIndex, uint32 slotIndex = 0, bool isInstancing = false, uint32 instancingStep = 0)
	{
		return Push(segmentName, segmentIndex, DXGI_FORMAT_R16G16B16A16_SINT, slotIndex, isInstancing, instancingStep);
	}
	inline DXInputElementList& PushUInt16(const char* segmentName, uint32 segmentIndex, uint32 slotIndex = 0, bool isInstancing = false, uint32 instancingStep = 0)
	{
		return Push(segmentName, segmentIndex, DXGI_FORMAT_R16_UINT, slotIndex, isInstancing, instancingStep);
	}
	inline DXInputElementList& Push2UInt16(const char* segmentName, uint32 segmentIndex, uint32 slotIndex = 0, bool isInstancing = false, uint32 instancingStep = 0)
	{
		return Push(segmentName, segmentIndex, DXGI_FORMAT_R16G16_UINT, slotIndex, isInstancing, instancingStep);
	}
	inline DXInputElementList& Push4UInt16(const char* segmentName, uint32 segmentIndex, uint32 slotIndex = 0, bool isInstancing = false, uint32 instancingStep = 0)
	{
		return Push(segmentName, segmentIndex, DXGI_FORMAT_R16G16B16A16_UINT, slotIndex, isInstancing, instancingStep);
	}

	/* int32 */

	inline DXInputElementList& PushInt32(const char* segmentName, uint32 segmentIndex, uint32 slotIndex = 0, bool isInstancing = false, uint32 instancingStep = 0)
	{
		return Push(segmentName, segmentIndex, DXGI_FORMAT_R32_SINT, slotIndex, isInstancing, instancingStep);
	}
	inline DXInputElementList& Push2Int32(const char* segmentName, uint32 segmentIndex, uint32 slotIndex = 0, bool isInstancing = false, uint32 instancingStep = 0)
	{
		return Push(segmentName, segmentIndex, DXGI_FORMAT_R32G32_SINT, slotIndex, isInstancing, instancingStep);
	}
	inline DXInputElementList& Push3Int32(const char* segmentName, uint32 segmentIndex, uint32 slotIndex = 0, bool isInstancing = false, uint32 instancingStep = 0)
	{
		return Push(segmentName, segmentIndex, DXGI_FORMAT_R32G32B32_SINT, slotIndex, isInstancing, instancingStep);
	}
	inline DXInputElementList& Push4Int32(const char* segmentName, uint32 segmentIndex, uint32 slotIndex = 0, bool isInstancing = false, uint32 instancingStep = 0)
	{
		return Push(segmentName, segmentIndex, DXGI_FORMAT_R32G32B32A32_SINT, slotIndex, isInstancing, instancingStep);
	}
	inline DXInputElementList& PushUInt32(const char* segmentName, uint32 segmentIndex, uint32 slotIndex = 0, bool isInstancing = false, uint32 instancingStep = 0)
	{
		return Push(segmentName, segmentIndex, DXGI_FORMAT_R32_UINT, slotIndex, isInstancing, instancingStep);
	}
	inline DXInputElementList& Push2UInt32(const char* segmentName, uint32 segmentIndex, uint32 slotIndex = 0, bool isInstancing = false, uint32 instancingStep = 0)
	{
		return Push(segmentName, segmentIndex, DXGI_FORMAT_R32G32_UINT, slotIndex, isInstancing, instancingStep);
	}
	inline DXInputElementList& Push3UInt32(const char* segmentName, uint32 segmentIndex, uint32 slotIndex = 0, bool isInstancing = false, uint32 instancingStep = 0)
	{
		return Push(segmentName, segmentIndex, DXGI_FORMAT_R32G32B32_UINT, slotIndex, isInstancing, instancingStep);
	}
	inline DXInputElementList& Push4UInt32(const char* segmentName, uint32 segmentIndex, uint32 slotIndex = 0, bool isInstancing = false, uint32 instancingStep = 0)
	{
		return Push(segmentName, segmentIndex, DXGI_FORMAT_R32G32B32A32_UINT, slotIndex, isInstancing, instancingStep);
	}

	/* matrix */

	inline DXInputElementList& PushMatrix(const char* segmentName, uint32 segmentIndex, uint32 slotIndex = 0, bool isInstancing = false, uint32 instancingStep = 0)
	{
		Push4Float(segmentName, segmentIndex, slotIndex, isInstancing, instancingStep).
		Push4Float(segmentName, segmentIndex + 1, slotIndex, isInstancing, instancingStep).
		Push4Float(segmentName, segmentIndex + 2, slotIndex, isInstancing, instancingStep).
		Push4Float(segmentName, segmentIndex + 3, slotIndex, isInstancing, instancingStep);
		return *this;
	}
};

class DXInputLayout
{
public:
	DXInputLayout();
	~DXInputLayout();

public:
	ComPtr<ID3D11InputLayout> GetResource()
	{
		return _mLayout;
	}

public:
	void Init(const std::vector<D3D11_INPUT_ELEMENT_DESC>& descs, const DXVertexShader& shader);

public:
	void PushData() const;

private:
	ComPtr<ID3D11InputLayout> _mLayout;
};

