#version 330

// based on tutorials from
//https://youtu.be/csKrVBWCItc?list=PLEETnX-uPtBXT9T-hD0Bj31DSnwio-ywh
//http://ogldev.atspace.co.uk/www/tutorial17/tutorial17.html
//http://ogldev.atspace.co.uk/www/tutorial18/tutorial18.html

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