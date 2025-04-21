#version 450

// Глобальный uniform-буфер с данными камеры и проекции (binding = 0)
layout(binding = 0) uniform UniformBufferObject {
    mat4 view;
    mat4 proj;
} ubo;

// Push constants: модельная матрица
layout(push_constant) uniform PushConstants {
    mat4 model;
} pushConstants;

// Входные атрибуты
layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;

// Выходы для фрагментного шейдера
layout(location = 0) out vec3 fragPos;
layout(location = 1) out vec3 fragNormal;
layout(location = 2) out vec2 fragTexCoord;

void main() {
    mat4 model = pushConstants.model;
    vec4 worldPos = model * vec4(inPosition, 1.0);
    fragPos = worldPos.xyz;
    // Правильное преобразование нормалей (с инверсией и транспонированием матрицы)
    fragNormal = mat3(transpose(inverse(model))) * inNormal;
    fragTexCoord = inTexCoord;
    gl_Position = ubo.proj * ubo.view * worldPos;
}
