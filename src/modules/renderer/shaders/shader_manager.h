#ifndef SHADER_MANAGER_H
#define SHADER_MANAGER_H

#include <shaders/shader_key.h>
#include <shaders/i_shader.h>
#include <irenderer.h>

#include <map>

class ShaderManager {
public:
    ShaderManager() = default;
    ~ShaderManager() = default;

    ShaderBlob getBinaryBlob(const ShaderKey& key);

private:
    std::map<std::string, ShaderBlob> blobCache;
};

#endif // SHADER_MANAGER_H