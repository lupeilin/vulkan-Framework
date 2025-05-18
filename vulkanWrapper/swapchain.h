#pragma once
#include"../base.h"
#include"device.h"
#include"window.h"
#include"windowSurface.h"
#include "renderPass.h"
#include "image.h"
#include "commandPoll.h"
namespace FF::Wrapper {

	struct SwapChainSupportInfo
	{
		VkSurfaceCapabilitiesKHR mCapabilities;
		std::vector<VkSurfaceFormatKHR> mFormats;
		std::vector<VkPresentModeKHR> mPresentModes;
	};

	//创建一张mutisample的图片，并且加到framebuffer里面


	class SwapChain
	{
	public:
		using Ptr = std::shared_ptr<SwapChain>;
		static Ptr create(Device::Ptr& device, Window::Ptr& window, WindowSurface::Ptr& surface, CommandPool::Ptr& commandPool) {
			return std::make_shared<SwapChain>(device, window, surface, commandPool);
		}

		SwapChain(
			Device::Ptr& device,
			Window::Ptr& window, 
			WindowSurface::Ptr& surface,
			CommandPool::Ptr& commandPool
			);
		~SwapChain();

		SwapChainSupportInfo querySwapChainSupportInfo();

		VkSurfaceFormatKHR chooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &avalibleFormats);
		VkPresentModeKHR chooseSurfacePresentMode(std::vector<VkPresentModeKHR>& avaliblePresentModes);
		VkExtent2D chooseExtent(const VkSurfaceCapabilitiesKHR& capabilities);

		void createFrameBuffers(RenderPass::Ptr &renderPass);

	public:
		[[nodiscard]] auto getFormat() const { return mSwapChainFormat; }

		[[nodiscard]] auto getImageCount() const { return mImageCount; }

		[[nodiscard]] auto getSwapchain() const { return mSwapChain; }

		[[nodiscard]] auto getFrameBuffer(const int index) const { return mSwapChainFrameBuffers[index]; }

		[[nodiscard]] auto getExtent() const { return mSwapChainExtent; }

	private:
		VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels = 1);
	private:
		VkSwapchainKHR mSwapChain{ VK_NULL_HANDLE };

		VkFormat mSwapChainFormat;
		VkExtent2D mSwapChainExtent;
		uint32_t mImageCount{ 0 };

		//VkImage由swapchanin创建，销毁也要交给swapchanin
		std::vector<VkImage> mSwapChianImages{};

		//对图像的管理器、管理框架
		std::vector<VkImageView> mSwapChainImageViews{};

		//交给渲染管线进行渲染的时候，需要把 ImageViews 包装成一个 FrameBuffers
		std::vector<VkFramebuffer> mSwapChainFrameBuffers{};

		//深度图片
		std::vector<Image::Ptr> mDepthImages{};

		//多重采样，中间图片
		std::vector<Image::Ptr> mMutiSampleImages{};

		Device::Ptr mDevice{ nullptr };
		Window::Ptr mWindow{ nullptr };
		WindowSurface::Ptr mSurface{ nullptr };

	};
}
