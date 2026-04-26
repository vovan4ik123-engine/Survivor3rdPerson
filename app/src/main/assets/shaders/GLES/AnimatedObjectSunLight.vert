#version 300 es

precision highp float; // highp mediump lowp

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTextureCoords;
layout(location = 3) in ivec4 inBoneIDs; // INT pointer here
layout(location = 4) in vec4 inWeights;

out vec2 textureCoords;
out vec3 normal;

const int MAX_BONES = 31; // 31 bones should be enough for most models

uniform mat4 bonesMatrices[MAX_BONES];
uniform mat4 MVPMatrix;
uniform mat3 normalMatrix;

void main()
{
    mat4 boneTransf;

    boneTransf[0][0] = bonesMatrices[inBoneIDs[0]][0][0] * inWeights[0];
    boneTransf[0][1] = bonesMatrices[inBoneIDs[0]][0][1] * inWeights[0];
    boneTransf[0][2] = bonesMatrices[inBoneIDs[0]][0][2] * inWeights[0];

    boneTransf[1][0] = bonesMatrices[inBoneIDs[0]][1][0] * inWeights[0];
    boneTransf[1][1] = bonesMatrices[inBoneIDs[0]][1][1] * inWeights[0];
    boneTransf[1][2] = bonesMatrices[inBoneIDs[0]][1][2] * inWeights[0];

    boneTransf[2][0] = bonesMatrices[inBoneIDs[0]][2][0] * inWeights[0];
    boneTransf[2][1] = bonesMatrices[inBoneIDs[0]][2][1] * inWeights[0];
    boneTransf[2][2] = bonesMatrices[inBoneIDs[0]][2][2] * inWeights[0];

    boneTransf[3][0] = bonesMatrices[inBoneIDs[0]][3][0] * inWeights[0];
    boneTransf[3][1] = bonesMatrices[inBoneIDs[0]][3][1] * inWeights[0];
    boneTransf[3][2] = bonesMatrices[inBoneIDs[0]][3][2] * inWeights[0];

    boneTransf[3][3] = bonesMatrices[inBoneIDs[0]][3][3] * inWeights[0];

    textureCoords = inTextureCoords;
    normal = normalMatrix * mat3(boneTransf) * inNormal;

    gl_Position = MVPMatrix * boneTransf * vec4(inPosition, 1.0f);
}
