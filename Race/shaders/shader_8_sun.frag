#version 430 core

uniform sampler2D sunTexture;

in vec2 TexCoord;

out vec4 outColor;

void main()
{
    // Pobierz kolor z tekstury s³oñca
    vec4 sunColor = texture(sunTexture, vec2(TexCoord.x, 1.0 - TexCoord.y));


    // Ustaw wynikowy kolor
    outColor = vec4(sunColor.rgb, 1.0);
}
