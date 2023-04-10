/**
 * @file odyssey_pipeline.h
 * @author liuyulvv (liuyulvv@outlook.com)
 * @date 2023-04-09
 */

#if !defined(ODYSSEY_ODYSSEY_PIPELINE_H_)
#define ODYSSEY_ODYSSEY_PIPELINE_H_

#include <string>
#include <vector>

namespace odyssey {

class OdysseyPipeline {
public:
    OdysseyPipeline(const std::string& vertex_shader_path, const std::string& fragment_shader_path);

    OdysseyPipeline() = delete;
    ~OdysseyPipeline() = default;
    OdysseyPipeline(const OdysseyPipeline& odyssey_pipeline) = default;
    OdysseyPipeline(OdysseyPipeline&& odyssey_pipeline) = default;
    OdysseyPipeline& operator=(const OdysseyPipeline& odyssey_pipeline) = default;
    OdysseyPipeline& operator=(OdysseyPipeline&& odyssey_pipeline) = default;

private:
    static std::vector<char> ReadFile(const std::string& path);
    void CreateGraphicsPipeline(const std::string& vertex_shader_path, const std::string& fragment_shader_path);
};

}  // namespace odyssey

#endif  // ODYSSEY_ODYSSEY_PIPELINE_H_