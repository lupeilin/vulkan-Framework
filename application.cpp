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

		mPipeline = Wrapper::Pipeline::create(mDevice);
		
		createPipeline();
	}
	void Application::createPipeline() {
		//设置视口
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

		//设置shader
		std::vector<Wrapper::Shader::Ptr> shaderGroup{};

		auto shaderVertex = Wrapper::Shader::create(mDevice, "shaders/vs.spv", VK_SHADER_STAGE_VERTEX_BIT, "main");
		shaderGroup.push_back(shaderVertex);

		auto shaderFragment = Wrapper::Shader::create(mDevice, "shaders/fs.spv", VK_SHADER_STAGE_FRAGMENT_BIT, "main");
		shaderGroup.push_back(shaderFragment);

		mPipeline->setShaderGroup(shaderGroup);
		
		//顶点的排布模式
		mPipeline->mVertexInputStage.vertexBindingDescriptionCount = 0;
		mPipeline->mVertexInputStage.pVertexBindingDescriptions = nullptr;
		mPipeline->mVertexInputStage.vertexAttributeDescriptionCount = 0;
		mPipeline->mVertexInputStage.pVertexAttributeDescriptions = nullptr;

		//图元装配
		mPipeline->mAssemblyState.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		mPipeline->mAssemblyState.primitiveRestartEnable = VK_FALSE; //如果设置true， 从0xff 重新开始算三角形，不和前面的三角形连起来

		//光栅化设置
		mPipeline->mRasterstate.polygonMode = VK_POLYGON_MODE_FILL; //光栅化的时候如何解释这一堆图元，是fill还是line  // 其他模式需要开启gpu特性
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
		mPipeline->mLayoutState.setLayoutCount = 0;
		mPipeline->mLayoutState.pSetLayouts = nullptr;
		mPipeline->mLayoutState.pushConstantRangeCount = 0;
		mPipeline->mLayoutState.pPushConstantRanges = nullptr;

		mPipeline->build();
	}

	void Application::createRenderPass() {
		VkAttachmentDescription attachmentDes{};
		attachmentDes.format =   
	}

	void Application::mainLoop() {
		while (!mWindow->shouldClose()){
			mWindow->pollEvents();
		}
	}

	void Application::clearUp() {
		mPipeline.reset();
		mSwapChain.reset();
		mDevice.reset(); 
		mSurface.reset();
		mInstance.reset();
		mWindow.reset();
	}
}