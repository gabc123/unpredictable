#version 330

layout(location = 0)in vec3 position;
layout(location = 1)in vec2 texCoord;
layout(location = 2)in vec3 normals;

out vec2 texCoord0;
out vec3 normalsFragment;

uniform mat4 transform;
uniform mat4 light_sun;

void main()
{
    gl_Position = transform * vec4(position,1.0);
    texCoord0 = texCoord;
    normalsFragment = (light_sun * vec4(normals,0.0)).xyz;
}