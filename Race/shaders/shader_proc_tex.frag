#version 430 core

float AMBIENT = 0.1;

uniform vec3 color;
uniform vec3 lightPos;

in vec3 vecNormal;
in vec3 worldPos;

in vec2 TexCoord;

uniform sampler2D colorTexture;

in vec3 localPosition;

uniform vec3 color1;
uniform vec3 color2;

out vec4 outColor;
void main()
{
	float sinusY = sin(localPosition.y);

	vec3 finalColor;
	if (sinusY > 0.0) {
		outColor = vec4(color1, 1.0);
	} else {
		outColor = vec4(color2, 1.0);
	}

}
