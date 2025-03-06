#pragma once

#include "../base.h"
#include "device.h"

namespace FF::Wrapper {
	/*
	1、attachment
	vkAttachmentDescription 描述一个颜色或者深度模板附着的描述结构，她并不是一个真正的附着，而是一个描述
	vkAttachmentDescription1 vkAttanchmentDescription2 vkAttanchmentDescription3 vkAttanchmentDescription4（深度模板），告诉外界我要什么
	vkAttachmentRefrence 说明本个subpass  需要的attachment其中一个，这一个的数组索引id是多少，这一个附着的图片期望格式是什么
	vkSubpass 用来填写一个子pass的描述结构
	vkSubpassDependency  描述不同的子流程之间的依赖关系
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