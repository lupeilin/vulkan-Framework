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
		//��ȡ��������
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(mDevice->getPhysicalDevice(), mSurface->getSurface(), &info.mCapabilities);
		//��ȡ����֧�ָ�ʽ
		uint32_t formatCount = 0;
		vkGetPhysicalDeviceSurfaceFormatsKHR(mDevice->getPhysicalDevice(), mSurface->getSurface(), &formatCount, nullptr);

		if (formatCount != 0) {
			info.mFormats.resize(formatCount);
			vkGetPhysicalDeviceSurfaceFormatsKHR(mDevice->getPhysicalDevice(), mSurface->getSurface(), &formatCount, info.mFormats.data());
		}

		//��ȡ����ģʽ
		uint32_t presentModeCount = 0;
		vkGetPhysicalDeviceSurfacePresentModesKHR(mDevice->getPhysicalDevice(), mSurface->getSurface(), &presentModeCount, nullptr);

		if (presentModeCount != 0) {
			info.mPresentModes.resize(presentModeCount);
			vkGetPhysicalDeviceSurfacePresentModesKHR(mDevice->getPhysicalDevice(), mSurface->getSurface(), &presentModeCount, info.mPresentModes.data());
		}

		return info;
	}

	VkSurfaceFormatKHR SwapChain::chooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &avalibleFormats) {
		//���ֻ������һ��δ����ĸ�ʽ����ô��û����ѡ��ʽ�������Լ�����һ��
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
		//���豸��ֻ��fifo������ʽ�Ǿ���֧�ֵģ�������ƶ��豸�ϣ�Ϊ�˽�ʡ��Դ������ѡ��fifo
		VkPresentModeKHR bestMode = VK_PRESENT_MODE_FIFO_KHR;

		for (const auto& avaliblePresentMode : avaliblePresentModes) {
			if (avaliblePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) { //������ģʽ
				return avaliblePresentMode;
			}	
		}
		return bestMode;
	}

	//���ô��ڵĴ�Сextent
	VkExtent2D SwapChain::chooseExtent(const VkSurfaceCapabilitiesKHR& capabilities) {
		//�������һ�������˵��ϵͳ�����������Լ��趨extent
		if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
			return capabilities.currentExtent;
		}
		//���ڸ�����Ļ�£�����ƻ��������������С�������������صĳ���
		int width = 0, height = 0;
		glfwGetFramebufferSize(mWindow->getWindow(), &width, &height); // ��ȡ����ǰ��Ļ������Ϊ��λ�����Ŀ��
		VkExtent2D actualExtent = {
			static_cast<uint32_t>(width),
			static_cast<uint32_t>(height) 
		};
		//�涨��max��min֮��
		actualExtent.width = std::max(std::min(capabilities.maxImageExtent.width, actualExtent.width), capabilities.minImageExtent.width);
		actualExtent.height = std::max(std::min(capabilities.maxImageExtent.height, actualExtent.height), capabilities.minImageExtent.height);
		
		return actualExtent;
	}
}