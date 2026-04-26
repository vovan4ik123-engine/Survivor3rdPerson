#version 300 es

precision highp float; // highp mediump lowp

in vec2 textureCoords;

out vec4 outColor;

uniform sampler2D diffuseTexture;
uniform sampler2D diffuseTextureMat2;
uniform sampler2D blendTextureMat2;

uniform float addToUVCoords; // Add this to UV,
uniform float UVCoordsMultiplier; // next multiply UV to return them in 0...1 range if was scaled.

void main()
{
    // Return UV coords into range 0...1.
    vec2 blendTextureUV = (textureCoords + addToUVCoords) * UVCoordsMultiplier;
    float blendFactor = texture(blendTextureMat2,  blendTextureUV).r;

    vec4 text1 = texture(diffuseTexture, textureCoords) * blendFactor;
    vec4 text2 = texture(diffuseTextureMat2, textureCoords) * (1.0f - blendFactor);

    outColor = text1 + text2;
}
