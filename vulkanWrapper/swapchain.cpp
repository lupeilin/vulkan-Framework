#include"swapchain.h"

namespace FF::Wrapper {
	SwapChain::SwapChain(Device::Ptr& device, Window::Ptr& window, WindowSurface::Ptr& surface, CommandPool::Ptr& commandPool) {
		mDevice = device;
		mWindow = window;
		mSurface = surface;

		auto swapChainSupportInfo = querySwapChainSupportInfo();

		//ѡ��vkformat
		VkSurfaceFormatKHR surefaceFormat = chooseSurfaceFormat(swapChainSupportInfo.mFormats);

		//ѡ��presentMode
		VkPresentModeKHR presentMode = chooseSurfacePresentMode(swapChainSupportInfo.mPresentModes);
		
		//ѡ�񽻻�����Χ
		VkExtent2D extent = chooseExtent(swapChainSupportInfo.mCapabilities);

		//����ͼ�񻺳�����
		mImageCount = swapChainSupportInfo.mCapabilities.minImageCount + 1;

		//���maxImageCountΪ0��˵��ֻҪ�ڴ治��ը�����ǿ����趨����������images
		if (swapChainSupportInfo.mCapabilities.maxImageCount > 0 && mImageCount > swapChainSupportInfo.mCapabilities.maxImageCount) {
			mImageCount = swapChainSupportInfo.mCapabilities.maxImageCount;
		}

		//��д������Ϣ���˴���ʼ�������ÿգ���Ϊ�����������õı�����ֵΪ���
		VkSwapchainCreateInfoKHR createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.surface = mSurface->getSurface();
		createInfo.minImageCount = mImageCount; // ���������õ��������ʺϵ�ǰ������������ܻ�õ�����
		createInfo.imageFormat = surefaceFormat.format;
		createInfo.imageColorSpace = surefaceFormat.colorSpace;
		createInfo.imageExtent = extent;

		//ͼ������Ĳ��,VRһ���������
		createInfo.imageArrayLayers = 1;

		//���������ɵ�ͼ�񣬵������ںδ�
		createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

		//��Ϊ��������ͼ�񣬻ᱻ������Ⱦ������ʾ������Ⱦ����ʾ�ֱ�ʹ�ò�ͬ�Ķ��У����Ի������������ʹ��ͬһ�������������
		//��ô���Ǿ���Ҫ���ã��ý�������ͼ�񣬱���������ʹ�ü���
		std::vector<uint32_t> queueFamilies = { mDevice->getGraphicQueueFamily().value() , mDevice->getPresentQueueFamily().value()};
		if (mDevice->getGraphicQueueFamily().value() == mDevice->getPresentQueueFamily().value()) {
			createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE; //ר��ģʽ����ĳһ�����дض�ռ�����ܻ����
			createInfo.queueFamilyIndexCount = 0;
			createInfo.pQueueFamilyIndices = nullptr;
		}
		else {
			createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT; //���Ա������ģʽ
			createInfo.queueFamilyIndexCount = static_cast<uint32_t>(queueFamilies.size());
			createInfo.pQueueFamilyIndices = queueFamilies.data();
		}

		//��������ͼ���ʼ�仯�������Ƿ���Ҫ��ת
		createInfo.preTransform = swapChainSupportInfo.mCapabilities.currentTransform;

		//����ԭ�����嵱�е����ݻ��
		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

		createInfo.presentMode = presentMode;

		//��ǰ���屻��ס�Ĳ��ֲ��û���,���ǻ�Ӱ�쵽�ض�
		createInfo.clipped = VK_TRUE;

		if (vkCreateSwapchainKHR(mDevice->getDevice(), &createInfo, nullptr, &mSwapChain) != VK_SUCCESS) {
			throw std::runtime_error("Error : failed to reate swap chain");
		}

		mSwapChainFormat = surefaceFormat.format;
		mSwapChainExtent = extent;

		//ϵͳ���ܴ��������image ����ǰ��imageCount����С����
		vkGetSwapchainImagesKHR(mDevice->getDevice(), mSwapChain, &mImageCount, nullptr);
		mSwapChianImages.resize(mImageCount);
		vkGetSwapchainImagesKHR(mDevice->getDevice(), mSwapChain, &mImageCount, mSwapChianImages.data());

		//����imageView
		mSwapChainImageViews.resize(mImageCount);
		for (uint32_t i = 0; i < mImageCount; ++i) {
			mSwapChainImageViews[i] = createImageView(mSwapChianImages[i], mSwapChainFormat, VK_IMAGE_ASPECT_COLOR_BIT, 1);
		}

		//����depthImage
		mDepthImages.resize(mImageCount);
		
		VkImageSubresourceRange range{};
		range.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
		range.baseArrayLayer = 0;
		range.layerCount = 1;
		range.baseMipLevel = 0;
		range.levelCount = 1;
		
		for (int i = 0; i < mImageCount; i++)
		{
			//�������ͼ��
			mDepthImages[i] = Image::createDepthImage(mDevice, 
				mSwapChainExtent.width, 
				mSwapChainExtent.height,
				mDevice->getMaxUsableSampleCount()
			); //���ͼ��Ŀ�ߺͽ������Ŀ��һ��
			//�������ͼ��ĸ�ʽ
			mDepthImages[i]->setImageLayout(
				VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
				VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
				VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
				range,
				commandPool //�����
				);
		}

		//�������ز�����ͼ��MultiSampleImages
		mMutiSampleImages.resize(mImageCount);

		VkImageSubresourceRange rangeMutiSample{};
		rangeMutiSample.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		rangeMutiSample.baseArrayLayer = 0;
		rangeMutiSample.layerCount = 1;
		rangeMutiSample.baseMipLevel = 0;
		rangeMutiSample.levelCount = 1;

		for(int i = 0; i < mImageCount; i++)
		{
			//�������ز�����ͼ��
			mMutiSampleImages[i] = Image::createRenderTargetImage(mDevice, mSwapChainExtent.width, mSwapChainExtent.height, mSwapChainFormat);
			//��ʽת��
			mMutiSampleImages[i]->setImageLayout(
				VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
				VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
				VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
				rangeMutiSample,
				commandPool //�����
			);
		}
		
	}
	void SwapChain::createFrameBuffers(RenderPass::Ptr& renderPass) {
		//����Framebuffer
		mSwapChainFrameBuffers.resize(mImageCount);
		for (uint32_t i = 0; i < mImageCount; i++) {
			//frameBuffer  ������һ֡�����ݣ�������n����ͬ��ColorAttachment һ��DepthStencilAttachment�� 
			//��Щ�����ļ���Ϊһ��frameBuffer��������ߣ��ͻ��߳�һ��gpu�ļ��ϣ�����������Ϸ���attachment����

			// ���� mSwapChainImageViews[i] ��˳��Ҫ�� ����renderPass��ʱ���VkAttachmentDescription��˳�򱣳�һ��
			std::array<VkImageView, 3> attachments = { 
				mSwapChainImageViews[i],
				mMutiSampleImages[i]->getImageView(), //���ز�����ͼƬ
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