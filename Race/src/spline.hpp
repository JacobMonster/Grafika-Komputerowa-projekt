#include <glm.hpp>

glm::vec3 CalculateCatmullRomSpline(float t, const glm::vec3& Pos0, const glm::vec3& Pos1, const glm::vec3& Pos2, const glm::vec3& Pos3)
{
	float t2 = t * t;
	float t3 = t * t2;

	glm::vec4 coefficients(-0.5f * t3 + t2 - 0.5f * t,
		1.5f * t3 - 2.5f * t2 + 1.0f,
		-1.5f * t3 + 2.0f * t2 + 0.5f * t,
		0.5f * t3 - 0.5f * t2);

	glm::mat4x3 controlPointsMatrix(Pos0, Pos1, Pos2, Pos3);

	glm::vec3 result = controlPointsMatrix * coefficients;

	return result;
}

glm::vec3 CalculateCatmullRomSplineDerivative(float t, const glm::vec3& Pos0, const glm::vec3& Pos1, const glm::vec3& Pos2, const glm::vec3& Pos3)
{
	float t2 = t * t;
	glm::vec4 coefficients(-1.5f * t2 + 2.0f * t - 0.5f,
		4.5f * t2 - 5.0f * t,
		-4.5f * t2 + 4.0f * t + 0.5f,
		1.5f * t2 - 1.0f * t);

	glm::mat4x3 controlPointsMatrix(Pos0, Pos1, Pos2, Pos3);

	glm::vec3 result = controlPointsMatrix * coefficients;

	return glm::normalize(result); 
}

