#pragma once

#include "../base.h"
#include "device.h"

namespace FF::Wrapper {
	/*
	1��attachment
	vkAttachmentDescription ����һ����ɫ�������ģ�帽�ŵ������ṹ����������һ�������ĸ��ţ�����һ������
	vkAttachmentDescription1 vkAttanchmentDescription2 vkAttanchmentDescription3 vkAttanchmentDescription4�����ģ�壩�����������Ҫʲô
	vkAttachmentRefrence ˵������subpass  ��Ҫ��attachment����һ������һ������������id�Ƕ��٣���һ�����ŵ�ͼƬ������ʽ��ʲô
	vkSubpass ������дһ����pass�������ṹ
	vkSubpassDependency  ������ͬ��������֮���������ϵ
	*/

	class SubPass
	{
	public:
		SubPass();
		~SubPass();

	private:
		VkSubpassDescription mSubpass{};
	};

	class RenderPass
	{
	public:
		using Ptr = std::shared_ptr<RenderPass>;
		RenderPass();
		~RenderPass();

	private:

	};
}