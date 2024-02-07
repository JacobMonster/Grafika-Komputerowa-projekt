#version 430 core

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexNormal;
layout(location = 2) in vec2 vertexTexCoord;
layout(location = 3) in vec3 vertexTangent;
layout(location = 4) in vec3 vertexBitangent;

uniform mat4 transformation;
uniform mat4 modelMatrix;
uniform vec3 cameraPos;
uniform vec3 lightPos;

out vec2 TexCoord;

out vec3 normal;
out vec3 tangent;
out vec3 bitangent;
out vec3 viewDirTS;
out vec3 lightDirTS;

void main()
{
    TexCoord = vertexTexCoord;

    // Transformacja pozycji i normalnej do przestrzeni œwiata
    vec4 worldPosVec4 = modelMatrix * vec4(vertexPosition, 1.0);
    vec4 worldNormalVec4 = modelMatrix * vec4(vertexNormal, 0.0);
    vec4 worldTangentVec4 = modelMatrix * vec4(vertexTangent, 0.0);
    vec4 worldBitangentVec4 = modelMatrix * vec4(vertexBitangent, 0.0);

    normal = normalize(worldNormalVec4.xyz);
    tangent = normalize(worldTangentVec4.xyz);
    bitangent = normalize(worldBitangentVec4.xyz);

    // Tworzenie macierzy TBN
    mat3 TBN = transpose(mat3(tangent, bitangent, normal));

    // Obliczenie wektora viewDirTS i lightDirTS w przestrzeni stycznej
    vec3 viewDir = normalize(cameraPos - worldPosVec4.xyz);
    viewDirTS = TBN * viewDir;

    vec3 lightDir = normalize(lightPos - worldPosVec4.xyz);
    lightDirTS = TBN * lightDir;

    // Przypisanie wartoœci do gl_Position
    gl_Position = transformation * vec4(vertexPosition, 1.0);
}
