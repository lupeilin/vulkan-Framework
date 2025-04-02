#pragma once

#include "../base.h"
#include "device.h"
#include "commandPoll.h"

namespace FF::Wrapper {
	class CommandBuffer
	{
	public:
		using Ptr = std::shared_ptr<CommandBuffer>;
		static Ptr create(const Device::Ptr& device, const CommandPool::Ptr& commandPool, bool asSecondry = false) {
			return std::make_shared<CommandBuffer>(device, commandPool, asSecondry); 
		}

		CommandBuffer(const Device::Ptr &device, const CommandPool::Ptr &commandPool, bool asSecondry = false);

		~CommandBuffer(); 

		//发送drawCall命令出去之前，需要构建CommandBuffer ，构建CommandBuffer需要如下几个过程
		//begin 
		// 执行指令
		// begin renderpass  
		// 绑定各类实际数据  
		/*while (n < onjectNnmber) {
			objects[i]->recordCommand()    //在一个renderPass中，给一堆object对应的二级缓冲填充命令
		}*/
		// end renderpass 
		// end   
		// 
		// 
		// 完成之后，前面的一堆命令就会按照顺序摆放在CommandBuffer中
		//然后submit， 把CommandBuffer 提交给渲染管线

		//VkCommandBufferUsageFlags :
			//VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT ： 着个命令只会被使用提交一次
			//VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT ： 这个命令缓冲是一个二级缓冲，位于一个renderPass之中
			//VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT ：命令已经被提交了，执行期间可以被再次提交
			// 
			//VkCommandBufferInheritanceInfo :如果本命令缓冲是二级缓冲，那么这个结构体，记录了他所属的主命令信息/继承信息
 		void begin(VkCommandBufferUsageFlags flag = 0, const VkCommandBufferInheritanceInfo & inheritance = {});


		//VkSubpassContents ： 
			//VK_SUBPASS_CONTENTS_INLINE ：渲染命令会被记录在主命令缓冲，即 本命令缓冲肯定就是主命令缓冲
			//VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS  :渲染命令会被记录在二级命令缓冲，适用于主命令缓冲调用beginRenderPass的时候，
				// 且使用了二级命令缓冲的情况下 使用
		void beginRenderPass(VkRenderPassBeginInfo& renderPassBeginInfo, const VkSubpassContents& subPassContents = VK_SUBPASS_CONTENTS_INLINE);

		void bindGraphicPipeline(const VkPipeline& pipeline); //为本次渲染绑定一个pipeline

		void bindVertexBuffer(const std::vector<VkBuffer>& buffers);

		void bindIndexBuffer(const VkBuffer& buffer);

		void bindDescriptorSet(const VkPipelineLayout layout, const VkDescriptorSet& descriptorSet);

		void draw(size_t vertexCount);

		void drawIndex(size_t indexCount);

		void endRenderPass();

		void end();

		void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, uint32_t copyInfoCount, const std::vector<VkBufferCopy> &copyInfos);

		void submitSync(VkQueue queue, VkFence fence);

		[[nodiscard]] auto getCommandBuffer() const { return mCommandBuffer; }

	private:
		VkCommandBuffer mCommandBuffer{ VK_NULL_HANDLE };
		Device::Ptr mDevice{ nullptr };
		CommandPool::Ptr mCommandPool{ nullptr }; 
	};
}