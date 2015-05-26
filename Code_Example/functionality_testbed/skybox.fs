#version 330


in vec3 texCoord0;

out vec4 fragColor;

uniform samplerCube cubeMapTextureData;

void main()
{
    fragColor = texture(cubeMapTextureData, texCoord0);
}