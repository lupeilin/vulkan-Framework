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

	CommandBuffer::~CommandBuffer() {
		if (mCommandBuffer != VK_NULL_HANDLE) {
			vkFreeCommandBuffers(mDevice->getDevice(), mCommandPool->getCommandPool(), 1, &mCommandBuffer);
		}
	} //������CommandPool�������������ͷ�

	void CommandBuffer::begin(VkCommandBufferUsageFlags flag, const VkCommandBufferInheritanceInfo& inheritance) {
		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = flag;
		beginInfo.pInheritanceInfo = &inheritance;

		if (vkBeginCommandBuffer(mCommandBuffer, &beginInfo) != VK_SUCCESS) {
			throw std::runtime_error("Error : failed to begin command buffer");
		}
	}

	//VkSubpassContents �� 
		//VK_SUBPASS_CONTENTS_INLINE ����Ⱦ����ᱻ��¼��������壬�� �������϶������������
		//VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS  :��Ⱦ����ᱻ��¼�ڶ�������壬����������������beginRenderPass��ʱ��
			// ��ʹ���˶�������������� ʹ��
	void CommandBuffer::beginRenderPass(VkRenderPassBeginInfo& renderPassBeginInfo,
										const VkSubpassContents& subPassContents) {

		vkCmdBeginRenderPass(mCommandBuffer, &renderPassBeginInfo, subPassContents);
	}

	void CommandBuffer::bindVertexBuffer(const std::vector<VkBuffer>& buffers) {
		std::vector<VkDeviceSize> offsets(buffers.size(), 0); //�����ж��ٸ�buffer�����ж��ٸ�0����ʾÿһ��buffer���Ǵ�ͷ��ʼ����
		
		vkCmdBindVertexBuffers(mCommandBuffer, 0, static_cast<uint32_t>(buffers.size()), buffers.data(), offsets.data());
	}

	void CommandBuffer::bindIndexBuffer(const VkBuffer& buffer) {
		vkCmdBindIndexBuffer(mCommandBuffer, buffer, 0, VK_INDEX_TYPE_UINT32); //indexType��˵�����index��һ��int16������int32�����ǡ�����
	}

	void CommandBuffer::bindGraphicPipeline(const VkPipeline& pipeline) { //Ϊ������Ⱦ��һ��pipeline
		vkCmdBindPipeline(mCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
	}
	void CommandBuffer::draw(size_t vertexCount) {
		vkCmdDraw(mCommandBuffer, vertexCount, 1, 0, 0);
	}

	void CommandBuffer::drawIndex(size_t indexCount) {
		vkCmdDrawIndexed(mCommandBuffer, indexCount, 1, 0, 0, 0);
	}

	void CommandBuffer::endRenderPass() {
		vkCmdEndRenderPass(mCommandBuffer);
	}

	void CommandBuffer::end() {
		if(vkEndCommandBuffer(mCommandBuffer) != VK_SUCCESS) {
			throw std::runtime_error("Error : failed to end  command buffer");
		}
	}
	//const std::vector<VkBufferCopy>& copyInfos ������������ã���Ϊ�������õĻ�������һ����ʱ�������Ȼ�������ˡ�
	void CommandBuffer::copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, uint32_t copyInfoCount, const std::vector<VkBufferCopy>& copyInfos) {
		vkCmdCopyBuffer(mCommandBuffer, srcBuffer, dstBuffer, copyInfoCount, copyInfos.data());
	}

	void CommandBuffer::submitSync(VkQueue queue, VkFence fence) {
		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &mCommandBuffer;

		vkQueueSubmit(queue, 1, &submitInfo, fence);

		vkQueueWaitIdle(queue);
	}
}