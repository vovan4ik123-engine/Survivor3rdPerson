#version 300 es

precision highp float; // highp mediump lowp

in float currentXPos;

out vec4 outColor;

uniform vec3 colorLeft;
uniform vec3 colorRight;
uniform float progressXPos;

void main()
{
    if(currentXPos < progressXPos)
    {
        outColor = vec4(colorLeft, 1.0f);
    }
    else
    {
        outColor = vec4(colorRight, 1.0f);
    }
}
