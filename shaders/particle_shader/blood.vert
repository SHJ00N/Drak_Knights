#version 460 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aUV;
layout (location = 2) in vec4 instancePos;

layout (std140, binding = 0) uniform CameraBlock
{
    mat4 projection;
    mat4 view;
    vec3 cameraPos;
    float cameraFov;
};

out vec2 UV;
out float lifeTime;

uniform mat4 model;

void main()
{
    UV = aUV;
    lifeTime = instancePos.w;

    // particle center pos in view space
    vec4 center = view * model * vec4(instancePos.xyz, 1.0);

    float life = clamp(lifeTime, 0.0, 1.0);
    float t = pow(1.0 - life, 0.25);
    float particleSize = mix(0.08, 1.6, t);

    center.xy += aPos * particleSize;
    
    gl_Position = projection * center;
}