#version 430 core

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec2 vertexTexCoord;
layout(location = 2) in vec3 vertexNormal;

out vec2 TexCoord;
out float Height;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    // Przypisanie pozycji wierzcho³ka
    gl_Position = projection * view * model * vec4(vertexPosition, 1.0);
    
    // Przekazanie wspó³rzêdnych tekstury do fragment shadera
    TexCoord = vertexTexCoord;
    
    // Przekazanie wysokoœci wierzcho³ka do fragment shadera
    Height = vertexPosition.y;
}
