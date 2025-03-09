#pragma once

#include"base.h"
#include"vulkanWrapper/instance.h"
#include"vulkanWrapper/device.h"
#include"vulkanWrapper/window.h"
#include"vulkanWrapper/windowSurface.h"
#include"vulkanWrapper/swapchain.h"
#include"vulkanWrapper/shader.h"
#include "vulkanWrapper/pipeline.h"
#include "vulkanWrapper/renderPass.h"
#include "vulkanWrapper/commandPoll.h"
namespace FF {
	
	const int WIDTH = 800;
	const int HEIGHT = 600;
	class Application
	{
	public:
		Application() = default;
		~Application() = default;
		void run();
	private:
		void initWindow();

		void initVulkan();

		void mainLoop();

		void clearUp();

	private:
		void createPipeline();
		void createRenderPass();
	private:
		Wrapper::Window::Ptr mWindow{ nullptr };
		Wrapper::Instance::Ptr mInstance{ nullptr };
		Wrapper::Device::Ptr mDevice{ nullptr };
		Wrapper::WindowSurface::Ptr mSurface{ nullptr };
		Wrapper::SwapChain::Ptr mSwapChain{ nullptr };
		Wrapper::Pipeline::Ptr mPipeline{ nullptr };
		Wrapper::RenderPass::Ptr mRenderPass{ nullptr };
	};
}