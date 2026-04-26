#version 300 es

precision highp float; // highp mediump lowp

in vec2 textureCoords;

out vec4 outColor;

uniform sampler2D diffuseTexture;
uniform vec3 textColor;

void main()
{
    outColor = vec4(textColor, texture(diffuseTexture, textureCoords).r);
}
