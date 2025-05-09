#pragma once

#include "../base.h"
#include "device.h"
#include "shader.h"
#include "renderPass.h"
namespace FF::Wrapper {
	class Pipeline
	{
	public:
		using Ptr = std::shared_ptr<Pipeline>;
		static Ptr create(const Device::Ptr& device, const RenderPass::Ptr& renderPass) { return std::make_shared<Pipeline>(device, renderPass); }

		Pipeline(const Device::Ptr &device, const RenderPass::Ptr &renderPass );
		~Pipeline();

		void setShaderGroup(std::vector<Shader::Ptr> &shaderGroup);

		void setViewPorts(const std::vector<VkViewport>& viewports) { mViewports = viewports; }
		void setScissors(const std::vector <VkRect2D>& scissors) { mScissors = scissors; }

		void pushBlendAttachment(const VkPipelineColorBlendAttachmentState& blendAttachment) { mBlendAttachmentStates.push_back(blendAttachment); }

		void build();

	public:
		VkPipelineVertexInputStateCreateInfo mVertexInputStage{};
		VkPipelineInputAssemblyStateCreateInfo mAssemblyState{};
		VkPipelineViewportStateCreateInfo mViewortState{};
		VkPipelineRasterizationStateCreateInfo mRasterstate{};
		VkPipelineMultisampleStateCreateInfo mSampleState{};
		std::vector<VkPipelineColorBlendAttachmentState> mBlendAttachmentStates{};
		VkPipelineColorBlendStateCreateInfo mBlendState{};
		VkPipelineDepthStencilStateCreateInfo mDepthStencilState{};
		VkPipelineLayoutCreateInfo mLayoutState{};

	public:
		[[nodiscard]] auto getPipeline() { return mPipeline; }

		//mLayout是根据mLayoutState生成的，mLayoutState里面填写的就是descriptorSetLayout
		[[nodiscard]] auto getLayout() { return mLayout; }

	private:
		VkPipeline mPipeline{ VK_NULL_HANDLE };
		VkPipelineLayout mLayout{ VK_NULL_HANDLE };
		Device::Ptr mDevice{ nullptr };
		RenderPass::Ptr mRenderPass{ nullptr };


		std::vector<Shader::Ptr> mShaders{};

		std::vector<VkViewport> mViewports{};
		std::vector<VkRect2D> mScissors{};
	};

}