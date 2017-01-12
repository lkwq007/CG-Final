#version 330 core

in vec3 Position;
in vec3 Normal;
in vec2 TexCoords;

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

uniform vec3 lightPos;
uniform sampler2D texture_diffuse1;

void main()
{   
    float ambientStrength = 0.1f;
    vec3 objectColor = vec3(1.0f, 1.0f, 1.0f);
    vec3 lightColor = vec3(1.0f, 1.0f, 1.0f);
    vec3 ambient = ambientStrength * lightColor;
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - Position);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
    vec3 result = (ambient + diffuse) * objectColor;
	FragColor = texture(texture_diffuse1, TexCoords);
	float brightness = dot(result, vec3(0.2126, 0.7152, 0.0722));
	if(brightness > 1.0)
		BrightColor = vec4(result, 1.0);
	else
		BrightColor = vec4(0.0f, 0.0f, 0.0f, 1.0f);
}