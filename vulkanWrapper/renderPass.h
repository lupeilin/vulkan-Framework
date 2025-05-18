#pragma once

#include "../base.h"
#include "device.h"

namespace FF::Wrapper {
	/*
	1、attachment
	vkAttachmentDescription 描述一个颜色或者深度模板附着的描述结构，她并不是一个真正的附着，而是一个描述
	vkAttachmentDescription1 vkAttanchmentDescription2 vkAttanchmentDescription3 vkAttanchmentDescription4（深度模板），告诉外界我要什么
	vkAttachmentRefrence 说明本个subpass  需要的attachment其中一个，这一个的数组索引id是多少，这一个附着的图片期望格式是什么
	VkSubpassDescription 用来填写一个子pass的描述结构
	vkSubpassDependency  描述不同的子流程之间的依赖关系
	*/

	class SubPass
	{
	public:

		SubPass();
		~SubPass();

		void addColorAttachmentRefrence(const VkAttachmentReference& ref);

		void addInputAttachmentRefrence(const VkAttachmentReference& ref);

		void addDepthStencilAttachmentRefrence(const VkAttachmentReference& ref);

		void setResolvedAttachmentRefrence(const VkAttachmentReference& ref);

		void buildSubpassDescription();

		[[nodiscard]] auto getSubPassDescription() const { return mSubPassDescription; }

	private:
		VkSubpassDescription mSubPassDescription{};
		std::vector<VkAttachmentReference> mColorAttachmentRefrences{};
		std::vector<VkAttachmentReference> mInputAttachmentRefrences{};
		VkAttachmentReference mDepthStencilAttachmentRefrence{};
		VkAttachmentReference mResolvedAttachmentRefrence{};
	};

	class RenderPass
	{
	public:
		using Ptr = std::shared_ptr<RenderPass>;

		static Ptr create(const Device::Ptr& device) { return std::make_shared<RenderPass>(device); }

		RenderPass(const Device::Ptr &device);
		~RenderPass();

		void addSubPass(const SubPass& subpass);

		void addDependency(const VkSubpassDependency& dependency);

		void addAttchment(const VkAttachmentDescription& attachmentDes);

		void buildRenderPass();

	public:
		[[nodiscard]] auto getRenderPass() const { return mRenderPass; }

	private:
		VkRenderPass mRenderPass{ VK_NULL_HANDLE };

		std::vector<SubPass> mSubPasses{};
		std::vector<VkSubpassDependency> mDependencies{};
		std::vector <VkAttachmentDescription> mAttachmentDescriptions{};

		Device::Ptr mDevice{ nullptr };
	};
}