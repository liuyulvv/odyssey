/**
 * @file odyssey_render_system.cpp
 * @author liuyulvv (liuyulvv@outlook.com)
 * @date 2023-04-21
 */

#include "odyssey_render_system.h"

#include "odyssey_device.h"

namespace odyssey {

OdysseyRenderSystem::OdysseyRenderSystem(OdysseyDevice* device, vk::RenderPass renderPass) : m_device(device) {
    createPipelineLayout();
    m_pipelineLine = createPipeline("shaders/shader.vert.spv", "shaders/shader.frag.spv", vk::PrimitiveTopology::eTriangleList, 1.0F, renderPass);
}

OdysseyRenderSystem::~OdysseyRenderSystem() {
    m_device->device().destroyPipelineLayout(m_pipelineLayout);
    m_pipelineLine.reset();
}

void OdysseyRenderSystem::renderObjects(vk::CommandBuffer commandBuffer, std::vector<OdysseyObject>& objects, OdysseyCamera* camera) {
    m_pipelineLine->bind(commandBuffer);
    auto projectionView = camera->getProjection() * camera->getView();
    for (auto& object : objects) {
        PushConstantData push{};
        auto model = object.transform.mat4();
        push.transform = projectionView * model;
        push.normal = object.transform.normal();
        commandBuffer.pushConstants<PushConstantData>(m_pipelineLayout, vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment, 0, push);
        object.model->bind(commandBuffer);
        object.model->draw(commandBuffer);
    }
}

void OdysseyRenderSystem::createPipelineLayout() {
    vk::PushConstantRange pushConstantRange{};
    pushConstantRange
        .setStageFlags(vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment)
        .setOffset(0)
        .setSize(sizeof(PushConstantData));

    vk::PipelineLayoutCreateInfo pipelineInfo{};
    pipelineInfo
        .setSetLayoutCount(0)
        .setPSetLayouts(nullptr)
        .setPushConstantRangeCount(1)
        .setPushConstantRanges(pushConstantRange);
    m_pipelineLayout = m_device->device().createPipelineLayout(pipelineInfo);
}

std::unique_ptr<OdysseyPipeline> OdysseyRenderSystem::createPipeline(const std::string& vertShaderPath, const std::string& fragShaderPath, vk::PrimitiveTopology primitiveTopology, float lineWidth, vk::RenderPass renderPass) {
    auto pipelineConfig = OdysseyPipeline::defaultPipelineConfigInfo(primitiveTopology, lineWidth);
    pipelineConfig.renderPass = renderPass;
    pipelineConfig.pipelineLayout = m_pipelineLayout;
    return std::make_unique<OdysseyPipeline>(m_device, vertShaderPath, fragShaderPath, pipelineConfig);
}

}  // namespace odyssey