#version 330

in vec2 texCoord0;
in vec3 normalsFragment;

uniform sampler2D diffuse;
uniform vec3 ambiantColor;

out vec4 fragColor;

void main()
{
    fragColor = clamp(texture(diffuse, texCoord0) + vec4(ambiantColor,0.0),0.0,1.0);//*clamp(dot(vec3(0.0,0.0,0.5),normalsFragment),0,1);
    
    //vec4 (1.0 , 0.0, 0.0, 1.0);
    //gl_FragColor = vec4 (1.0 , 0.0, 0.0, 1.0);
}