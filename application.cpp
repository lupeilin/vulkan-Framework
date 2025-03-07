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
		mPipeline->mRasterstate.polygonMode = VK_POLYGON_MODE_FILL; //��դ����ʱ����ν�����һ��ͼԪ����fill����line  // ����ģʽ��Ҫ����gpu����
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