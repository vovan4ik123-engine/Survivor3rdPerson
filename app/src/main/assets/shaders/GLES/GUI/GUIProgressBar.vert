#version 300 es

precision highp float; // highp mediump lowp

layout(location = 0) in vec3 inPosition;

out float currentXPos;

uniform mat4 VPMatrix;

void main()
{
    currentXPos = inPosition.x;
    gl_Position = VPMatrix * vec4(inPosition, 1.0f);
}
