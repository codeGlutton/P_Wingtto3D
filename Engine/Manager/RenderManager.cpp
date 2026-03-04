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
#include "Graphics/Buffer/DXConstantBuffer.h"
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
	_mWidgetMesh.reset();
	_mWidgetCBuffer.reset();
	_mCameraCBuffer.reset();
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
		DXTextureBase* currentTexture = reinterpret_cast<DXTextureBase*>(&container);
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
						WidgetRenderBatch(box.mResource, false, static_cast<uint32>(container->mIndices.size()))
					);
				}

				WidgetRenderBatch& batch = container->mBatches.back();
				const std::size_t preVBSize = container->mVertices.size();
				batch.mIndexCount += 6;

				// 배처에 추가적인 정점과 인덱스 데이터 덧불이기
				PushQuad(
					container->mVertices,
					container->mIndices,
					box.mMargin.mLeft * box.mBoxSize.x,
					(1.f - box.mMargin.mRight) * box.mBoxSize.x,
					box.mMargin.mUp * box.mBoxSize.y,
					(1.f - box.mMargin.mDown) * box.mBoxSize.y,
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
				WidgetRenderTextElement& text = *static_cast<WidgetRenderTextElement*>(element.get());

				// 자신에 맞는 배처 탐색
				if (text.mResource.get() != currentTexture)
				{
					// 새로운 배처 데이터 생성
					currentTexture = text.mResource.get();
					container->mBatches.push_back(
						WidgetRenderBatch(text.mResource, false, static_cast<uint32>(container->mIndices.size()))
					);
				}

				WidgetRenderBatch& batch = container->mBatches.back();
				const FontAtlasData& fontAtlas = text.mAtlasData.GetAtlasData();

				for (const WidgetCharRenderCache& cache : text.mRenderCaches)
				{
					const std::size_t preVBSize = container->mVertices.size();
					batch.mIndexCount += 6;

					const GlyphData& glyph = fontAtlas.mGlyphs.at(cache.mChar);

					// 배처에 추가적인 정점과 인덱스 데이터 덧불이기
					PushQuad(
						container->mVertices,
						container->mIndices,
						cache.mScaleOffset.x,
						cache.mScaleSize.x + cache.mScaleOffset.x,
						cache.mScaleOffset.y,
						cache.mScaleSize.y + cache.mScaleOffset.y,
						glyph.mUVStart,
						glyph.mUVSize,
						text.mTint
					);
					for (std::size_t i = 0; i < 4; ++i)
					{
						UIVertexData& vertex = container->mVertices[preVBSize + i];
						vertex.mPosition = Vec3::Transform(vertex.mPosition, text.mRenderMat);
					}
				}
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
	
	if (_mWidgetMesh == nullptr)
	{
		_mWidgetMesh = std::make_shared<DXWidgetBatchMesh>();
		_mWidgetMesh->Init();
	}
	if (_mWidgetCBuffer == nullptr)
	{
		_mWidgetCBuffer = std::make_shared<DXConstantBufferTemplate<WidgetConstantData>>();
		_mWidgetCBuffer->Init();
	}
	if (_mScreenCBuffer == nullptr)
	{
		_mScreenCBuffer = std::make_shared<DXConstantBufferTemplate<ScreenConstantData>>();
		_mScreenCBuffer->Init();
	}
	if (_mCameraCBuffer == nullptr)
	{
		_mCameraCBuffer = std::make_shared<DXConstantBufferTemplate<CameraConstantData>>();
		_mCameraCBuffer->Init();
	}

	static const std::string textureBindName = "widgetResourceTB";
	static const std::string widgetCBufferBindName = "M_WidgetTypeCB";
	static const std::string screenCBufferBindName = "F_ScreenCB";
	const std::wstring matPath = PATH_MANAGER->GetEngineResourceFolderName() + L"\\M_UI";

	std::shared_ptr<DXMaterial> matUI = RESOURCE_MANAGER->GetRenderResource<DXMaterial>(matPath, DXSharedResourceType::Material);
	{
		drawInputs.mSwapChain->BeginRender(drawInputs.mViewport);
		
		// 동적으로 데이터 Mapping
		_mWidgetMesh->UpdateData(drawInputs.mContainer->mVertices, drawInputs.mContainer->mIndices);
		_mScreenCBuffer->UpdateData(ScreenConstantData(drawInputs.mContainer->mWindowClientSize));

		_mWidgetMesh->PushData();														// 배칭 메시 넣고
		matUI->PushData();																// 머테리얼 넣고
		matUI->PushTransientData(widgetCBufferBindName, _mWidgetCBuffer);
		matUI->PushTransientData(screenCBufferBindName, _mScreenCBuffer);

		const std::vector<WidgetRenderBatch>& batches = drawInputs.mContainer->mBatches;
		WidgetConstantData preCBuffer(false);
		if (batches.empty() == false)
		{
			preCBuffer.SetIsText(!(batches[0].mIsText));
		}

		for (const WidgetRenderBatch& batch : batches)
		{
			if (preCBuffer.IsText() != batch.mIsText)
			{
				preCBuffer.SetIsText(batch.mIsText);
				_mWidgetCBuffer->UpdateData(preCBuffer);
			}
			if (batch.mTexture == nullptr)
			{
				std::shared_ptr<DXTextureBase> defaultWhiteTexture = RESOURCE_MANAGER->CreateOrGetRuntimeRenderResource<DXConstTexture2D>(L"White", DXSharedResourceType::Texture);
				matUI->PushTransientData(textureBindName, defaultWhiteTexture);
			}
			else
			{
				matUI->PushTransientData(textureBindName, batch.mTexture);
			}
			DX_DEVICE_CONTEXT->DrawIndexed(batch.mIndexCount, batch.mStartIndex, 0);	// 인덱스로 드로잉
		}

		drawInputs.mSwapChain->EndRender();
	}
}

