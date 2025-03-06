#include"swapchain.h"

namespace FF::Wrapper {
	SwapChain::SwapChain(Device::Ptr device, Window::Ptr window, WindowSurface::Ptr surface) {
		mDevice = device;
		mWindow = window;
		mSurface = surface;

		auto swapChainSupportInfo = querySwapChainSupportInfo();
	}

	SwapChain::~SwapChain() {
		if (mSwapChain != VK_NULL_HANDLE) {
			vkDestroySwapchainKHR(mDevice->getDevice(), mSwapChain, nullptr);
		}
		mWindow.reset();
		mSurface.reset();
		mDevice.reset();
	}
	SwapChainSupportInfo SwapChain::querySwapChainSupportInfo() {
		SwapChainSupportInfo info;
		//获取基础特性
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(mDevice->getPhysicalDevice(), mSurface->getSurface(), &info.mCapabilities);
		//获取表面支持格式
		uint32_t formatCount = 0;
		vkGetPhysicalDeviceSurfaceFormatsKHR(mDevice->getPhysicalDevice(), mSurface->getSurface(), &formatCount, nullptr);

		if (formatCount != 0) {
			info.mFormats.resize(formatCount);
			vkGetPhysicalDeviceSurfaceFormatsKHR(mDevice->getPhysicalDevice(), mSurface->getSurface(), &formatCount, info.mFormats.data());
		}

		//获取呈现模式
		uint32_t presentModeCount = 0;
		vkGetPhysicalDeviceSurfacePresentModesKHR(mDevice->getPhysicalDevice(), mSurface->getSurface(), &presentModeCount, nullptr);

		if (presentModeCount != 0) {
			info.mPresentModes.resize(presentModeCount);
			vkGetPhysicalDeviceSurfacePresentModesKHR(mDevice->getPhysicalDevice(), mSurface->getSurface(), &presentModeCount, info.mPresentModes.data());
		}

		return info;
	}

	VkSurfaceFormatKHR SwapChain::chooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &avalibleFormats) {
		//如果只返回了一个未定义的格式，那么就没有首选格式，我们自己定义一个
		if (avalibleFormats.size() == 1 && avalibleFormats[0].format == VK_FORMAT_UNDEFINED) {
			return { VK_FORMAT_B8G8R8A8_SRGB, VK_COLORSPACE_SRGB_NONLINEAR_KHR };
		}

		for (const auto& avalibleFormat : avalibleFormats) {
			if (avalibleFormat.format == VK_FORMAT_B8G8R8A8_SRGB && avalibleFormat.colorSpace == VK_COLORSPACE_SRGB_NONLINEAR_KHR) {
				return avalibleFormat;
			}
		}
		return avalibleFormats[0];
	}

	VkPresentModeKHR SwapChain::chooseSurfacePresentMode(std::vector<VkPresentModeKHR>& avaliblePresentModes) {
		//在设备上只有fifo这种形式是绝对支持的，如果在移动设备上，为了节省电源，优先选择fifo
		VkPresentModeKHR bestMode = VK_PRESENT_MODE_FIFO_KHR;

		for (const auto& avaliblePresentMode : avaliblePresentModes) {
			if (avaliblePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) { //高性能模式
				return avaliblePresentMode;
			}	
		}
		return bestMode;
	}

	//设置窗口的大小extent
	VkExtent2D SwapChain::chooseExtent(const VkSurfaceCapabilitiesKHR& capabilities) {
		//如果出现一下情况，说明系统不允许我们自己设定extent
		if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
			return capabilities.currentExtent;
		}
		//由于高清屏幕下，比如苹果，窗体的坐标大小，并不等于像素的长宽
		int width = 0, height = 0;
		glfwGetFramebufferSize(mWindow->getWindow(), &width, &height); // 获取到当前屏幕以像素为单位来讲的宽高
		VkExtent2D actualExtent = {
			static_cast<uint32_t>(width),
			static_cast<uint32_t>(height) 
		};
		//规定在max和min之间
		actualExtent.width = std::max(std::min(capabilities.maxImageExtent.width, actualExtent.width), capabilities.minImageExtent.width);
		actualExtent.height = std::max(std::min(capabilities.maxImageExtent.height, actualExtent.height), capabilities.minImageExtent.height);
		
		return actualExtent;
	}
}