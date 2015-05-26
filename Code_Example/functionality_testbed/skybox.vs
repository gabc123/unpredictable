#version 330

//builds on the tutorial : http://ogldev.atspace.co.uk/www/tutorial25/tutorial25.html
layout(location = 0)in vec3 position;

out vec3 texCoord0;

uniform mat4 mvp;

void main()
{
    vec4 pos_world = mvp*vec4(position,1.0) ;
    gl_Position =pos_world.xyww;
    texCoord0 = position;
}
