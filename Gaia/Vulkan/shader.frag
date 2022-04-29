#version 450

layout(binding = 1) uniform sampler2D texSampler;

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

float linearToSrgb(float value) {
    const float inv_2_4 = 0.41666666666;
    return value <= 0.0031308
       ? value * 12.92 
       : 1.055 * pow(value, inv_2_4) - 0.055;
}

vec3 vec3ToSrgb(vec3 value) {
    return vec3(linearToSrgb(value.x), linearToSrgb(value.y), linearToSrgb(value.z));
}

void main() {

    outColor = vec4(vec3ToSrgb(texture(texSampler, fragTexCoord).rgb), 1.0);
    //outColor = vec4(fragTexCoord, 0.0, 1.0);
}