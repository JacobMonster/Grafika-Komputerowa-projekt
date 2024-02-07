#version 430 core

uniform sampler2D moonTexture;
uniform sampler2D waterTexture;
uniform sampler2D sandTexture;
uniform sampler2D heightmapTexture;

in vec2 TexCoord;

out vec4 FragColor;

vec3 blend(vec3 texture1, float height1, vec3 texture2, float height2, float blend_ratio)
{
    float mix_threshold = 0.1;
    float h1 = height1 + 1.0 - blend_ratio;
    float h2 = height2 + blend_ratio;
    float havg = (h1 + h2) / 2.0;
	
    h1 = clamp((h1 - havg + 0.5 * mix_threshold) / (mix_threshold), 0.0, 1.0);
    h2 = clamp((h2 - havg + 0.5 * mix_threshold) / (mix_threshold), 0.0, 1.0);

    return (texture1 * h1 + texture2 * h2);
}

void main()
{
    // Wczytaj wysokoœæ z mapy wysokoœci
    float height = texture(heightmapTexture, TexCoord).r;

    // Tekstury
    vec3 moonColor = texture(moonTexture, TexCoord).rgb;
    vec3 waterColor = texture(waterTexture, TexCoord).rgb;
    vec3 sandColor = texture(sandTexture, TexCoord).rgb;

    // Mieszanie tekstur w zale¿noœci od wysokoœci
    vec3 finalColor;
    if (height < 0.3)
    {
        finalColor = blend(waterColor, 0.0, moonColor, 0.3, height);
    }
    else if (height >= 0.3 && height < 0.6)
    {
        finalColor = blend(moonColor, 0.3, sandColor, 0.6, height);
    }
    else
    {
        finalColor = blend(sandColor, 0.6, vec3(1.0), 1.0, height);
    }

    FragColor = vec4(finalColor, 1.0);
}
