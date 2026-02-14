#include "pch.h"
#include "RenderManager.h"

#include "Manager/ThreadManager.h"
#include "Manager/TimeManager.h"
#include "Manager/ResourceManager.h"
#include "Manager/PathManager.h"

#include "Core/App/AppWindow/AppWindow.h"

#include "Utils/Thread/Job.h"
#include "Utils/Memory/ObjectPool.h"

#include "Graphics/DXSwapChain.h"
#include "Graphics/Viewport/DXViewport.h"
#include "Graphics/Resource/DXTexture.h"
#include "Graphics/Resource/DXMaterial.h"
#include "Graphics/Render/DXWidgetBatchMesh.h"

#include "Utils/GeometryUtils.h"

RenderManager::RenderManager()
{
}

RenderManager::~RenderManager()
{
}

void RenderManager::Init()
{
}

void RenderManager::Destroy()
{
	mWidgetMesh.reset();
}

void RenderManager::DrawWindow(std::shared_ptr<WindowRenderElementContainer> container)
{
	ASSERT_THREAD(MainThreadType::Game);

	// 큐에 전달되는 동안 윈도우 weak_ptr이 제거될 수 있기 때문에
	// 유용한 데이터 복사
	std::shared_ptr<WindowDrawInputs> windowDrawInputs = std::make_shared<WindowDrawInputs>();
	windowDrawInputs->mContainer = container;
	windowDrawInputs->mGameThreadDeltaTime = DELTA_TIME;
	windowDrawInputs->mGameThreadTime = PLAY_TIME;

	std::shared_ptr<AppWindow> window = container->mWindow.lock();
	windowDrawInputs->mSwapChain = window->GetSwapChainProxy()->mData;
	windowDrawInputs->mViewport = window->GetViewportProxy()->mData;
	windowDrawInputs->mWindowTitle = window->GetDesc().mName;
	windowDrawInputs->mIsVsync = window->GetDesc().mVsync;

	// 메테리얼이 같은 순으로 묶어서 정적 배칭 시켜주기
	container->mVertices.reserve(4 * container->mElementCount);
	container->mIndices.reserve(6 * container->mElementCount);

	for (const auto& layer : container->mElementLayers)
	{
		DXTextureBase* currentTexture = nullptr;
		for (const std::unique_ptr<WidgetRenderElement>& element : layer)
		{
			switch (element->mType)
			{
			case WidgetRenderElementType::Box:
			{
				WidgetRenderBoxElement& box = *static_cast<WidgetRenderBoxElement*>(element.get());

				// 자신에 맞는 배처 탐색
				if (box.mResource.get() != currentTexture)
				{
					// 새로운 배처 데이터 생성
					currentTexture = box.mResource.get();
					container->mBatches.push_back(
						WidgetRenderBatch(box.mResource, static_cast<uint32>(container->mIndices.size()))
					);
				}

				WidgetRenderBatch& batch = container->mBatches.back();
				const std::size_t preVBSize = container->mVertices.size();
				batch.mIndexCount += 6;

				// 배처에 추가적인 정점과 인덱스 데이터 덧불이기
				PushQuad(
					container->mVertices,
					container->mIndices,
					(-0.5f + box.mMargin.mLeft) * box.mBoxSize.x,
					(0.5f - box.mMargin.mRight) * box.mBoxSize.x,
					(-0.5f + box.mMargin.mUp) * box.mBoxSize.y,
					(0.5f - box.mMargin.mDown) * box.mBoxSize.y,
					box.mTint
				);
				for (std::size_t i = 0; i < 4; ++i)
				{
					UIVertexData& vertex = container->mVertices[preVBSize + i];
					vertex.mPosition = Vec3::Transform(vertex.mPosition, box.mRenderMat);
				}
				break;
			}
			case WidgetRenderElementType::Line:
			{
				// TODO
				break;
			}
			case WidgetRenderElementType::Text:
			{
				// TODO
				break;
			}
			}
		}
	}

	THREAD_MANAGER->PushRenderThreadRenderJob(ObjectPool<Job>::MakeShared([winDrawInputs = std::move(windowDrawInputs)]() {
		RENDER_MANAGER->DrawWindow_Internal(*winDrawInputs.get());
		}));
}

void RenderManager::DrawWindow_Internal(const WindowDrawInputs& drawInputs)
{
	ASSERT_THREAD(MainThreadType::Render);
	
	if (mWidgetMesh == nullptr)
	{
		mWidgetMesh = std::make_shared<DXWidgetBatchMesh>();
		mWidgetMesh->Init();
	}

	static const std::string textureBindName = "widgetResourceTB";
	const std::wstring matPath = PATH_MANAGER->GetEngineResourceFolderName() + L"\\M_UI";;
	std::shared_ptr<DXMaterial> matUI = RESOURCE_MANAGER->GetRenderResource<DXMaterial>(matPath, DXSharedResourceType::Material);
	{
		drawInputs.mSwapChain->BeginRender(drawInputs.mViewport);

		// 동적으로 데이터 Mapping
		mWidgetMesh->UpdateData(drawInputs.mContainer->mVertices, drawInputs.mContainer->mIndices);

		mWidgetMesh->PushData();														// 배칭 메시 넣고
		matUI->PushData();																// 머테리얼 넣고

		const std::vector<WidgetRenderBatch>& batches = drawInputs.mContainer->mBatches;
		for (const WidgetRenderBatch& batch : batches)
		{
			matUI->PushTransientData(textureBindName, batch.mTexture);					// 텍스처 SRV 넣고
			DX_DEVICE_CONTEXT->DrawIndexed(batch.mIndexCount, batch.mStartIndex, 0);	// 인덱스로 드로잉
		}

		drawInputs.mSwapChain->EndRender();
	}
}

