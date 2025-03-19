#include"application.h"

namespace FF {

	void Application::run() {
		initWindow();
		initVulkan();
		mainLoop();
		clearUp();
	}

	void Application::initWindow() {
		mWindow = Wrapper::Window::create(WIDTH, HEIGHT); 
	}

	void Application::initVulkan() {
		mInstance = Wrapper::Instance::create(true);
		mSurface = Wrapper::WindowSurface::create(mInstance, mWindow);
		mDevice = Wrapper::Device::create(mInstance, mSurface);
		mSwapChain = Wrapper::SwapChain::create(mDevice, mWindow, mSurface);

		mRenderPass = Wrapper::RenderPass::create(mDevice);
		createRenderPass();

		mSwapChain->createFrameBuffers(mRenderPass);

		mPipeline = Wrapper::Pipeline::create(mDevice, mRenderPass);
		
		createPipeline();

		mCommandPool = Wrapper::CommandPool::create(mDevice);

		mCommandBuffers.resize(mSwapChain->getImageCount());

		for (int i = 0; i < mSwapChain->getImageCount(); ++i) {
			mCommandBuffers[i] = Wrapper::CommandBuffer::create(mDevice, mCommandPool);

			mCommandBuffers[i]->begin();

			VkRenderPassBeginInfo renderPassBeginInfo{};
			renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			renderPassBeginInfo.renderPass = mRenderPass->getRenderPass();
			renderPassBeginInfo.framebuffer = mSwapChain->getFrameBuffer(i);
			renderPassBeginInfo.renderArea.offset = { 0, 0 };
			renderPassBeginInfo.renderArea.extent = mSwapChain->getExtent();
			 
			VkClearValue  clearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
			renderPassBeginInfo.clearValueCount = 1;
			renderPassBeginInfo.pClearValues = &clearColor;

			mCommandBuffers[i]->beginRenderPass(renderPassBeginInfo);

			mCommandBuffers[i]->bindGraphicPipeline(mPipeline->getPipeline());
		
			mCommandBuffers[i]->draw(3);
			
			mCommandBuffers[i]->endRenderPass();

			mCommandBuffers[i]->end();
		}
		for (int i = 0; i < mSwapChain->getImageCount(); ++i) {
			auto imageSemaphore = Wrapper::Semaphore::create(mDevice);
			mImageAvailableSemaphores.push_back(imageSemaphore);

			auto renderSemaphore = Wrapper::Semaphore::create(mDevice);
			mRenderFinishedSemaphores.push_back(renderSemaphore);

			//����ÿһ��������һ��fence
			auto fence = Wrapper::Fence::create(mDevice);
			mFences.push_back(fence);
		}

	}
	void Application::createPipeline() {
		//�����ӿ�
		VkViewport viewport = {};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = (float)WIDTH;
		viewport.height = (float)HEIGHT;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0;

		VkRect2D scissor = {};
		scissor.offset = { 0, 0 };
		scissor.extent = { WIDTH, HEIGHT };

		mPipeline->setViewPorts({ viewport });
		mPipeline->setScissors({ scissor });

		//����shader
		std::vector<Wrapper::Shader::Ptr> shaderGroup{};

		auto shaderVertex = Wrapper::Shader::create(mDevice, "shaders/vs.spv", VK_SHADER_STAGE_VERTEX_BIT, "main");
		shaderGroup.push_back(shaderVertex);

		auto shaderFragment = Wrapper::Shader::create(mDevice, "shaders/fs.spv", VK_SHADER_STAGE_FRAGMENT_BIT, "main");
		shaderGroup.push_back(shaderFragment);

		mPipeline->setShaderGroup(shaderGroup);
		
		//������Ų�ģʽ
		mPipeline->mVertexInputStage.vertexBindingDescriptionCount = 0;
		mPipeline->mVertexInputStage.pVertexBindingDescriptions = nullptr;
		mPipeline->mVertexInputStage.vertexAttributeDescriptionCount = 0;
		mPipeline->mVertexInputStage.pVertexAttributeDescriptions = nullptr;

		//ͼԪװ��
		mPipeline->mAssemblyState.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		mPipeline->mAssemblyState.primitiveRestartEnable = VK_FALSE; //�������true�� ��0xff ���¿�ʼ�������Σ�����ǰ���������������

		//��դ������
		mPipeline->mRasterstate.polygonMode = VK_POLYGON_MODE_FILL; //��դ����ʱ����ν�����һ��ͼԪ����fill����line  // ����ģʽ��Ҫ����gpu���� //����λ���ģʽ����䡢�߿򡢵㣩
		mPipeline->mRasterstate.lineWidth = 1.0f;//����1��Ҫ����gpu����
		mPipeline->mRasterstate.cullMode = VK_CULL_MODE_BACK_BIT; //�����޳�
		mPipeline->mRasterstate.frontFace = VK_FRONT_FACE_CLOCKWISE; // ˳ʱ��Ϊ����

		mPipeline->mRasterstate.depthBiasEnable = VK_FALSE;//�ڶ�����Ⱦ����ȣ�����ʵ�����Сһ�㣬��ԭ�������������һ����Ҷ˹�� 
		mPipeline->mRasterstate.depthBiasConstantFactor = 0.0f;
		mPipeline->mRasterstate.depthBiasClamp = 0.0f;
		mPipeline->mRasterstate.depthBiasSlopeFactor = 0.0f;

		//TODO:���ز���
		mPipeline->mSampleState.sampleShadingEnable = VK_FALSE;
		mPipeline->mSampleState.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		mPipeline->mSampleState.minSampleShading = 1.0; 
		mPipeline->mSampleState.pSampleMask = nullptr;
		mPipeline->mSampleState.alphaToCoverageEnable = VK_FALSE;
		mPipeline->mSampleState.alphaToOneEnable = VK_FALSE;

		//TODO:�����ģ�����

		//��ɫ���
		//�������ɫ������룬�õ��Ļ�Ͻ��������ͨ�����������and���������
		VkPipelineColorBlendAttachmentState blendAttachment{};
		blendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT |
			VK_COLOR_COMPONENT_G_BIT |
			VK_COLOR_COMPONENT_B_BIT |
			VK_COLOR_COMPONENT_A_BIT;
		blendAttachment.blendEnable = VK_FALSE;
		blendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
		blendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
		blendAttachment.colorBlendOp = VK_BLEND_OP_ADD;

		blendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
		blendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
		blendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

		mPipeline->pushBlendAttachment(blendAttachment);

		//1��blend�����ּ���ķ�ʽ����һ��������ʾ������alphaΪ�����ļ��㣬�ڶ��ֽ���λ���㡣
		//2�����������logicop����ô�Ϸ����õ�alphaΪ���������㣬ʧ��
		//3��colorWrite���룬��Ȼ��Ч�����㿪����logicOp
		//4�� ��Ϊ���ǿ��ܻ��ж��franmebuffer��������Կ�����Ҫ���blendattachment
		mPipeline->mBlendState.logicOpEnable = VK_FALSE;
		mPipeline->mBlendState.logicOp = VK_LOGIC_OP_COPY;

		//���blend Attachment��Factor��operation
		mPipeline->mBlendState.blendConstants[0] = 0.0f;
		mPipeline->mBlendState.blendConstants[1] = 0.0f;
		mPipeline->mBlendState.blendConstants[2] = 0.0f;
		mPipeline->mBlendState.blendConstants[3] = 0.0f;

		//uniform�Ĵ���
		mPipeline->mLayoutState.setLayoutCount = 0;
		mPipeline->mLayoutState.pSetLayouts = nullptr;
		mPipeline->mLayoutState.pushConstantRangeCount = 0;
		mPipeline->mLayoutState.pPushConstantRanges = nullptr;

		mPipeline->build();
	}

