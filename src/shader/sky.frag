#version 330 core
in vec3 TexCoords;
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

uniform samplerCube skybox;

void main()
{
    FragColor = texture(skybox, TexCoords);
    vec3 result=vec3(FragColor);
    float brightness = dot(result, vec3(0.2126, 0.7152, 0.0722));
	if(brightness > 1.0)
		BrightColor = vec4(result, 1.0);
	else
		BrightColor = vec4(0.0f, 0.0f, 0.0f, 1.0f);
}