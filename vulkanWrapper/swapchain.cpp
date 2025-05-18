#include"swapchain.h"

namespace FF::Wrapper {
	SwapChain::SwapChain(Device::Ptr& device, Window::Ptr& window, WindowSurface::Ptr& surface, CommandPool::Ptr& commandPool) {
		mDevice = device;
		mWindow = window;
		mSurface = surface;

		auto swapChainSupportInfo = querySwapChainSupportInfo();

		//选择vkformat
		VkSurfaceFormatKHR surefaceFormat = chooseSurfaceFormat(swapChainSupportInfo.mFormats);

		//选择presentMode
		VkPresentModeKHR presentMode = chooseSurfacePresentMode(swapChainSupportInfo.mPresentModes);
		
		//选择交换链范围
		VkExtent2D extent = chooseExtent(swapChainSupportInfo.mCapabilities);

		//设置图像缓冲数量
		mImageCount = swapChainSupportInfo.mCapabilities.minImageCount + 1;

		//如果maxImageCount为0，说明只要内存不爆炸，我们可以设定任意数量的images
		if (swapChainSupportInfo.mCapabilities.maxImageCount > 0 && mImageCount > swapChainSupportInfo.mCapabilities.maxImageCount) {
			mImageCount = swapChainSupportInfo.mCapabilities.maxImageCount;
		}

		//填写创建信息，此处初始化必须置空，因为会有忘记设置的变量，值为随机
		VkSwapchainCreateInfoKHR createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.surface = mSurface->getSurface();
		createInfo.minImageCount = mImageCount; // 我现在设置的数量，适合当前的情况，但可能会得到更多
		createInfo.imageFormat = surefaceFormat.format;
		createInfo.imageColorSpace = surefaceFormat.colorSpace;
		createInfo.imageExtent = extent;

		//图像包含的层次,VR一般会有两个
		createInfo.imageArrayLayers = 1;

		//交换链生成的图像，到底用于何处
		createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

		//因为交换链的图像，会被用来渲染或者显示，而渲染跟显示分别使用不同的队列，所以会出现两个队列使用同一个交换链的情况
		//那么我们就需要设置，让交换链的图像，被两个队列使用兼容
		std::vector<uint32_t> queueFamilies = { mDevice->getGraphicQueueFamily().value() , mDevice->getPresentQueueFamily().value()};
		if (mDevice->getGraphicQueueFamily().value() == mDevice->getPresentQueueFamily().value()) {
			createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE; //专有模式，被某一个队列簇独占，性能会更好
			createInfo.queueFamilyIndexCount = 0;
			createInfo.pQueueFamilyIndices = nullptr;
		}
		else {
			createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT; //可以被共享的模式
			createInfo.queueFamilyIndexCount = static_cast<uint32_t>(queueFamilies.size());
			createInfo.pQueueFamilyIndices = queueFamilies.data();
		}

		//交换链的图像初始变化，比如是否需要反转
		createInfo.preTransform = swapChainSupportInfo.mCapabilities.currentTransform;

		//不予原来窗体当中的内容混合
		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

		createInfo.presentMode = presentMode;

		//当前窗体被挡住的部分不用绘制,但是会影响到回读
		createInfo.clipped = VK_TRUE;

		if (vkCreateSwapchainKHR(mDevice->getDevice(), &createInfo, nullptr, &mSwapChain) != VK_SUCCESS) {
			throw std::runtime_error("Error : failed to reate swap chain");
		}

		mSwapChainFormat = surefaceFormat.format;
		mSwapChainExtent = extent;

		//系统可能创建更多的image ，当前的imageCount是最小数量
		vkGetSwapchainImagesKHR(mDevice->getDevice(), mSwapChain, &mImageCount, nullptr);
		mSwapChianImages.resize(mImageCount);
		vkGetSwapchainImagesKHR(mDevice->getDevice(), mSwapChain, &mImageCount, mSwapChianImages.data());

		//创建imageView
		mSwapChainImageViews.resize(mImageCount);
		for (uint32_t i = 0; i < mImageCount; ++i) {
			mSwapChainImageViews[i] = createImageView(mSwapChianImages[i], mSwapChainFormat, VK_IMAGE_ASPECT_COLOR_BIT, 1);
		}

		//创建depthImage
		mDepthImages.resize(mImageCount);
		
		VkImageSubresourceRange range{};
		range.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
		range.baseArrayLayer = 0;
		range.layerCount = 1;
		range.baseMipLevel = 0;
		range.levelCount = 1;
		
		for (int i = 0; i < mImageCount; i++)
		{
			//创建深度图像
			mDepthImages[i] = Image::createDepthImage(mDevice, 
				mSwapChainExtent.width, 
				mSwapChainExtent.height,
				mDevice->getMaxUsableSampleCount()
			); //深度图像的宽高和交换链的宽高一致
			//设置深度图像的格式
			mDepthImages[i]->setImageLayout(
				VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
				VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
				VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
				range,
				commandPool //命令池
				);
		}

		//创建多重采样的图像MultiSampleImages
		mMutiSampleImages.resize(mImageCount);

		VkImageSubresourceRange rangeMutiSample{};
		rangeMutiSample.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		rangeMutiSample.baseArrayLayer = 0;
		rangeMutiSample.layerCount = 1;
		rangeMutiSample.baseMipLevel = 0;
		rangeMutiSample.levelCount = 1;

		for(int i = 0; i < mImageCount; i++)
		{
			//创建多重采样的图像
			mMutiSampleImages[i] = Image::createRenderTargetImage(mDevice, mSwapChainExtent.width, mSwapChainExtent.height, mSwapChainFormat);
			//格式转换
			mMutiSampleImages[i]->setImageLayout(
				VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
				VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
				VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
				rangeMutiSample,
				commandPool //命令池
			);
		}
		
	}
	void SwapChain::createFrameBuffers(RenderPass::Ptr& renderPass) {
		//创建Framebuffer
		mSwapChainFrameBuffers.resize(mImageCount);
		for (uint32_t i = 0; i < mImageCount; i++) {
			//frameBuffer  里面有一帧的数据，比如有n个不同的ColorAttachment 一个DepthStencilAttachment， 
			//这些东西的集合为一个frameBuffer，送入管线，就会线程一个gpu的集合，这个集合由上方的attachment构成

			// 这里 mSwapChainImageViews[i] 的顺序要和 创建renderPass的时候的VkAttachmentDescription的顺序保持一致
			std::array<VkImageView, 3> attachments = { 
				mSwapChainImageViews[i],
				mMutiSampleImages[i]->getImageView(), //多重采样的图片
				mDepthImages[i]->getImageView(),
			}; 

			VkFramebufferCreateInfo frameBufferCreateInfo{};
			frameBufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			frameBufferCreateInfo.renderPass = renderPass->getRenderPass();
			frameBufferCreateInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
			frameBufferCreateInfo.pAttachments = attachments.data();
			frameBufferCreateInfo.width = mSwapChainExtent.width;
			frameBufferCreateInfo.height = mSwapChainExtent.height;
			frameBufferCreateInfo.layers = 1;

			if (vkCreateFramebuffer(mDevice->getDevice(), &frameBufferCreateInfo, nullptr, &mSwapChainFrameBuffers[i]) != VK_SUCCESS) {
				throw std::runtime_error("Error : failed to create frame buffer");
			}
		}
	}
	SwapChain::~SwapChain() {
		for (auto& imageView : mSwapChainImageViews) {
			vkDestroyImageView(mDevice->getDevice(), imageView, nullptr);
		}

		for (auto& frameBuffer : mSwapChainFrameBuffers) {
			vkDestroyFramebuffer(mDevice->getDevice(), frameBuffer, nullptr);
		}

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

	VkImageView SwapChain::createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels) {
		VkImageViewCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		createInfo.image = image;
		createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		createInfo.format = format;
		 
		createInfo.subresourceRange.aspectMask = aspectFlags;
		createInfo.subresourceRange.baseMipLevel = 0;
		createInfo.subresourceRange.levelCount = mipLevels;
		createInfo.subresourceRange.baseArrayLayer = 0;
		createInfo.subresourceRange.layerCount = 1;

		VkImageView imageView{ VK_NULL_HANDLE };
		if (vkCreateImageView(mDevice->getDevice(), &createInfo, nullptr, &imageView) != VK_SUCCESS) {
			throw std::runtime_error("Error: failed to create image view in swapchain");
		}

		return imageView;
	}
}