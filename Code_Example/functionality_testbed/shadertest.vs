#version 330

layout(location = 0)in vec3 position;
layout(location = 1)in vec2 texCoord;
layout(location = 2)in vec3 normals;

// based on tutorials from
//https://youtu.be/csKrVBWCItc?list=PLEETnX-uPtBXT9T-hD0Bj31DSnwio-ywh
//http://ogldev.atspace.co.uk/www/tutorial17/tutorial17.html
//http://ogldev.atspace.co.uk/www/tutorial18/tutorial18.html

//layout(location = 4) out vec2 texCoord0;
//layout(location = 5) out vec3 normalsFragment;

out vec2 texCoord0;
out vec3 normals_worldSpace;

uniform mat4 mvp;
uniform mat4 model;
uniform mat4 light_sun;
// magnus
void main()
{
    gl_Position = mvp*vec4(position,1.0) ;
    texCoord0 = texCoord;
    normals_worldSpace = (model*vec4(normals,0.0)).xyz;
}
