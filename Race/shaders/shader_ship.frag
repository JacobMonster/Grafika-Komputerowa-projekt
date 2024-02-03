#version 430 core

float AMBIENT = 0.1;

uniform vec3 color;
uniform vec3 lightPos;
uniform sampler2D colorTexture;
uniform sampler2D normalSampler; // Dodany sampler do czytania mapy normalnych

in vec2 TexCoord;
in vec3 normal;
in vec3 tangent;
in vec3 bitangent;
in vec3 viewDirTS;
in vec3 lightDirTS;

out vec4 outColor;

void main()
{
    // Wczytaj wektor normalny z tekstury i przekszta³æ go do przedzia³u [-1, 1]
    vec3 N = texture(normalSampler, vec2(TexCoord.x, 1.0 - TexCoord.y)).xyz * 2.0 - 1.0;
    // Znormalizuj wektor normalny
    N = normalize(N);

    // Oblicz wektor widoku i œwiat³a w przestrzeni stycznej
    vec3 viewDir = normalize(viewDirTS);
    vec3 lightDir = normalize(lightDirTS);


    // Oblicz wartoœæ diffuse
    float diffuse = max(0, dot(N, lightDir));

    // Oblicz kolor z tekstury
    vec4 textureColor = texture(colorTexture, vec2(TexCoord.x, 1.0 - TexCoord.y));

    // Oblicz ostateczny kolor
     outColor = vec4(textureColor.rgb * min(1, AMBIENT + diffuse), 1.0);

}
