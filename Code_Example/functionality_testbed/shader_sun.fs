#version 330

in vec2 texCoord0;
in vec3 normals_worldSpace;

// based on tutorials from
//https://youtu.be/csKrVBWCItc?list=PLEETnX-uPtBXT9T-hD0Bj31DSnwio-ywh
//http://ogldev.atspace.co.uk/www/tutorial17/tutorial17.html
//http://ogldev.atspace.co.uk/www/tutorial18/tutorial18.html

//layout(location = 4) in vec2 texCoord0;
//layout(location = 5) in vec3 normalsFragment;

uniform sampler2D diffuse;


uniform vec3 ambiantColor;  //sun color
uniform float ambiantIntensity; // dropoff dir , 1.0 = intetense middle , transparent edge, 0.0 = revers

uniform vec3 lightColor;    // dropoff color
uniform vec3 lightDir;      // camera diraction
uniform float lightIntensity;   // unused


out vec4 fragColor;
//magnus
void main()
{
    // This is temporary untill mats model gets a propper scaling done to it
    //vec4 ambiantLight = vec4(ambiantColor,1.0)*ambiantIntensity;
    
    vec4 sunColor =  vec4(ambiantColor,1.0);
    vec4 dropoffColor = vec4(lightColor,1.0);
    
    
    
    // directional light, we want light comming in at a step angle to get the brigther fraction of
    // the intensity then ligth comming in at a shallow angle, this calculation
    float fraction = -dot(normalize(normals_worldSpace),lightDir);
    
    //
    vec4 edgeDrop;

    
    if (ambiantIntensity > 0.5) {
        fraction = ambiantIntensity - fraction;
        fraction = fraction/(1 - fraction);
    }
    
    if (fraction > 0) {
        edgeDrop = dropoffColor*(fraction);

        
    }else
    {
        edgeDrop = vec4(1.0, 1.0, 1.0, 1.0);   // edgeDrop is used to substract the sun color,
    }
    

    fragColor = clamp((sunColor - edgeDrop),0.0,1.0);
    
    
}