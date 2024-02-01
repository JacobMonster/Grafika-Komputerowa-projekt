#version 430 core

float AMBIENT = 0.1;

uniform vec3 color;
uniform vec3 lightPos;

in vec3 vecNormal;
in vec3 worldPos;

in vec2 TexCoord;

uniform sampler2D colorTexture;

out vec4 outColor;
void main()
{
	vec3 lightDir = normalize(lightPos-worldPos);
	vec3 normal = normalize(vecNormal);
	float diffuse=max(0,dot(normal,lightDir));
	outColor = vec4(color*min(1,AMBIENT+diffuse), 1.0);

	vec4 textureColor = texture2D(colorTexture, vec2(TexCoord.x, 1.0 - TexCoord.y));


	outColor.rgb = textureColor.rgb;

	outColor = vec4(textureColor.rgb * min(1, AMBIENT + diffuse), 1.0);
}
