#include"application.h"
#include "vulkanWrapper/image.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
namespace FF {

	void Application::run() {
		initWindow();
		initVulkan();
		mainLoop();
		clearUp();
	}

	void Application::initWindow() {
		mWindow = Wrapper::Window::create(mWidth, mHeight);
	}

	void Application::initVulkan() {
		mInstance = Wrapper::Instance::create(true);
		mSurface = Wrapper::WindowSurface::create(mInstance, mWindow);
		mDevice = Wrapper::Device::create(mInstance, mSurface);

		mCommandPool = Wrapper::CommandPool::create(mDevice);

		mSwapChain = Wrapper::SwapChain::create(mDevice, mWindow, mSurface, mCommandPool);

		mRenderPass = Wrapper::RenderPass::create(mDevice);
		createRenderPass();

		mSwapChain->createFrameBuffers(mRenderPass);

		mWidth = mSwapChain->getExtent().width;
		mHeight = mSwapChain->getExtent().height;

		//descriptor============================ 
		mUniformManager = UniformManager::create();
		mUniformManager->init(mDevice, mCommandPool, mSwapChain->getImageCount());

		//����ģ��  ֮�����ǾͿ�����pipeline�ṩ������Ϣ�� 
		mModel = Model::create(mDevice);

		mPipeline = Wrapper::Pipeline::create(mDevice, mRenderPass);
		
		createPipeline();

		mCommandBuffers.resize(mSwapChain->getImageCount());

		createCommandBufers();

		createSyncObject();

	}
	void Application::createPipeline() {
		//�����ӿ�
		VkViewport viewport = {};
		viewport.x = 0.0f;
		/*viewport.y = 0.0f;*/
		viewport.y = (float)mHeight;
		viewport.width = (float)mWidth;
		/*viewport.height = (float)mHeight;*/
		viewport.height = -(float)mHeight;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0;

		VkRect2D scissor = {};
		scissor.offset = { 0, 0 };
		scissor.extent = { mWidth, mHeight };

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
		auto vertexBindingDes = mModel->getVertexInputBindingDescriptions();
		auto attributeDes = mModel->getAttributeDescriptions();

		mPipeline->mVertexInputStage.vertexBindingDescriptionCount = static_cast<uint32_t>(vertexBindingDes.size());
		mPipeline->mVertexInputStage.pVertexBindingDescriptions = vertexBindingDes.data();
		mPipeline->mVertexInputStage.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDes.size());
		mPipeline->mVertexInputStage.pVertexAttributeDescriptions = attributeDes.data();

		//ͼԪװ��
		mPipeline->mAssemblyState.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		mPipeline->mAssemblyState.primitiveRestartEnable = VK_FALSE; //�������true�� ��0xff ���¿�ʼ�������Σ�����ǰ���������������

		//��դ������
		mPipeline->mRasterstate.polygonMode = VK_POLYGON_MODE_FILL; //��դ����ʱ����ν�����һ��ͼԪ����fill����line  // ����ģʽ��Ҫ����gpu���� //����λ���ģʽ����䡢�߿򡢵㣩
		mPipeline->mRasterstate.lineWidth = 1.0f;//����1��Ҫ����gpu����
		mPipeline->mRasterstate.cullMode = VK_CULL_MODE_BACK_BIT; //�����޳�
		//mPipeline->mRasterstate.frontFace = VK_FRONT_FACE_CLOCKWISE; // ˳ʱ��Ϊ����
		mPipeline->mRasterstate.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE; // ��ʱ��Ϊ����

		mPipeline->mRasterstate.depthBiasEnable = VK_FALSE;//�ڶ�����Ⱦ����ȣ�����ʵ�����Сһ�㣬��ԭ�������������һ����Ҷ˹�� 
		mPipeline->mRasterstate.depthBiasConstantFactor = 0.0f;
		mPipeline->mRasterstate.depthBiasClamp = 0.0f;
		mPipeline->mRasterstate.depthBiasSlopeFactor = 0.0f;

		//TODO:���ز���
		mPipeline->mSampleState.sampleShadingEnable = VK_FALSE;
		mPipeline->mSampleState.rasterizationSamples = mDevice->getMaxUsableSampleCount();
		mPipeline->mSampleState.minSampleShading = 1.0; 
		mPipeline->mSampleState.pSampleMask = nullptr;
		mPipeline->mSampleState.alphaToCoverageEnable = VK_FALSE;
		mPipeline->mSampleState.alphaToOneEnable = VK_FALSE;

		//TODO:�����ģ�����
		mPipeline->mDepthStencilState.depthTestEnable = VK_TRUE;
		mPipeline->mDepthStencilState.depthWriteEnable = VK_TRUE;
		mPipeline->mDepthStencilState.depthCompareOp = VK_COMPARE_OP_LESS;

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
		mPipeline->mLayoutState.setLayoutCount = 1;

