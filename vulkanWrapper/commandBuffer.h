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

		//����drawCall�����ȥ֮ǰ����Ҫ����CommandBuffer ������CommandBuffer��Ҫ���¼�������
		//begin 
		// ִ��ָ��
		// begin renderpass  
		// �󶨸���ʵ������  
		/*while (n < onjectNnmber) {
			objects[i]->recordCommand()    //��һ��renderPass�У���һ��object��Ӧ�Ķ��������������
		}*/
		// end renderpass 
		// end   
		// 
		// 
		// ���֮��ǰ���һ������ͻᰴ��˳��ڷ���CommandBuffer��
		//Ȼ��submit�� ��CommandBuffer �ύ����Ⱦ����

		//VkCommandBufferUsageFlags :
			//VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT �� �Ÿ�����ֻ�ᱻʹ���ύһ��
			//VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT �� ����������һ���������壬λ��һ��renderPass֮��
			//VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT �������Ѿ����ύ�ˣ�ִ���ڼ���Ա��ٴ��ύ
			// 
			//VkCommandBufferInheritanceInfo :�����������Ƕ������壬��ô����ṹ�壬��¼������������������Ϣ/�̳���Ϣ
 		void begin(VkCommandBufferUsageFlags flag = 0, const VkCommandBufferInheritanceInfo & inheritance = {});


		//VkSubpassContents �� 
			//VK_SUBPASS_CONTENTS_INLINE ����Ⱦ����ᱻ��¼��������壬�� �������϶������������
			//VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS  :��Ⱦ����ᱻ��¼�ڶ�������壬����������������beginRenderPass��ʱ��
				// ��ʹ���˶�������������� ʹ��
		void beginRenderPass(VkRenderPassBeginInfo& renderPassBeginInfo, const VkSubpassContents& subPassContents = VK_SUBPASS_CONTENTS_INLINE);

		void bindGraphicPipeline(const VkPipeline& pipeline); //Ϊ������Ⱦ��һ��pipeline

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