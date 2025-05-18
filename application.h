#pragma once

#include "base.h"
#include "vulkanWrapper/instance.h"
#include "vulkanWrapper/device.h"
#include "vulkanWrapper/window.h"
#include "vulkanWrapper/windowSurface.h"
#include "vulkanWrapper/swapchain.h"
#include "vulkanWrapper/shader.h"
#include "vulkanWrapper/pipeline.h"
#include "vulkanWrapper/renderPass.h"
#include "vulkanWrapper/commandPoll.h"
#include "vulkanWrapper/commandBuffer.h"
#include "vulkanWrapper/semaphore.h"
#include "vulkanWrapper/fence.h"
#include "vulkanWrapper/buffer.h"
#include "vulkanWrapper/descriptorSetLayout.h"
#include "vulkanWrapper/descriptorPool.h"
#include "vulkanWrapper/descriptorSet.h"
#include "vulkanWrapper/description.h"  
#include "vulkanWrapper/image.h"
#include "vulkanWrapper/sampler.h"

#include "texture/texture.h"
#include "uniformManager.h"
#include "camera.h"

#include "model.h"
namespace FF {
	
	class Application
	{
	public:
		Application() = default;

		~Application() = default;

		void run();

		void onMouseMove(double xpos, double ypos);

		void onKeyDown (CAMERA_MOVE moveDire);

	private:
		void initWindow();

		void initVulkan();

		void mainLoop();

		void render();

		void clearUp();

	private:
		void createPipeline();
		void createRenderPass();
		void createCommandBufers();
		void createSyncObject();
	/*	void createUniformParams();

		void createTexture();*/

		//重建交换链： 当窗口大小发生变化的时候，交换链也要发生变化，  frame  view  pipeline  renderPass  sync
		void recreateSwapChain();
		
		void cleanUpSwapChain();
		

	private:
		unsigned int mWidth = 800;
		unsigned int mHeight = 600;
	private:
		int mCurrentFrame{ 0 };
		Wrapper::Window::Ptr mWindow{ nullptr };
		Wrapper::Instance::Ptr mInstance{ nullptr };
		Wrapper::Device::Ptr mDevice{ nullptr };
		Wrapper::WindowSurface::Ptr mSurface{ nullptr };
		Wrapper::SwapChain::Ptr mSwapChain{ nullptr };
		Wrapper::Pipeline::Ptr mPipeline{ nullptr };
		Wrapper::RenderPass::Ptr mRenderPass{ nullptr };
		Wrapper::CommandPool::Ptr mCommandPool{ nullptr };
		Wrapper::CommandBuffer::Ptr mCommandBuffer{ nullptr };

		std::vector<Wrapper::CommandBuffer::Ptr> mCommandBuffers{};
		std::vector<Wrapper::Semaphore::Ptr> mImageAvailableSemaphores{};
		std::vector<Wrapper::Semaphore::Ptr> mRenderFinishedSemaphores{};
		std::vector <Wrapper::Fence::Ptr> mFences{};

		UniformManager::Ptr mUniformManager{ nullptr };

	
		 
		Model::Ptr mModel{ nullptr };
		VPMatrix mVPMatrix;
	};
}