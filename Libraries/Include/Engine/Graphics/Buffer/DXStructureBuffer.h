#pragma once

class DXStructureBuffer : public std::enable_shared_from_this<DXStructureBuffer>
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
	uint32 GetInputByteWidth() 
	{ 
		return _mInputStride * _mInputCount;
	}
	uint32 GetOutputByteWidth() 
	{ 
		return _mOutputStride * _mOutputCount;
	}

public:
	void Init(const void* initData, uint32 inputStride, uint32 inputCount, uint32 outputStride, uint32 outputCount);

public:
	void PushData(const void* data);
	void PopData(OUT void* data);

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
};

template<typename In, std::size_t InN, typename Out = In, std::size_t OutN = InN>
class DXTypedStructureBuffer : public DXStructureBuffer
{
public:
	void Init(const In& initData);

public:
	void PushData(const In& data);
	void PopData(OUT Out& data);
};

template<typename In, std::size_t InN, typename Out, std::size_t OutN>
inline void DXTypedStructureBuffer<In, InN, Out, OutN>::Init(const In& initData)
{
	DXStructureBuffer::Init(&initData, sizeof(In), InN, sizeof(Out), OutN);
}

template<typename In, std::size_t InN, typename Out, std::size_t OutN>
inline void DXTypedStructureBuffer<In, InN, Out, OutN>::PushData(const In& data)
{
	DXStructureBuffer::PushData(&data);
}

template<typename In, std::size_t InN, typename Out, std::size_t OutN>
inline void DXTypedStructureBuffer<In, InN, Out, OutN>::PopData(OUT Out& data)
{
	DXStructureBuffer::PopData(&data);
}
