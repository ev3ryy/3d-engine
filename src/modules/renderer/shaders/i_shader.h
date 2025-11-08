#ifndef I_SHADER_H
#define I_SHADER_H

#include <vector>
#include <string>

enum class ShaderStage {
    Vertex = 0,
    Fragment,
    Compute,
    // etc
};

struct ShaderBlob {
    ShaderStage stage;
    std::vector<char> code;
    std::string entryPoint;
};

using ShaderBlobSet = std::vector<ShaderBlob>;

class IShader {
public:
    virtual ~IShader() = default;
};

#endif // I_SHADER_H