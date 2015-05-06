#version 330

layout(location = 0)in vec3 position;
layout(location = 1)in vec2 texCoord;
layout(location = 2)in vec3 normals;

//layout(location = 4) out vec2 texCoord0;
//layout(location = 5) out vec3 normalsFragment;

out vec2 texCoord0;
out vec3 normals_worldSpace;

uniform mat4 mvp;
uniform mat4 model;
uniform mat4 light_sun;

void main()
{
    gl_Position = mvp*vec4(position,1.0) ;
    texCoord0 = texCoord;
    normals_worldSpace = (model*vec4(normals,0.0)).xyz;
}
