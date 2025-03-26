#include"application.h"

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
		mSwapChain = Wrapper::SwapChain::create(mDevice, mWindow, mSurface);

		mRenderPass = Wrapper::RenderPass::create(mDevice);
		createRenderPass();

		mSwapChain->createFrameBuffers(mRenderPass);

		mWidth = mSwapChain->getExtent().width;
		mHeight = mSwapChain->getExtent().height;

		//descriptor============================ 
		mUniformManager = UniformManager::create();
		mUniformManager->init(mDevice, mSwapChain->getImageCount());

		//创建模型  之后我们就可以向pipeline提供描述信息了 
		mModel = Model::create(mDevice);

		mPipeline = Wrapper::Pipeline::create(mDevice, mRenderPass);
		
		createPipeline();

		mCommandPool = Wrapper::CommandPool::create(mDevice);

		mCommandBuffers.resize(mSwapChain->getImageCount());

		createCommandBufers();

		createSyncObject();

	}
	void Application::createPipeline() {
		//设置视口
		VkViewport viewport = {};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = (float)mWidth;
		viewport.height = (float)mHeight;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0;

		VkRect2D scissor = {};
		scissor.offset = { 0, 0 };
		scissor.extent = { mWidth, mHeight };

		mPipeline->setViewPorts({ viewport });
		mPipeline->setScissors({ scissor });

		//设置shader
		std::vector<Wrapper::Shader::Ptr> shaderGroup{};

		auto shaderVertex = Wrapper::Shader::create(mDevice, "shaders/vs.spv", VK_SHADER_STAGE_VERTEX_BIT, "main");
		shaderGroup.push_back(shaderVertex);

		auto shaderFragment = Wrapper::Shader::create(mDevice, "shaders/fs.spv", VK_SHADER_STAGE_FRAGMENT_BIT, "main");
		shaderGroup.push_back(shaderFragment);

		mPipeline->setShaderGroup(shaderGroup);
		
		//顶点的排布模式
		auto vertexBindingDes = mModel->getVertexInputBindingDescriptions();
		auto attributeDes = mModel->getAttributeDescriptions();

		mPipeline->mVertexInputStage.vertexBindingDescriptionCount = static_cast<uint32_t>(vertexBindingDes.size());
		mPipeline->mVertexInputStage.pVertexBindingDescriptions = vertexBindingDes.data();
		mPipeline->mVertexInputStage.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDes.size());
		mPipeline->mVertexInputStage.pVertexAttributeDescriptions = attributeDes.data();

		//图元装配
		mPipeline->mAssemblyState.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		mPipeline->mAssemblyState.primitiveRestartEnable = VK_FALSE; //如果设置true， 从0xff 重新开始算三角形，不和前面的三角形连起来

		//光栅化设置
		mPipeline->mRasterstate.polygonMode = VK_POLYGON_MODE_FILL; //光栅化的时候如何解释这一堆图元，是fill还是line  // 其他模式需要开启gpu特性 //多边形绘制模式（填充、线框、点）
		mPipeline->mRasterstate.lineWidth = 1.0f;//大于1需要开启gpu特性
		mPipeline->mRasterstate.cullMode = VK_CULL_MODE_BACK_BIT; //背面剔除
		mPipeline->mRasterstate.frontFace = VK_FRONT_FACE_CLOCKWISE; // 顺时针为正面

		mPipeline->mRasterstate.depthBiasEnable = VK_FALSE;//第二次渲染的深度，比真实的深度小一点，在原来的深度上设置一个贝叶斯。 
		mPipeline->mRasterstate.depthBiasConstantFactor = 0.0f;
		mPipeline->mRasterstate.depthBiasClamp = 0.0f;
		mPipeline->mRasterstate.depthBiasSlopeFactor = 0.0f;

		//TODO:多重采样
		mPipeline->mSampleState.sampleShadingEnable = VK_FALSE;
		mPipeline->mSampleState.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		mPipeline->mSampleState.minSampleShading = 1.0; 
		mPipeline->mSampleState.pSampleMask = nullptr;
		mPipeline->mSampleState.alphaToCoverageEnable = VK_FALSE;
		mPipeline->mSampleState.alphaToOneEnable = VK_FALSE;

		//TODO:深度与模板测试

		//颜色混合
		//这个是颜色混合掩码，得到的混合结果，按照通道与掩码进行and操作，输出
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

		//1、blend有两种计算的方式，第一种如上所示，进行alpha为基础的计算，第二种进行位运算。
		//2、如果开启了logicop，那么上方设置的alpha为基础的运算，失灵
		//3、colorWrite掩码，任然有效，即便开启了logicOp
		//4、 因为我们可能会有多个franmebuffer输出，所以可能需要多个blendattachment
		mPipeline->mBlendState.logicOpEnable = VK_FALSE;
		mPipeline->mBlendState.logicOp = VK_LOGIC_OP_COPY;

		//配合blend Attachment的Factor和operation
		mPipeline->mBlendState.blendConstants[0] = 0.0f;
		mPipeline->mBlendState.blendConstants[1] = 0.0f;
		mPipeline->mBlendState.blendConstants[2] = 0.0f;
		mPipeline->mBlendState.blendConstants[3] = 0.0f;

		//uniform的传递
		mPipeline->mLayoutState.setLayoutCount = 1;

		auto layout = mUniformManager->getDescriptorSetLayout()->getLayout();
		mPipeline->mLayoutState.pSetLayouts = &layout;
		mPipeline->mLayoutState.pushConstantRangeCount = 0;
		mPipeline->mLayoutState.pPushConstantRanges = nullptr;

		mPipeline->build();
	}

	void Application::createRenderPass() {
		//输入画布的描述
		VkAttachmentDescription attachmentDes{};
		attachmentDes.format = mSwapChain->getFormat();
		attachmentDes.samples = VK_SAMPLE_COUNT_1_BIT;
		attachmentDes.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;//加载图片进来的时候，把画布给到renderpass ，画布上原来的数据保留或者不保留
		attachmentDes.storeOp = VK_ATTACHMENT_STORE_OP_STORE;//输出
		attachmentDes.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		attachmentDes.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		attachmentDes.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		attachmentDes.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		mRenderPass->addAttchment(attachmentDes);

		//对于画布的索引设置，以及格式要求
		VkAttachmentReference attachmentRef{};
		attachmentRef.attachment = 0;
		attachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		
		//创建子流程
		Wrapper::SubPass subPass{};
		subPass.addColorAttachmentRefrence(attachmentRef);
		subPass.buildSubpassDescription();

		mRenderPass->addSubPass(subPass);

		//子流程之间的依赖关系
		VkSubpassDependency dependency{};
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL; //虚拟流程
		dependency.dstSubpass = 0; //代表subPass数组中的第0个subpass
		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT; //哪个阶段
		dependency.srcAccessMask = 0;//那个操作,0表示不关心在哪个阶段
		dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT; //即将在输出到画布上颜色的这个阶段停下来，等待上面的操作完成
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

			VkClearValue  clearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
			renderPassBeginInfo.clearValueCount = 1;
			renderPassBeginInfo.pClearValues = &clearColor;

			mCommandBuffers[i]->beginRenderPass(renderPassBeginInfo);

			mCommandBuffers[i]->bindGraphicPipeline(mPipeline->getPipeline());

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

			//对于每一个都生成一个fence
			auto fence = Wrapper::Fence::create(mDevice);
			mFences.push_back(fence);
		}
	} 

	void Application::recreateSwapChain() {
		//处理窗体最小化的问题，如果窗体最小化了，我们就不着急重建
		int width = 0, height = 0;
		glfwGetFramebufferSize(mWindow->getWindow(), &width, &height);
		while (width == 0 || height == 0) {
			glfwWaitEvents();
			glfwGetFramebufferSize(mWindow->getWindow(), &width, &height);
		}

		//等待当前窗口中的任务完成
		vkDeviceWaitIdle(mDevice->getDevice());

		//清理相关的资源
		cleanUpSwapChain();

		//重建
		mSwapChain = Wrapper::SwapChain::create(mDevice, mWindow, mSurface);
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
		mSwapChain.reset();    //因为智能指针的计数为0的时候，我们的对象就会被释放。 
		mCommandBuffers.clear();
		mPipeline.reset();
		mRenderPass.reset();
		mImageAvailableSemaphores.clear();
		mRenderFinishedSemaphores.clear();
		mFences.clear();
	}

	void Application::mainLoop() {
		while (!mWindow->shouldClose()) {
			mWindow->pollEvents();

			render();
		}
		vkDeviceWaitIdle(mDevice->getDevice());
	}

	void Application::render() {
		//等待当前要提交的frameBuffer提交完毕
		mFences[mCurrentFrame]->block();
		//获取交换链当中的下一帧
		uint32_t imageIndex{ 0 };
		VkResult result = vkAcquireNextImageKHR(
			mDevice->getDevice(),
			mSwapChain->getSwapchain(),
			UINT64_MAX,
			mImageAvailableSemaphores[mCurrentFrame]->getSemaphore(),
			VK_NULL_HANDLE,
			&imageIndex);

		//窗体发生了尺寸变化
		if (result == VK_ERROR_OUT_OF_DATE_KHR) {
			recreateSwapChain();
			mWindow->mWindowResized = false;
		}//VK_SUBOPTIMAL_KHR ：得到了一张认为可用的图像，但是表面格式不一定匹配
		else if(result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR){
			throw std::runtime_error("Error : failed to acquire next image");
		}

		//构建提交信息
		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

		//提交1  提交2  提交3  没渲染完  提交1 （肯定会报错，需要一个控制手段）

		//同步信息，渲染对于显示图像的依赖，显示完毕后，才能输出颜色
		VkSemaphore waitSemaphores[] = { mImageAvailableSemaphores[mCurrentFrame]->getSemaphore() };
		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphores;
		submitInfo.pWaitDstStageMask = waitStages;
		
		//指定提交哪些命令
		auto commandBuffer = mCommandBuffers[imageIndex]->getCommandBuffer();
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;

		VkSemaphore signalSemaphore[] = { mRenderFinishedSemaphores[mCurrentFrame]->getSemaphore() };
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphore;

		mFences[mCurrentFrame]->resetFence();
		//提交渲染命令
		if (vkQueueSubmit(mDevice->getGraphicQueue(), 1, &submitInfo, mFences[mCurrentFrame]->getFence()) != VK_SUCCESS) {
			throw std::runtime_error("Error : failed to submit renderCommand");
		}

		//提交显示命令
		VkPresentInfoKHR presentInfo{};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = signalSemaphore;

		//到那个交换链上去做显示
		VkSwapchainKHR swapChains[] = { mSwapChain->getSwapchain() };
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = swapChains;

		//用swapchain的哪一帧去渲染
		presentInfo.pImageIndices = &imageIndex;

		result = vkQueuePresentKHR(mDevice->getPresentQueue(), &presentInfo);
		//由于驱动程序不一定精准，所以我们还需要用自己的标志位判断
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