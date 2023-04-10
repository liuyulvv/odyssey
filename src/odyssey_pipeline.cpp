/**
 * @file odyssey_pipeline.cpp
 * @author liuyulvv (liuyulvv@outlook.com)
 * @date 2023-04-09
 */

#include "odyssey_pipeline.h"

#include <cstring>
#include <fstream>
#include <iostream>
#include <stdexcept>

#include "fmt/format.h"

namespace odyssey {

OdysseyPipeline::OdysseyPipeline(const std::string& vertex_shader_path, const std::string& fragment_shader_path) {
    CreateGraphicsPipeline(vertex_shader_path, fragment_shader_path);
}

std::vector<char> OdysseyPipeline::ReadFile(const std::string& path) {
    std::ifstream file(path, std::ios::ate | std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error(fmt::format("Failed to open file: {}.", path));
    }
    auto file_size = static_cast<std::streamsize>(file.tellg());
    std::vector<char> buffer(file_size);
    file.seekg(0);
    file.read(buffer.data(), file_size);
    file.close();
    return buffer;
}

void OdysseyPipeline::CreateGraphicsPipeline(const std::string& vertex_shader_path, const std::string& fragment_shader_path) {
    auto vertex_shader_code = ReadFile(vertex_shader_path);
    auto fragment_shader_code = ReadFile(fragment_shader_path);
    std::cout << vertex_shader_code.size() << " " << fragment_shader_code.size() << std::endl;
}

}  // namespace odyssey