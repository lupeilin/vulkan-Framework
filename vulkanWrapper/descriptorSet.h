#pragma once

#include "../base.h"
#include "device.h"
#include "description.h"
#include "descriptorSetLayout.h"
#include "descriptorPool.h"

namespace FF::Wrapper {

	/*
	* ����ÿһ��ģ�͵���Ⱦ������Ҫ��һ��DescriptorSet���󶨵�λ�þ���CommandBuffer
	* ÿһ��DescriptorSet���棬����Ӧ��һ��vp����ʹ�õ�buffer��һ��model����ʹ�õ�buffer���ȵȡ�����Ҳ������
	*  binding��size�ȵ�������Ϣ
	* ���ڽ������Ĵ��ڣ���֡���ܲ�����Ⱦ������������ҪΪÿһ����������ͼƬ����Ӧ����һ��DescriptorSet��
	*/
	
	//���class���ջ���Ϊһ��model������Ķ���
	class DescriptorSet
	{
	public:
		using Ptr = std::shared_ptr<DescriptorSet>;
		static Ptr create(
			const Device::Ptr& device,
			const std::vector<UniformParameter::Ptr> params,
			const DescriptorSetLayout::Ptr& layout,
			const DescriptorPool::Ptr& pool,
			int frameCount
		) {
			return std::make_shared<DescriptorSet>(device, params, layout, pool, frameCount);
		}

		DescriptorSet(
			const Device::Ptr &device,
			const std::vector<UniformParameter::Ptr> params,
			const DescriptorSetLayout::Ptr &layout,
			const DescriptorPool::Ptr &pool,
			int frameCount
		);

		~DescriptorSet();

	private:
		std::vector<VkDescriptorSet> mDescriptorSets{};
		Device::Ptr mDevice{ nullptr };
	};
}