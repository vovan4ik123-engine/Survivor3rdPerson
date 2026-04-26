#version 300 es

precision highp float; // highp mediump lowp

in vec2 textureCoords;
in vec3 normal;
in vec3 fragPos;

out vec4 outColor;

uniform sampler2D diffuseTexture;

uniform vec3 sunLightDir;
uniform vec3 cameraPos;

uniform float ambientLight;
uniform float specularLightStrength;

void main()
{
    // diffuse
    vec3 norm = normalize(normal);
    float diffuse = max(dot(norm, -sunLightDir), 0.0f);

    // specular
    vec3 fragToCameraDir = normalize(cameraPos - fragPos);
    vec3 reflectDir = reflect(sunLightDir, norm);
    float specular = pow(max(dot(fragToCameraDir, reflectDir), 0.0f), 64.0f) * specularLightStrength; // 64.0f <- bigger number = smaller light dot

    vec3 textureCollor = texture(diffuseTexture, textureCoords).rgb;
    outColor = vec4((ambientLight + diffuse + specular) * textureCollor, 1.0f);
}