		auto layout = mUniformManager->getDescriptorSetLayout()->getLayout();
		mPipeline->mLayoutState.pSetLayouts = &layout;
		mPipeline->mLayoutState.pushConstantRangeCount = 0;
		mPipeline->mLayoutState.pPushConstantRanges = nullptr;

		mPipeline->build();
	}

	void Application::createRenderPass() {

		//0���������ͼƬ  1��resolveͼƬ��Mutisample�� 2��depthͼƬ
		//���뻭�������� 0��λ���������  ��swapchainԭ������ͼƬ����resolved��Ŀ��㣬����Ҫ���õ�subpass��resolved����
		VkAttachmentDescription finalAttachmentDes{};
		finalAttachmentDes.format = mSwapChain->getFormat();
		finalAttachmentDes.samples = VK_SAMPLE_COUNT_1_BIT;
		finalAttachmentDes.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;//����ͼƬ������ʱ�򣬰ѻ�������renderpass ��������ԭ�������ݱ������߲�����
		finalAttachmentDes.storeOp = VK_ATTACHMENT_STORE_OP_STORE;//�������ΪrenderPass�������Ҫ������ʾ
		finalAttachmentDes.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		finalAttachmentDes.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		finalAttachmentDes.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;   
		finalAttachmentDes.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
		mRenderPass->addAttchment(finalAttachmentDes);

		//1��λ�� ��resolvedͼƬ���������ز�����ԴͷͼƬ��Ҳ����ɫ�����Ŀ��ͼƬ
		VkAttachmentDescription mutiAttachmentDes{};
		mutiAttachmentDes.format = mSwapChain->getFormat();
		mutiAttachmentDes.samples = mDevice->getMaxUsableSampleCount();
		mutiAttachmentDes.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;//����ͼƬ������ʱ�򣬰ѻ�������renderpass ��������ԭ�������ݱ������߲�����
		mutiAttachmentDes.storeOp = VK_ATTACHMENT_STORE_OP_STORE;//�������ΪrenderPass�������Ҫ������ʾ
		mutiAttachmentDes.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		mutiAttachmentDes.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		mutiAttachmentDes.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		mutiAttachmentDes.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		mRenderPass->addAttchment(mutiAttachmentDes);

		//��Ȼ���attachment 2��λ��depth
		VkAttachmentDescription depthattachmentDes{};
		depthattachmentDes.format = Wrapper::Image::finddepthFormat(mDevice);
		depthattachmentDes.samples = mDevice->getMaxUsableSampleCount();
		depthattachmentDes.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;//����ͼƬ������ʱ�򣬰ѻ�������renderpass ��������ԭ�������ݱ������߲�����
		//����Ҫ��ǰ���colorAttanchment�������棬��Ϊrenderpass����֮��Ͳ���Ҫ���ֵ�ˣ��Ѿ���fragment�����ͼƬ����
		depthattachmentDes.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depthattachmentDes.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		depthattachmentDes.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depthattachmentDes.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		depthattachmentDes.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		mRenderPass->addAttchment(depthattachmentDes);

		//���ڻ������������ã��Լ���ʽҪ��
		VkAttachmentReference finalAttachmentRef{};
		finalAttachmentRef.attachment = 0;
		finalAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkAttachmentReference mutiAttachmentRef{};
		mutiAttachmentRef.attachment = 1;
		mutiAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkAttachmentReference depthAttachmentRef{};
		depthAttachmentRef.attachment = 2;
		depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		
		//����������
		Wrapper::SubPass subPass{};
		subPass.addColorAttachmentRefrence(mutiAttachmentRef);
		subPass.addDepthStencilAttachmentRefrence(depthAttachmentRef);
		subPass.setResolvedAttachmentRefrence(finalAttachmentRef);
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

	void Application::createCommandBufers() {

		for (uint32_t i = 0; i < mSwapChain->getImageCount(); ++i) {
			mCommandBuffers[i] = Wrapper::CommandBuffer::create(mDevice, mCommandPool);

			mCommandBuffers[i]->begin();

			VkRenderPassBeginInfo renderPassBeginInfo{};
			renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			renderPassBeginInfo.renderPass = mRenderPass->getRenderPass(); 
			renderPassBeginInfo.framebuffer = mSwapChain->getFrameBuffer(i);
			renderPassBeginInfo.renderArea.offset = { 0, 0 };
			renderPassBeginInfo.renderArea.extent = mSwapChain->getExtent();

			//0:FINAL 1:muti 2:depth
			std::vector<VkClearValue> clearColors{};
			VkClearValue finalClearColor{};
			finalClearColor.color = { 0.0f, 0.0f, 0.0f, 1.0f };
			clearColors.push_back(finalClearColor);

			VkClearValue MutiClearColor{};
			MutiClearColor.color = { 0.0f, 0.0f, 0.0f, 1.0f };
			clearColors.push_back(MutiClearColor);

			VkClearValue depthClearColor{};
			depthClearColor.depthStencil = { 1.0f, 0};

			clearColors.push_back(depthClearColor);

			renderPassBeginInfo.clearValueCount = static_cast<uint32_t>(clearColors.size());
			renderPassBeginInfo.pClearValues = clearColors.data();

			mCommandBuffers[i]->beginRenderPass(renderPassBeginInfo);

			mCommandBuffers[i]->bindGraphicPipeline(mPipeline->getPipeline());

			mCommandBuffers[i]->bindDescriptorSet(mPipeline->getLayout(), mUniformManager->getmDescriptorSet(mCurrentFrame));

			//mCommandBuffers[i]->bindVertexBuffer({ mModel->getVertexBuffer()->getBuffer() });
			mCommandBuffers[i]->bindVertexBuffer({ mModel->getVertexBuffers() });

			mCommandBuffers[i]->bindIndexBuffer(mModel->getIndexBuffer()->getBuffer());

			mCommandBuffers[i]->drawIndex(mModel->getIndexCount());

			mCommandBuffers[i]->endRenderPass();

			mCommandBuffers[i]->end();
		}
	}

	void Application::createSyncObject() {
		for (uint32_t i = 0; i < mSwapChain->getImageCount(); ++i) {
			auto imageSemaphore = Wrapper::Semaphore::create(mDevice);
			mImageAvailableSemaphores.push_back(imageSemaphore);

			auto renderSemaphore = Wrapper::Semaphore::create(mDevice);
			mRenderFinishedSemaphores.push_back(renderSemaphore);

			//����ÿһ��������һ��fence
			auto fence = Wrapper::Fence::create(mDevice);
			mFences.push_back(fence);
		}
	} 

	void Application::recreateSwapChain() {
		//��������С�������⣬���������С���ˣ����ǾͲ��ż��ؽ�
		int width = 0, height = 0;
		glfwGetFramebufferSize(mWindow->getWindow(), &width, &height);
		while (width == 0 || height == 0) {
			glfwWaitEvents();
			glfwGetFramebufferSize(mWindow->getWindow(), &width, &height);
		}

		//�ȴ���ǰ�����е��������
		vkDeviceWaitIdle(mDevice->getDevice());

		//������ص���Դ
		cleanUpSwapChain();

		//�ؽ�
		mSwapChain = Wrapper::SwapChain::create(mDevice, mWindow, mSurface, mCommandPool);
		mWidth = mSwapChain->getExtent().width;
		mHeight = mSwapChain->getExtent().height;

		mRenderPass = Wrapper::RenderPass::create(mDevice);
		createRenderPass();

		mSwapChain->createFrameBuffers(mRenderPass);

		mPipeline = Wrapper::Pipeline::create(mDevice, mRenderPass);

		createPipeline();

		mCommandBuffers.resize(mSwapChain->getImageCount());

		createCommandBufers();

		createSyncObject();
	}

	void Application::cleanUpSwapChain() {
		mSwapChain.reset();    //��Ϊ����ָ��ļ���Ϊ0��ʱ�����ǵĶ���ͻᱻ�ͷš� 
		mCommandBuffers.clear();
		mPipeline.reset();
		mRenderPass.reset();
		mImageAvailableSemaphores.clear();
		mRenderFinishedSemaphores.clear();
		mFences.clear();
	}

	void Application::mainLoop() {
		while (!mWindow->shouldClose()) {
			mWindow->pollEvents(); // �������¼������̡���ꡢ���ڴ�С������

			mModel->updata();

			mUniformManager->updata(mVPMatrix, mModel->getUniform(), mCurrentFrame); //����uniform
			
			render();
		}
		vkDeviceWaitIdle(mDevice->getDevice());
	}

	void Application::render() {
		//�ȴ���ǰҪ�ύ��frameBuffer�ύ���
		mFences[mCurrentFrame]->block();
		//��ȡ���������е���һ֡
		uint32_t imageIndex{ 0 };
		VkResult result = vkAcquireNextImageKHR(
			mDevice->getDevice(),
			mSwapChain->getSwapchain(),
			UINT64_MAX,
			mImageAvailableSemaphores[mCurrentFrame]->getSemaphore(),
			VK_NULL_HANDLE,
			&imageIndex);

		//���巢���˳ߴ�仯
		if (result == VK_ERROR_OUT_OF_DATE_KHR) {
			recreateSwapChain();
			mWindow->mWindowResized = false;
		}//VK_SUBOPTIMAL_KHR ���õ���һ����Ϊ���õ�ͼ�񣬵��Ǳ����ʽ��һ��ƥ��
		else if(result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR){
			throw std::runtime_error("Error : failed to acquire next image");
		}

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

		result = vkQueuePresentKHR(mDevice->getPresentQueue(), &presentInfo);
		//������������һ����׼���������ǻ���Ҫ���Լ��ı�־λ�ж�
		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || mWindow->mWindowResized) {
			recreateSwapChain();
			mWindow->mWindowResized = false;
		} 
		else if (result != VK_SUCCESS) {
			throw std::runtime_error("Error : failed to present");
		}

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