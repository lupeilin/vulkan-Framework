#include "renderPass.h"

namespace FF::Wrapper {
	
	SubPass::SubPass() {

	}
	SubPass::~SubPass() {

	}
	void SubPass::addColorAttachmentRefrence(const VkAttachmentReference& ref) {
		mColorAttachmentRefrences.push_back(ref);
	}

	void SubPass::addInputAttachmentRefrence(const VkAttachmentReference& ref) {
		mInputAttachmentRefrences.push_back(ref);
	}

	void SubPass::addDepthStencilAttachmentRefrence(const VkAttachmentReference& ref) {
		mDepthStencilAttachmentRefrence = ref;
	}

	void SubPass::buildSubpassDescription() {
		if (mColorAttachmentRefrences.empty()) {
			throw std::runtime_error("Error : color attachment group is empty");
		}
		mSubPassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;

		mSubPassDescription.colorAttachmentCount = static_cast<uint32_t>(mColorAttachmentRefrences.size());
		mSubPassDescription.pColorAttachments = mColorAttachmentRefrences.data();

		mSubPassDescription.inputAttachmentCount = static_cast<uint32_t>(mInputAttachmentRefrences.size());
		mSubPassDescription.pInputAttachments = mInputAttachmentRefrences.data();

		mSubPassDescription.pDepthStencilAttachment = mDepthStencilAttachmentRefrence.layout == VK_IMAGE_LAYOUT_UNDEFINED ? nullptr : &mDepthStencilAttachmentRefrence;
	}

	RenderPass::RenderPass(const Device::Ptr& device) {
		mDevice = device;
	}

	RenderPass::~RenderPass() {
		if (mRenderPass != VK_NULL_HANDLE) {
			vkDestroyRenderPass(mDevice->getDevice(), mRenderPass, nullptr);
		}
	}

	void RenderPass::addSubPass(const SubPass& subpass) { mSubPasses.push_back(subpass); }

	void RenderPass::addDependency(const VkSubpassDependency& dependency) { mDependencies.push_back(dependency); }

	void RenderPass::addAttchment(const VkAttachmentDescription& attachmentDes) { mAttachmentDescriptions.push_back(attachmentDes); }

	void RenderPass::buildRenderPass() {
		if (mSubPasses.empty() || mAttachmentDescriptions.empty() || mDependencies.empty()) {
			throw std::runtime_error("Error : not enough elements to build renderPass");
		}

		//°Ñsubpass ½â°ü unwrap
		std::vector<VkSubpassDescription> subPasses{};
		for (int i = 0; i < mSubPasses.size(); ++i) {
			subPasses.push_back(mSubPasses[i].getSubPassDescription());
		}

		VkRenderPassCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;

		createInfo.attachmentCount = static_cast<uint32_t>(mAttachmentDescriptions.size());
		createInfo.pAttachments = mAttachmentDescriptions.data();

		createInfo.dependencyCount = static_cast<uint32_t>(mDependencies.size());
		createInfo.pDependencies = mDependencies.data();

		createInfo.subpassCount = static_cast<uint32_t>(subPasses.size());
		createInfo.pSubpasses = subPasses.data();

		if (vkCreateRenderPass(mDevice->getDevice(), &createInfo, nullptr, &mRenderPass) != VK_SUCCESS) {
			throw std::runtime_error("Error : failed to create renderPass");
		}
	}
}