#include "commandBuffer.h"

namespace FF::Wrapper {
	CommandBuffer::CommandBuffer(const Device::Ptr& device, const CommandPool::Ptr& commandPool, bool asSecondry ) {
		mDevice = device;
		mCommandPool = commandPool;

		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandBufferCount = 1;
		allocInfo.commandPool = commandPool->getCommandPool();
		allocInfo.level = asSecondry ? VK_COMMAND_BUFFER_LEVEL_SECONDARY : VK_COMMAND_BUFFER_LEVEL_PRIMARY;

		if (vkAllocateCommandBuffers(mDevice->getDevice(), &allocInfo, &mCommandBuffer) != VK_SUCCESS) {
			throw std::runtime_error("Error : failed to create command buffer");
		}
	}

	CommandBuffer::~CommandBuffer() {} //会随着CommandPool析构，而析构释放

	void CommandBuffer::begin(VkCommandBufferUsageFlags flag, const VkCommandBufferInheritanceInfo& inheritance) {
		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = flag;
		beginInfo.pInheritanceInfo = &inheritance;

		if (vkBeginCommandBuffer(mCommandBuffer, &beginInfo) != VK_SUCCESS) {
			throw std::runtime_error("Error : failed to begin command buffer");
		}
	}

	//VkSubpassContents ： 
		//VK_SUBPASS_CONTENTS_INLINE ：渲染命令会被记录在主命令缓冲，即 本命令缓冲肯定就是主命令缓冲
		//VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS  :渲染命令会被记录在二级命令缓冲，适用于主命令缓冲调用beginRenderPass的时候，
			// 且使用了二级命令缓冲的情况下 使用
	void CommandBuffer::beginRenderPass(VkRenderPassBeginInfo& renderPassBeginInfo,
										const VkSubpassContents& subPassContents) {

		vkCmdBeginRenderPass(mCommandBuffer, &renderPassBeginInfo, subPassContents);
	}

	void CommandBuffer::bindGraphicPipeline(const VkPipeline& pipeline) { //为本次渲染绑定一个pipeline
		vkCmdBindPipeline(mCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
	}
	void CommandBuffer::draw(size_t vertexCount) {
		vkCmdDraw(mCommandBuffer, vertexCount, 1, 0, 0);
	}

	void CommandBuffer::endRenderPass() {
		vkCmdEndRenderPass(mCommandBuffer);
	}

	void CommandBuffer::end() {
		if(vkEndCommandBuffer(mCommandBuffer) != VK_SUCCESS) {
			throw std::runtime_error("Error : failed to end  command buffer");
		}
	}

}