#pragma once
#include"../base.h"
#include"device.h"
#include"window.h"
#include"windowSurface.h"
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

	public:
		//[[nodiscard]] 
	private:
		VkSwapchainKHR mSwapChain{ VK_NULL_HANDLE };

		VkFormat mSwapChainFormat;
		VkExtent2D mSwapChainExtent;

		//VkImage��swapchanin����������ҲҪ����swapchanin
		std::vector<VkImage> mSwapChianImages{};
		//��ͼ��Ĺ�������������
		std::vector<VkImageView> mSwapChainImageViews{};

		Device::Ptr mDevice{ nullptr };
		Window::Ptr mWindow{ nullptr };
		WindowSurface::Ptr mSurface{ nullptr };

	};
}
