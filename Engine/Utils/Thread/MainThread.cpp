#include "pch.h"
#include "MainThread.h"

#include "Manager/ThreadManager.h"
#include "Manager/RenderManager.h"
#include "Manager/TimeManager.h"

#include "Manager/AppWindowManager.h"

#include "Manager/ResourceManager.h"
#include "Graphics/DXDefaultVertexData.h"
#include "Graphics/Resource/DXShader.h"
#include "Graphics/State/DXSamplerState.h"
#include "Graphics/State/DXBlendState.h"
#include "Graphics/State/DXRasterizerState.h"

void MainThread::Run()
{
	while (THREAD_MANAGER->IsAlive() == true)
	{
		LEndTickCount = GetTickCount64() + mThreadMaxTick;
		Work();
	}
}

void RenderThread::Init()
{
	RENDER_TIME_MANAGER->Init();
	RegisterDefaultRuntimeResources();
}

void RenderThread::Work()
{
	RENDER_TIME_MANAGER->Update();
	
	THREAD_MANAGER->DoRenderJob();
}

void RenderThread::Destroy()
{
	UnregisterDefaultRuntimeResources();
	RENDER_TIME_MANAGER->Destroy();
}

void RenderThread::RegisterDefaultRuntimeResources()
{
	/* 쉐이더 */

	// UI
	{
		std::shared_ptr<DXGraphicShader> shader = RESOURCE_MANAGER->CreateOrGetRuntimeRenderResource<DXGraphicShader>(L"UI", DXSharedResourceType::Shader);

		std::vector<ShaderCompileDesc> descs;
		{
			ShaderCompileDesc desc;
			desc.mEntryName = "VS";
			desc.mUsageBit = DXResourceUsageBit::VertexBit;
			descs.push_back(desc);
		}
		{
			ShaderCompileDesc desc;
			desc.mEntryName = "PS";
			desc.mUsageBit = DXResourceUsageBit::PixelBit;
			descs.push_back(desc);
		}
		shader->Init(UIVertexData::MakeLayout(), descs, L"UIRender.hlsl");

		_mDefaultResources.push_back(shader);
	}
	// 테스트
	{
		std::shared_ptr<DXGraphicShader> shader = RESOURCE_MANAGER->CreateOrGetRuntimeRenderResource<DXGraphicShader>(L"Test", DXSharedResourceType::Shader);

		std::vector<ShaderCompileDesc> descs;
		{
			ShaderCompileDesc desc;
			desc.mEntryName = "VS";
			desc.mUsageBit = DXResourceUsageBit::VertexBit;
			descs.push_back(desc);
		}
		{
			ShaderCompileDesc desc;
			desc.mEntryName = "PS";
			desc.mUsageBit = DXResourceUsageBit::PixelBit;
			descs.push_back(desc);
		}
		shader->Init(NullVertexData::MakeLayout(), descs, L"TestQuadRender.hlsl");

		_mDefaultResources.push_back(shader);
	}


	/* 샘플러 스테이트 */

	// Linear wrap
	{
		std::shared_ptr<DXSamplerState> sampler = RESOURCE_MANAGER->CreateOrGetRuntimeRenderResource<DXSamplerState>(L"LinearWrap", DXSharedResourceType::SamplerState);
		
		sampler->Init(
			D3D11_FILTER_MIN_MAG_MIP_LINEAR,
			D3D11_COMPARISON_NEVER,
			D3D11_TEXTURE_ADDRESS_WRAP,
			D3D11_TEXTURE_ADDRESS_WRAP,
			D3D11_TEXTURE_ADDRESS_WRAP
		);

		_mDefaultResources.push_back(sampler);
	}
	// Point wrap
	{
		std::shared_ptr<DXSamplerState> sampler = RESOURCE_MANAGER->CreateOrGetRuntimeRenderResource<DXSamplerState>(L"PointWrap", DXSharedResourceType::SamplerState);

		sampler->Init(
			D3D11_FILTER_MIN_MAG_MIP_POINT,
			D3D11_COMPARISON_NEVER,
			D3D11_TEXTURE_ADDRESS_WRAP,
			D3D11_TEXTURE_ADDRESS_WRAP,
			D3D11_TEXTURE_ADDRESS_WRAP
		);

		_mDefaultResources.push_back(sampler);
	}

	/* 블렌드 스테이트 */

	// 기본 알파
	{
		std::shared_ptr<DXBlendState> state = RESOURCE_MANAGER->CreateOrGetRuntimeRenderResource<DXBlendState>(L"DefaultAlpha", DXSharedResourceType::BlendState);

		D3D11_RENDER_TARGET_BLEND_DESC desc;
		desc.BlendEnable = true;
		desc.SrcBlend = D3D11_BLEND_SRC_ALPHA;
		desc.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		desc.BlendOp = D3D11_BLEND_OP_ADD;
		desc.SrcBlendAlpha = D3D11_BLEND_ONE;
		desc.DestBlendAlpha = D3D11_BLEND_ZERO;
		desc.BlendOpAlpha = D3D11_BLEND_OP_ADD;
		desc.RenderTargetWriteMask = 0x0F;

		state->Init(desc);

		_mDefaultResources.push_back(state);
	}

	/* 레스터라이저 스테이트 */

	// 기본 뒷면 컬링
	{
		std::shared_ptr<DXRasterizerState> state = RESOURCE_MANAGER->CreateOrGetRuntimeRenderResource<DXRasterizerState>(L"BackCulling", DXSharedResourceType::RasterizerState);

		state->Init(D3D11_FILL_SOLID, D3D11_CULL_BACK, true);

		_mDefaultResources.push_back(state);
	}
	// 정면 컬링
	{
		std::shared_ptr<DXRasterizerState> state = RESOURCE_MANAGER->CreateOrGetRuntimeRenderResource<DXRasterizerState>(L"FrontCulling", DXSharedResourceType::RasterizerState);

		state->Init(D3D11_FILL_SOLID, D3D11_CULL_BACK, false);

		_mDefaultResources.push_back(state);
	}
	// 양면 렌더
	{
		std::shared_ptr<DXRasterizerState> state = RESOURCE_MANAGER->CreateOrGetRuntimeRenderResource<DXRasterizerState>(L"NoneCulling", DXSharedResourceType::RasterizerState);

		state->Init(D3D11_FILL_SOLID, D3D11_CULL_NONE, true);

		_mDefaultResources.push_back(state);
	}
	// 와이어 프레임
	{
		std::shared_ptr<DXRasterizerState> state = RESOURCE_MANAGER->CreateOrGetRuntimeRenderResource<DXRasterizerState>(L"WireFrame", DXSharedResourceType::RasterizerState);

		state->Init(D3D11_FILL_WIREFRAME, D3D11_CULL_NONE, true);

		_mDefaultResources.push_back(state);
	}

	/* 텍스처 */

	// 기본 흰 배경
	{
		std::shared_ptr<DXConstTexture2D> texture = RESOURCE_MANAGER->CreateOrGetRuntimeRenderResource<DXConstTexture2D>(L"White", DXSharedResourceType::Texture);

		const float whitePixel[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
		texture->Init(&whitePixel, 1, 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, DXResourceUsageFlag::Pixel);

		_mDefaultResources.push_back(texture);
	}
}

void RenderThread::UnregisterDefaultRuntimeResources()
{
	_mDefaultResources.clear();
}
