#pragma once
#include "Graphics/Resource/DXResource.h"

class DXStructureBuffer : public DXResource
{
public:
	DXStructureBuffer();
	~DXStructureBuffer();

public:
	ComPtr<ID3D11ShaderResourceView> GetSRV() 
	{
		return _mInputSRV;
	}
	ComPtr<ID3D11UnorderedAccessView> GetUAV() 
	{ 
		return _mOutputUAV;
	}

public:
	uint32 GetInputByteWidth() const
	{ 
		return _mInputStride * _mInputCount;
	}
	uint32 GetOutputByteWidth() const
	{ 
		return _mOutputStride * _mOutputCount;
	}

public:
	uint32 GetInputSlot() const
	{
		return _mInputSlot;
	}
	uint32 GetInputCount() const
	{
		return _mInputCount;
	}
	uint32 GetOutputSlot() const
	{
		return _mOutputSlot;
	}
	uint32 GetOutputCount() const
	{
		return _mOutputCount;
	}

	void SetInputSlot(uint32 slot) const
	{
		_mInputSlot = slot;
	}
	void SetOutputSlot(uint32 slot) const
	{
		_mOutputSlot = slot;
	}

public:
	void Init(const void* initData, uint32 inputStride, uint32 inputCount, uint32 inputSlot, uint32 outputStride, uint32 outputCount, uint32 outputSlot, bool canCpuWrite = true);

public:
	void PushData() const;
	bool UpdateData(const void* data) const;
	void PopData(OUT void* data) const;

private:
	void CreateBuffer(const void* initData);

private:
	void CreateInput(const void* initData);
	void CreateSRV();
	void CreateOutput();
	void CreateUAV();
	void CreateResult();

private:
	/* 입력 */
	ComPtr<ID3D11Buffer> _mInputBuffer;
	ComPtr<ID3D11ShaderResourceView> _mInputSRV;

	/* 출력 */
	ComPtr<ID3D11Buffer> _mOutputBuffer;
	ComPtr<ID3D11UnorderedAccessView> _mOutputUAV;

	/* 결과 */
	// GPU에 저장된 결과를 CPU가 가져오기 위함
	ComPtr<ID3D11Buffer> _mResultBuffer;

private:
	uint32 _mInputStride = 0;
	uint32 _mInputCount = 0;
	uint32 _mOutputStride = 0;
	uint32 _mOutputCount = 0;

	mutable uint32 _mInputSlot = 0;
	mutable uint32 _mOutputSlot = 0;
};

template<typename In, std::size_t InN, typename Out = In, std::size_t OutN = InN>
class DXTypedStructureBuffer : public DXStructureBuffer
{
public:
	void Init(const In& initData);

public:
	bool UpdateData(const In& data) const;
	void PopData(OUT Out& data) const;
};

template<typename In, std::size_t InN, typename Out, std::size_t OutN>
inline void DXTypedStructureBuffer<In, InN, Out, OutN>::Init(const In& initData)
{
	DXStructureBuffer::Init(&initData, sizeof(In), InN, sizeof(Out), OutN);
}

template<typename In, std::size_t InN, typename Out, std::size_t OutN>
inline bool DXTypedStructureBuffer<In, InN, Out, OutN>::UpdateData(const In& data) const
{
	return DXStructureBuffer::PushData(&data);
}

template<typename In, std::size_t InN, typename Out, std::size_t OutN>
inline void DXTypedStructureBuffer<In, InN, Out, OutN>::PopData(OUT Out& data) const
{
	DXStructureBuffer::PopData(&data);
}
