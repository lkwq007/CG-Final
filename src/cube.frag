#version 330 core

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoord;

uniform vec3 lightPos;
uniform sampler2D cubeTexture;

out vec4 color;

void main()
{
    // 环境
    float ambientStrength = 0.1f;
    vec3 objectColor = vec3(1.0f, 1.0f, 1.0f);
    vec3 lightColor = vec3(1.0f, 1.0f, 1.0f);
    vec3 ambient = ambientStrength * lightColor;
    // 法线
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
    vec3 result = (ambient + diffuse) * objectColor;
    //color = vec4(result, 1.0f);
    color = texture(cubeTexture, TexCoord) * vec4(result, 1.0f);
}