	void Application::createRenderPass() {
		//���뻭��������
		VkAttachmentDescription attachmentDes{};
		attachmentDes.format = mSwapChain->getFormat();
		attachmentDes.samples = VK_SAMPLE_COUNT_1_BIT;
		attachmentDes.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;//����ͼƬ������ʱ�򣬰ѻ�������renderpass ��������ԭ�������ݱ������߲�����
		attachmentDes.storeOp = VK_ATTACHMENT_STORE_OP_STORE;//���
		attachmentDes.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		attachmentDes.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		attachmentDes.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		attachmentDes.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		mRenderPass->addAttchment(attachmentDes);

		//���ڻ������������ã��Լ���ʽҪ��
		VkAttachmentReference attachmentRef{};
		attachmentRef.attachment = 0;
		attachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		
		//����������
		Wrapper::SubPass subPass{};
		subPass.addColorAttachmentRefrence(attachmentRef);
		subPass.buildSubpassDescription();

		mRenderPass->addSubPass(subPass);

		//������֮���������ϵ
		VkSubpassDependency dependency{};
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL; //��������
		dependency.dstSubpass = 0; //����subPass�����еĵ�0��subpass
		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT; //�ĸ��׶�
		dependency.srcAccessMask = 0;//�Ǹ�����,0��ʾ���������ĸ��׶�
		dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT; //�������������������ɫ������׶�ͣ�������ȴ�����Ĳ������
		dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

		mRenderPass->addDependency(dependency);

		mRenderPass->buildRenderPass();
	}

