#version 430 core

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec2 vertexTexCoord;

uniform mat4 transformation;
uniform mat4 modelMatrix;

out vec2 TexCoord;

void main()
{
    // Transformacja pozycji wierzcho³ka
    gl_Position = transformation * modelMatrix * vec4(vertexPosition, 1.0);

    // Przekazanie wspó³rzêdnych tekstury do fragment shadera
    TexCoord = vertexTexCoord;
}
