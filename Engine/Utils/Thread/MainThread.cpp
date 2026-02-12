#include "pch.h"
#include "MainThread.h"

#include "Manager/ThreadManager.h"
#include "Manager/RenderManager.h"
#include "Manager/TimeManager.h"

#include "Manager/AppWindowManager.h"

#include "Manager/ResourceManager.h"
#include "Graphics/DXDefaultVertexData.h"
#include "Graphics/Resource/DXShader.h"
#include "Graphics/State/DXBlendState.h"

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
	RegisterDefaultResources();
}

void RenderThread::Work()
{
	RENDER_TIME_MANAGER->Update();
	
	THREAD_MANAGER->DoRenderJob();
}

void RenderThread::Destroy()
{
	UnregisterDefaultResources();
	RENDER_TIME_MANAGER->Destroy();
}

void RenderThread::RegisterDefaultResources()
{
	///* 쉐이더 */

	//// 월드 스태틱
	//{
	//	std::shared_ptr<DXGraphicShader> shader = RESOURCE_MANAGER->CreateOrGetTransientRenderResource<DXGraphicShader>(L"World_Static", DXSharedResourceType::Shader);

	//	std::vector<ShaderCompileDesc> descs;
	//	{
	//		ShaderCompileDesc desc;
	//		desc.mEntryName = "LS";
	//		desc.mUsageBit = DXResourceUsageBit::VertexBit;
	//		descs.push_back(desc);
	//	}
	//	{
	//		ShaderCompileDesc desc;
	//		desc.mEntryName = "PS";
	//		desc.mUsageBit = DXResourceUsageBit::PixelBit;
	//		descs.push_back(desc);
	//	}
	//	shader->Init(StaticVertexData::MakeLayout(), descs, L"경로");

	//	_mDefaultResources.push_back(shader);
	//}
	//// 월드 스켈레탈
	//{
	//	std::shared_ptr<DXGraphicShader> shader = RESOURCE_MANAGER->CreateOrGetTransientRenderResource<DXGraphicShader>(L"World_Skeletal", DXSharedResourceType::Shader);

	//	std::vector<ShaderCompileDesc> descs;
	//	{
	//		ShaderCompileDesc desc;
	//		desc.mEntryName = "LS";
	//		desc.mUsageBit = DXResourceUsageBit::VertexBit;
	//		descs.push_back(desc);
	//	}
	//	{
	//		ShaderCompileDesc desc;
	//		desc.mEntryName = "PS";
	//		desc.mUsageBit = DXResourceUsageBit::PixelBit;
	//		descs.push_back(desc);
	//	}
	//	shader->Init(SkeletalVertexData::MakeLayout(), descs, L"경로");

	//	_mDefaultResources.push_back(shader);
	//}


	///* 블렌드 스테이트 */

	//// 일반 스테이트
	//{
	//	std::shared_ptr<DXBlendState> state = RESOURCE_MANAGER->CreateOrGetTransientRenderResource<DXBlendState>(L"World_Default", DXSharedResourceType::BlendState);

	//	D3D11_RENDER_TARGET_BLEND_DESC desc;
	//	desc.BlendEnable = true;
	//	desc.SrcBlend = D3D11_BLEND_SRC_ALPHA;
	//	desc.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	//	desc.BlendOp = D3D11_BLEND_OP_ADD;
	//	desc.SrcBlendAlpha = D3D11_BLEND_ONE;
	//	desc.DestBlendAlpha = D3D11_BLEND_ZERO;
	//	desc.BlendOpAlpha = D3D11_BLEND_OP_ADD;
	//	desc.RenderTargetWriteMask = 0x0F;

	//	state->Init(desc);

	//	_mDefaultResources.push_back(state);
	//}
}

void RenderThread::UnregisterDefaultResources()
{
	_mDefaultResources.clear();
}
