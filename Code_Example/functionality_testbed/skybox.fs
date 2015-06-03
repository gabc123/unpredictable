#version 330
// based on tutorial from: http://ogldev.atspace.co.uk/www/tutorial25/tutorial25.html

in vec3 texCoord0;

out vec4 fragColor;

uniform samplerCube cubeMapTextureData;
//walle
void main()
{
    fragColor = texture(cubeMapTextureData, texCoord0);
}