#include "pipeline.h"

namespace FF::Wrapper {

	Pipeline::Pipeline(const Device::Ptr& device) {
		mDevice = device;

		mVertexInputStage.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		mAssemblyState.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		mViewortState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		mRasterstate.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		mSampleState.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		mBlendState.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		mDepthStencilState.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		mLayoutState.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	}

	Pipeline::~Pipeline() {
		if (mLayout != VK_NULL_HANDLE) {
			vkDestroyPipelineLayout(mDevice->getDevice(), mLayout, nullptr);
		}

		if (mPipeline != VK_NULL_HANDLE) {
			vkDestroyPipeline(mDevice->getDevice(), mPipeline, nullptr);
		}
	}

	void Pipeline::setShaderGroup(std::vector<Shader::Ptr>& shaderGroup) {
		mShaders = shaderGroup;
	}

	void Pipeline::build() {
		//设置shader
		std::vector<VkPipelineShaderStageCreateInfo> shaderCreateInfos{};
		for (const auto& shader : mShaders) {
			VkPipelineShaderStageCreateInfo shaderCreateInfo{};
			shaderCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			shaderCreateInfo.stage = shader->getShaderStage();
			shaderCreateInfo.pName = shader->getShaderEntruPoint().c_str();
			shaderCreateInfo.module = shader->getShaderModule();

			shaderCreateInfos.push_back(shaderCreateInfo);
		}

		//设置视口剪裁
		mViewortState.viewportCount = static_cast<uint32_t>(mViewports.size());
		mViewortState.pViewports = mViewports.data();
		mViewortState.scissorCount = static_cast<uint32_t>(mScissors.size());
		mViewortState.pScissors = mScissors.data();

		//blending
		mBlendState.attachmentCount = static_cast<uint32_t>(mBlendAttachmentStates.size());
		mBlendState.pAttachments = mBlendAttachmentStates.data();

		//layout生成
		if (mLayout != VK_NULL_HANDLE) {
			vkDestroyPipelineLayout(mDevice->getDevice(), mLayout, nullptr);
		}

		if (vkCreatePipelineLayout(mDevice->getDevice(), &mLayoutState, nullptr, &mLayout)) {
			throw std::runtime_error("Eorror : failed to create pipeline layout");
		}

		VkGraphicsPipelineCreateInfo pipelineCreateInfo{};
		pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineCreateInfo.stageCount = static_cast<uint32_t>(shaderCreateInfos.size());
		pipelineCreateInfo.pStages = shaderCreateInfos.data();

		pipelineCreateInfo.pVertexInputState = &mVertexInputStage;
		pipelineCreateInfo.pInputAssemblyState = &mAssemblyState;
		pipelineCreateInfo.pViewportState = &mViewortState;
		pipelineCreateInfo.pRasterizationState = &mRasterstate;
		pipelineCreateInfo.pMultisampleState = &mSampleState;
		pipelineCreateInfo.pDepthStencilState = nullptr; //TODO
		pipelineCreateInfo.pColorBlendState = &mBlendState;
		pipelineCreateInfo.layout = mLayout;
		pipelineCreateInfo.renderPass = VK_NULL_HANDLE; // TODO: add render pass
		pipelineCreateInfo.subpass = 0;

		//以存在的pipeline为基础进行创建，会更快，但是需要指定flags为 VK_PIPELINE_CREATE_DERIVATIVE_BIT
		pipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
		// pipelineCreateInfo.flags = 
		pipelineCreateInfo.basePipelineIndex = -1;

		if (mPipeline != VK_NULL_HANDLE) {
			vkDestroyPipeline(mDevice->getDevice(), mPipeline, nullptr);
		}
		//pipeline chache ，可以将相关数据存入缓存，在多个pipeline当中使用，也可以存到文件，不同程序调用
		/*if (vkCreateGraphicsPipelines(mDevice->getDevice(), VK_NULL_HANDLE, 1 ,&pipelineCreateInfo, nullptr, &mPipeline) != VK_SUCCESS) {
			throw std::runtime_error("Error : failed to create pipeline");
		}*/
	}

}