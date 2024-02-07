#version 430 core

uniform vec3 lightPos;
uniform sampler2D colorTexture;
uniform sampler2D normalSampler;
uniform sampler2D armTexture; // Tekstura zawieraj�ca informacje o AO, roughness i metallic

in vec2 TexCoord;
in vec3 normal;
in vec3 tangent;
in vec3 bitangent;
in vec3 viewDirTS;
in vec3 lightDirTS;

out vec4 outColor;

void main()
{
    // Wczytaj wektor normalny z tekstury i przekszta�� go do przedzia�u [-1, 1]
    vec3 N = texture(normalSampler, TexCoord).xyz * 2.0 - 1.0;
    // Znormalizuj wektor normalny
    N = normalize(N);

    // Wczytaj warto�ci AO, roughness i metallic z tekstury "armTexture"
    vec4 armData = texture(armTexture, TexCoord);
    float ao = armData.r; // Warto�� ambient occlusion
    float roughness = armData.g; // Warto�� chropowato�ci
    float metallic = armData.b; // Warto�� metaliczno�ci

    // Oblicz wektor widoku i �wiat�a w przestrzeni stycznej
    vec3 viewDir = normalize(viewDirTS);
    vec3 lightDir = normalize(lightDirTS);

    // Oblicz warto�� diffuse
    float diffuse = max(0, dot(N, lightDir));

    // Oblicz wsp�czynnik F0 (przej�ciowe odbicie) dla dielektryk�w
    vec3 F0 = vec3(0.04); // Dla wi�kszo�ci dielektryk�w warto�� ta wynosi 0.04

    // Oblicz wsp�czynnik F0 dla metali
    F0 = mix(F0, vec3(1.0), metallic);

    // Oblicz model odbicia �wiat�a (BRDF - Bidirectional Reflectance Distribution Function) dla PBR
    float NdotL = max(dot(N, lightDir), 0.0);
    float NdotV = max(dot(N, viewDir), 0.0);
    float NdotH = max(dot(N, normalize(lightDir + viewDir)), 0.0);
    float roughnessSq = roughness * roughness;

    // Oblicz funkcj� cieniowania (Schlick approximation) dla F0
    vec3 F = F0 + (1.0 - F0) * pow(1.0 - NdotH, 5.0);

    // Oblicz funkcj� cieniowania dla diffuse
    float kD = 1.0 - metallic;
    vec3 diffuseColor = (1.0 - F) * (texture(colorTexture, TexCoord).rgb * kD);

    // Oblicz funkcj� cieniowania dla specular
    vec3 specularColor = F * max(NdotL * NdotV, 0.0) * (1.0 / (3.14 * (1.0 + (roughnessSq - 1.0) * pow(NdotH, 4.0))));

    // Skaluj kolor przez warto�� ambient occlusion (AO)
    vec3 finalColor = (diffuseColor + specularColor) * (1.0 - ao);

    outColor = vec4(finalColor, 1.0);
}
