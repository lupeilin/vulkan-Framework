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

	//����һ��mutisample��ͼƬ�����Ҽӵ�framebuffer����


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

		//VkImage��swapchanin����������ҲҪ����swapchanin
		std::vector<VkImage> mSwapChianImages{};

		//��ͼ��Ĺ�������������
		std::vector<VkImageView> mSwapChainImageViews{};

		//������Ⱦ���߽�����Ⱦ��ʱ����Ҫ�� ImageViews ��װ��һ�� FrameBuffers
		std::vector<VkFramebuffer> mSwapChainFrameBuffers{};

		//���ͼƬ
		std::vector<Image::Ptr> mDepthImages{};

		//���ز������м�ͼƬ
		std::vector<Image::Ptr> mMutiSampleImages{};

		Device::Ptr mDevice{ nullptr };
		Window::Ptr mWindow{ nullptr };
		WindowSurface::Ptr mSurface{ nullptr };

	};
}
