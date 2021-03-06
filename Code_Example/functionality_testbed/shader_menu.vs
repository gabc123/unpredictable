#version 330

layout(location = 0)in vec3 position;
layout(location = 1)in vec2 texCoord;
layout(location = 2)in vec3 normals;

out vec2 texCoord0;
out vec3 normalsFragment;

uniform mat4 mvp;

void main()
{
    gl_Position = mvp * vec4(position,1.0);
    texCoord0 = texCoord;
    //normalsFragment = (mvp * vec4(normals,0.1)).xyz;
}