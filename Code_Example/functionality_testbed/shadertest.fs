#version 330

in vec2 texCoord0;
in vec3 normalsFragment;

//layout(location = 4) in vec2 texCoord0;
//layout(location = 5) in vec3 normalsFragment;

uniform sampler2D diffuse;

out vec4 fragColor;

void main()
{
    // This is temporary untill mats model gets a propper scaling done to it
    fragColor = texture(diffuse, texCoord0)*2*dot(vec3(0.0,0.0,1.0),normalsFragment);
    
    //vec4 (1.0 , 0.0, 0.0, 1.0);
    //gl_FragColor = vec4 (1.0 , 0.0, 0.0, 1.0);
}