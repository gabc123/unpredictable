#version 330

in vec2 texCoord0;
in vec3 normals_worldSpace;

//layout(location = 4) in vec2 texCoord0;
//layout(location = 5) in vec3 normalsFragment;

uniform sampler2D diffuse;


uniform vec3 ambiantColor;
uniform float ambiantIntensity;

uniform vec3 lightColor;
uniform vec3 lightDir;
uniform float lightIntensity;


out vec4 fragColor;

void main()
{
    // This is temporary untill mats model gets a propper scaling done to it
    vec4 ambiantLight = vec4(ambiantColor,0.0) * ambiantIntensity;
    ambiantLight = vec4(ambiantLight.xyz,1.0);
    
    // directional light, we want light comming in at a step angle to get the brigther fraction of
    // the intensity then ligth comming in at a shallow angle, this calculation
    float lightFraction = dot(normalize(normals_worldSpace),-lightDir);
    
    //
    vec4 totaldirectional_light;
    
    if (lightFraction > 0) {
        totaldirectional_light = vec4(lightColor,1.0) * lightIntensity * lightFraction;
    }else
    {
        totaldirectional_light = vec4(0.0, 0.0, 0.0, 0.0);
    }
    
    
    fragColor = texture(diffuse, texCoord0)*clamp((totaldirectional_light + ambiantLight),0.0,1.0);
    
    
}