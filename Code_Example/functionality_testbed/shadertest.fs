#version 330

in vec2 texCoord0;
uniform sampler2D diffuse;

out vec4 fragColor;

void main()
{
    fragColor = texture(diffuse, texCoord0);//vec4 (1.0 , 0.0, 0.0, 1.0);
    //gl_FragColor = vec4 (1.0 , 0.0, 0.0, 1.0);
}