#version 450

// Глобальный uniform-буфер с данными камеры и проекции (binding = 0)
layout(binding = 0) uniform UniformBufferObject {
    mat4 view;
    mat4 proj;
} ubo;

// Uniform-блок с параметрами материала (binding = 1)
// Здесь ambientFactor и sunParameters задаются на уровне материала.
layout(binding = 1) uniform MaterialBuffer {
    vec4 u_DiffuseColor;       // u_DiffuseColor.xyz – диффузный цвет, .w – padding
    float u_AmbientFactor;     // ambient-свет для материала
    vec3 pad;                  // заполнение для выравнивания
    vec4 u_SunParameters;      // u_SunParameters.xyz – направление солнечного света, u_SunParameters.w – интенсивность
} material;

// Входы от вершинного шейдера
layout(location = 0) in vec3 fragPos;
layout(location = 1) in vec3 fragNormal;
layout(location = 2) in vec2 fragTexCoord;

// Выходной цвет фрагмента
layout(location = 0) out vec4 outColor;

void main() {
    vec3 normal = normalize(fragNormal);

    // Используем параметры солнечного света из материала:
    vec3 lightDir = normalize(material.u_SunParameters.xyz);
    vec3 lightColor = vec3(1.0) * material.u_SunParameters.w;  // интенсивность света

    // Вычисляем направление взгляда (предполагается, что камера в (0,0,0))
    vec3 viewDir = normalize(-fragPos);

    // Diffuse компонент (Ламберт)
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = material.u_DiffuseColor.xyz * diff * lightColor;

    // Ambient-компонента, масштабируемая ambient-фактором материала
    vec3 ambient = material.u_AmbientFactor * material.u_DiffuseColor.xyz;

    vec3 color = ambient + diffuse;
    outColor = vec4(color, 1.0);
}
