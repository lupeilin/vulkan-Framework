#pragma once
#include"../base.h"
#include"device.h"
#include"window.h"
#include"windowSurface.h"
#include "renderPass.h"
namespace FF::Wrapper {

	struct SwapChainSupportInfo
	{
		VkSurfaceCapabilitiesKHR mCapabilities;
		std::vector<VkSurfaceFormatKHR> mFormats;
		std::vector<VkPresentModeKHR> mPresentModes;
	};

	class SwapChain
	{
	public:
		using Ptr = std::shared_ptr<SwapChain>;
		static Ptr create(Device::Ptr device, Window::Ptr window, WindowSurface::Ptr surface) { return std::make_shared<SwapChain>(device, window, surface); }

		SwapChain(Device::Ptr device, Window::Ptr window, WindowSurface::Ptr surface);
		~SwapChain();

		SwapChainSupportInfo querySwapChainSupportInfo();

		VkSurfaceFormatKHR chooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &avalibleFormats);
		VkPresentModeKHR chooseSurfacePresentMode(std::vector<VkPresentModeKHR>& avaliblePresentModes);
		VkExtent2D chooseExtent(const VkSurfaceCapabilitiesKHR& capabilities);

		void createFrameBuffers(RenderPass::Ptr &renderPass);

	public:
		[[nodiscard]] auto getFormat() const { return mSwapChainFormat; }

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

		Device::Ptr mDevice{ nullptr };
		Window::Ptr mWindow{ nullptr };
		WindowSurface::Ptr mSurface{ nullptr };

	};
}