	void Application::mainLoop() {
		while (!mWindow->shouldClose()) {
			mWindow->pollEvents();

			render();
		}
		vkDeviceWaitIdle(mDevice->getDevice());
	}

	void Application::render() {
		//�ȴ���ǰҪ�ύ��frameBuffer�ύ���
		mFences[mCurrentFrame]->block();
		//��ȡ���������е���һ֡
		uint32_t imageIndex{ 0 };
		vkAcquireNextImageKHR(
			mDevice->getDevice(),
			mSwapChain->getSwapchain(),
			UINT64_MAX,
			mImageAvailableSemaphores[mCurrentFrame]->getSemaphore(),
			VK_NULL_HANDLE,
			&imageIndex);

		//�����ύ��Ϣ
		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

		//�ύ1  �ύ2  �ύ3  û��Ⱦ��  �ύ1 ���϶��ᱨ����Ҫһ�������ֶΣ�

		//ͬ����Ϣ����Ⱦ������ʾͼ�����������ʾ��Ϻ󣬲��������ɫ
		VkSemaphore waitSemaphores[] = { mImageAvailableSemaphores[mCurrentFrame]->getSemaphore() };
		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphores;
		submitInfo.pWaitDstStageMask = waitStages;
		
		//ָ���ύ��Щ����
		auto commandBuffer = mCommandBuffers[imageIndex]->getCommandBuffer();
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;

		VkSemaphore signalSemaphore[] = { mRenderFinishedSemaphores[mCurrentFrame]->getSemaphore() };
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphore;

		mFences[mCurrentFrame]->resetFence();
		//�ύ��Ⱦ����
		if (vkQueueSubmit(mDevice->getGraphicQueue(), 1, &submitInfo, mFences[mCurrentFrame]->getFence()) != VK_SUCCESS) {
			throw std::runtime_error("Error : failed to submit renderCommand");
		}

		//�ύ��ʾ����
		VkPresentInfoKHR presentInfo{};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = signalSemaphore;

		//���Ǹ���������ȥ����ʾ
		VkSwapchainKHR swapChains[] = { mSwapChain->getSwapchain() };
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = swapChains;

		//��swapchain����һ֡ȥ��Ⱦ
		presentInfo.pImageIndices = &imageIndex;

		vkQueuePresentKHR(mDevice->getPresentQueue(), &presentInfo);

		mCurrentFrame = (mCurrentFrame + 1) % mSwapChain->getImageCount(); 
	}


	void Application::clearUp() {
		mPipeline.reset();
		mRenderPass.reset();
		mSwapChain.reset();
		mDevice.reset(); 
		mSurface.reset();
		mInstance.reset();
		mWindow.reset();
	}
}