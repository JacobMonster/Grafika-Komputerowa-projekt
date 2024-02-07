#include "glew.h"
#include <GLFW/glfw3.h>
#include "glm.hpp"
#include "ext.hpp"
#include <iostream>
#include <cmath>
#include "./SOIL/SOIL.h"

#include "Shader_Loader.h"
#include "Render_Utils.h"
#include "Texture.h"

#include "Box.cpp"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <string>

#include <thread>
#include <chrono>

#include "spline.hpp"


namespace texture {
	GLuint earth;
	GLuint earth2;
	GLuint clouds;
	GLuint moon;
	GLuint ship;
	GLuint grid;
	GLuint spaceship;
	GLuint sun;
	GLuint mars;

	//normals
	GLuint earth_normal;
	GLuint earth2_normal;
	GLuint spaceship_normal;
	GLuint moon_normal;
	GLuint earthNormal;
	GLuint asteroidNormal;
	GLuint shipNormal;

	//new
	GLuint brain;
	GLuint brain_normal;
	GLuint tiles;
	GLuint tiles_normal;
	GLuint brick;
	GLuint brick_normal;
	GLuint stone;
	GLuint stone_normal;
	GLuint gold;
	GLuint gold_normal;
	GLuint rock;
	GLuint rock_normal;
	GLuint rock2;
	GLuint rock2_normal;
	GLuint rope;
	GLuint rope_normal;
	GLuint sand;
	GLuint sand_normal;
	GLuint water;
	GLuint water_normal;
	GLuint grass;
	GLuint grass_normal;
	GLuint mud;
	GLuint mud_normal;
	GLuint moss;
	GLuint moss_normal;
	GLuint ground;
	GLuint ground_normal;
}

GLuint program;
GLuint programSun;
GLuint programTex;
GLuint programSkyBox;
GLuint programEarth;
GLuint programProcTex;
GLuint cubemapTexture;
GLuint programTexNormal;
GLuint programShip;

Core::Shader_Loader shaderLoader;

Core::RenderContext shipContext;
Core::RenderContext sphereContext;
Core::RenderContext cubeContext;
Core::RenderContext checkpointContext;
Core::RenderContext place1Context;
Core::RenderContext place2Context;
Core::RenderContext place3Context;
Core::RenderContext youContext;
Core::RenderContext arrowContext;

// Checkpoint positions
const int numCheckpoints = 14;
std::vector<glm::vec3> checkpointPos = {
	glm::vec3(0.0f, 1.0f, 4.0f),
	glm::vec3(-5.5f, 2.0f, 12.5f),
	glm::vec3(-10.0f, 3.0f, 20.5f),
	glm::vec3(-5.0f, 1.0f, 26.0f),
	glm::vec3(2.0f, -1.0f, 20.0f),
	glm::vec3(1.0f, 0.0f, 13.0f),
	glm::vec3(3.0f, -1.0f, 3.0f),
	glm::vec3(0.0f, 1.0f, -6.0f),
	glm::vec3(-2.0f, 3.0f, -12.0f),
	glm::vec3(-5.0f, 2.0f, -10.0f),
 	glm::vec3(-9.0f, 3.0f, -5.0f),
	glm::vec3(-11.0f, 1.0f, 2.0f),
	glm::vec3(-6.0f, 0.0f, 8.0f),
	glm::vec3(0.5f, -1.0f, 8.0f),
	// Additional
	glm::vec3(1.0f, -1.0f, 8.0f),
	glm::vec3(2.0f, -1.0f, 8.0f)

	//glm::vec3(0.0f, 1.0f, 4.0f),
	//glm::vec3(-5.5f, 1.0f, 12.5f),
	//glm::vec3(-6.5f, 1.0f, 18.5f),
	//glm::vec3(-3.5f, 1.0f, 22.5f),
	//glm::vec3(-0.5f, 1.0f, 18.5f),
	//// Additional
	//glm::vec3(-1.5f, 1.0f, 13.0f),
	//glm::vec3(-1.5f, 1.0f, 10.0f)

};

glm::vec3 checkpos = glm::vec3(0.0f, 0.0f, 1.0f);

int currentCheckpointIndex = 0;

bool reachedCheckpoint = false;

std::vector<glm::vec3> colors = {
	glm::vec3(1.0f, 1.0f, 0.0f),
	glm::vec3(1.0f, 0.0f, 0.0f)
};

int colorIndex = 0;

glm::vec3 Bot1Spawn = glm::vec3(2.0f, 1.0f, -1.0f);
glm::vec3 Bot2Spawn = glm::vec3(-2.0f, 1.0f, -1.0f);

bool Bot1End = false;
bool Bot2End = false;
bool Bot1Endc = false;
bool Bot2Endc = false;
bool PlayerEnd = false;
float raceStartTime = 0.0f;
float deltaTime;
float lastFrameTime = 0.f;

glm::vec3 cameraPos = glm::vec3(-4.f, 0, 0);
glm::vec3 cameraDir = glm::vec3(1.f, 0.f, 0.f);


glm::vec3 spaceshipPos = glm::vec3(0.0f, 1.000000f, -5.0f);
glm::vec3 spaceshipDir = glm::vec3(0.0f, 0.000000f, 1.0f);

glm::vec3 Bot1Pos = glm::vec3(2.0f, 1.0f, -2.0f);
glm::vec3 Bot2Pos = glm::vec3(-2.0f, 1.0f, -2.0f);
glm::vec3 Bot1Dir = glm::normalize(glm::vec3(0.354510f, 0.000000f, 0.935054f));
glm::vec3 Bot2Dir = glm::normalize(glm::vec3(-0.354510f, 0.000000f, 0.935054f));

glm::vec3 youPos = glm::vec3(-100.0f, -100.0f, -100.0f);
glm::vec3 youDir = glm::vec3(0.0f, 0.2f, -1.0f);

//glm::vec3 checkpointDir = glm::vec3(0.0f, 1.0f, 0.0f);

bool checkpointReached(int index, glm::vec3 playerPos) {
	if (playerPos.x >= checkpointPos[index].x - 1.0f && playerPos.x <= checkpointPos[index].x + 1.0f &&
		playerPos.y >= checkpointPos[index].y - 2.25f && playerPos.y <= checkpointPos[index].y + 1.0f &&
		playerPos.z >= checkpointPos[index].z - 0.1f && playerPos.z <= checkpointPos[index].z + 0.1f)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool checkpointReached2(int index, glm::vec3 playerPos, const glm::mat4& checkpointRotationMatrix) {
	// Obliczamy ró¿nicê miêdzy pozycj¹ gracza a pozycj¹ checkpointu
	glm::vec3 diff = playerPos - checkpointPos[index];

	// Przekszta³camy ró¿nicê do uk³adu wspó³rzêdnych checkpointu przez pomno¿enie przez odwrotnoœæ macierzy rotacji
	glm::vec3 transformedDiff = glm::inverse(checkpointRotationMatrix) * glm::vec4(diff, 1.0f);

	// Sprawdzamy, czy przetransformowana ró¿nica mieœci siê w zakresie odleg³oœci od checkpointu
	if (transformedDiff.x >= -1.0f && transformedDiff.x <= 1.0f &&
		transformedDiff.y >= -2.25f && transformedDiff.y <= 1.0f &&
		transformedDiff.z >= -0.1f && transformedDiff.z <= 0.1f) {
		return true;
	}
	else {
		return false;
	}
}

glm::mat4 BotCameraRotationMatrix(glm::vec3 BotDir) {
	glm::vec3 BotSide = glm::normalize(glm::cross(BotDir, glm::vec3(0.f, 1.f, 0.f)));
	glm::vec3 BotUp = glm::normalize(glm::cross(BotSide, BotDir));
	glm::mat4 BotCameraRotationMatrix = glm::mat4({
		BotSide.x,BotSide.y,BotSide.z,0,
		BotUp.x,BotUp.y,BotUp.z ,0,
		-BotDir.x,-BotDir.y,-BotDir.z,0,
		0.,0.,0.,1.,
		});

	return BotCameraRotationMatrix;
}

glm::mat4 calculateCheckpointRotationMatrix(glm::vec3 checkpointPos, glm::vec3 NextcheckpointPos) {
	glm::vec3 direction = glm::normalize(NextcheckpointPos - checkpointPos);

	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
	if (glm::length(glm::cross(direction, up)) < 0.0001f) {
		up = glm::vec3(1.0f, 0.0f, 0.0f); // Jeœli blisko równoleg³y, u¿yj wektora (1, 0, 0)
	}

	glm::vec3 right = glm::normalize(glm::cross(direction, up)); // Obliczamy wektor prawej strony
	up = glm::normalize(glm::cross(right, direction)); // Poprawiamy "górê" by by³a prostopad³a do prawej strony

	// Tworzymy macierz rotacji na podstawie uzyskanych wektorów
	glm::mat4 rotationMatrix(1.0f); // Inicjujemy macierz jednostkow¹
	rotationMatrix[0] = glm::vec4(right, 0.0f);
	rotationMatrix[1] = glm::vec4(up, 0.0f);
	rotationMatrix[2] = glm::vec4(-direction, 0.0f);

	return rotationMatrix;
}

GLuint VAO, VBO;

float aspectRatio = 16.0f / 9.0f;

float exposition = 1.f;

glm::vec3 lightPos = glm::vec3(0, 5, -2);
glm::vec3 lightColor = glm::vec3(0.9, 0.7, 0.8) * 100;

glm::vec3 spotlightPos = glm::vec3(0, 0, 0);
glm::vec3 spotlightConeDir = glm::vec3(0, 0, 0);
glm::vec3 spotlightColor = glm::vec3(0.5, 0.9, 0.8) * 10;
float spotlightPhi = 3.14 / 3;


glm::mat4 createCameraMatrix()
{
	glm::vec3 cameraSide = glm::normalize(glm::cross(cameraDir, glm::vec3(0.f, 1.f, 0.f)));
	glm::vec3 cameraUp = glm::normalize(glm::cross(cameraSide, cameraDir));
	glm::mat4 cameraRotrationMatrix = glm::mat4({
		cameraSide.x,cameraSide.y,cameraSide.z,0,
		cameraUp.x,cameraUp.y,cameraUp.z ,0,
		-cameraDir.x,-cameraDir.y,-cameraDir.z,0,
		0.,0.,0.,1.,
		});
	cameraRotrationMatrix = glm::transpose(cameraRotrationMatrix);
	glm::mat4 cameraMatrix = cameraRotrationMatrix * glm::translate(-cameraPos);

	return cameraMatrix;
}

glm::mat4 createPerspectiveMatrix()
{

	glm::mat4 perspectiveMatrix;
	float n = 0.05f;
	float f = 100.0f;
	float a1 = glm::min(aspectRatio, 1.f);
	float a2 = glm::min(1 / aspectRatio, 1.f);
	perspectiveMatrix = glm::mat4({
		1,0.,0.,0.,
		0.,aspectRatio,0.,0.,
		0.,0.,(f + n) / (n - f),2 * f * n / (n - f),
		0.,0.,-1.,0.,
		});


	perspectiveMatrix = glm::transpose(perspectiveMatrix);

	return perspectiveMatrix;
}

void drawObjectColor(Core::RenderContext& context, glm::mat4 modelMatrix, glm::vec3 color, glm::vec3 lightPos) {

	glUseProgram(program);
	glm::mat4 viewProjectionMatrix = createPerspectiveMatrix() * createCameraMatrix();
	glm::mat4 transformation = viewProjectionMatrix * modelMatrix;
	glUniformMatrix4fv(glGetUniformLocation(program, "transformation"), 1, GL_FALSE, (float*)&transformation);
	glUniformMatrix4fv(glGetUniformLocation(program, "modelMatrix"), 1, GL_FALSE, (float*)&modelMatrix);

	glUniform1f(glGetUniformLocation(program, "exposition"), exposition);
	glUniform3f(glGetUniformLocation(program, "color"), color.x, color.y, color.z);
	glUniform3f(glGetUniformLocation(program, "cameraPos"), cameraPos.x, cameraPos.y, cameraPos.z);
	glUniform3f(glGetUniformLocation(program, "lightPos"), lightPos.x, lightPos.y, lightPos.z);
	glUniform3f(glGetUniformLocation(program, "lightColor"), lightColor.x, lightColor.y, lightColor.z);

	glUniform3f(glGetUniformLocation(program, "spotlightConeDir"), spotlightConeDir.x, spotlightConeDir.y, spotlightConeDir.z);
	glUniform3f(glGetUniformLocation(program, "spotlightPos"), spotlightPos.x, spotlightPos.y, spotlightPos.z);
	glUniform3f(glGetUniformLocation(program, "spotlightColor"), spotlightColor.x, spotlightColor.y, spotlightColor.z);
	glUniform1f(glGetUniformLocation(program, "spotlightPhi"), spotlightPhi);
	Core::DrawContext(context);

}

void drawObjectTexture(Core::RenderContext& context, glm::mat4 modelMatrix, GLuint textureID) {
	glUseProgram(programTex);
	glm::mat4 viewProjectionMatrix = createPerspectiveMatrix() * createCameraMatrix();
	glm::mat4 transformation = viewProjectionMatrix * modelMatrix;
	glUniformMatrix4fv(glGetUniformLocation(programTex, "transformation"), 1, GL_FALSE, (float*)&transformation);
	glUniformMatrix4fv(glGetUniformLocation(programTex, "modelMatrix"), 1, GL_FALSE, (float*)&modelMatrix);
	glUniform3f(glGetUniformLocation(programTex, "lightPos"), lightPos.x, lightPos.y, lightPos.z);
	Core::SetActiveTexture(textureID, "colorTexture", programTex, 0);
	Core::DrawContext(context);

}

void drawObjectTextureNormal(Core::RenderContext& context, glm::mat4 modelMatrix, GLuint textureID, GLuint normalmapId) {
	glUseProgram(programTexNormal);

	glm::mat4 viewProjectionMatrix = createPerspectiveMatrix() * createCameraMatrix();
	glm::mat4 transformation = viewProjectionMatrix * modelMatrix;

	glUniformMatrix4fv(glGetUniformLocation(programTexNormal, "transformation"), 1, GL_FALSE, (float*)&transformation);
	glUniformMatrix4fv(glGetUniformLocation(programTexNormal, "modelMatrix"), 1, GL_FALSE, (float*)&modelMatrix);
	glUniform3f(glGetUniformLocation(programTexNormal, "cameraPos"), cameraPos.x, cameraPos.y, cameraPos.z);
	glUniform3f(glGetUniformLocation(programTexNormal, "lightPos"), lightPos.x, lightPos.y, lightPos.z);

	Core::SetActiveTexture(textureID, "colorTexture", programTexNormal, 0);

	Core::SetActiveTexture(normalmapId, "normalSampler", programTexNormal, 1);


	/*glUniform1f(glGetUniformLocation(program, "exposition"), exposition);
	glUniform3f(glGetUniformLocation(program, "lightColor"), lightColor.x, lightColor.y, lightColor.z);
	glUniform3f(glGetUniformLocation(program, "spotlightConeDir"), spotlightConeDir.x, spotlightConeDir.y, spotlightConeDir.z);
	glUniform3f(glGetUniformLocation(program, "spotlightPos"), spotlightPos.x, spotlightPos.y, spotlightPos.z);
	glUniform3f(glGetUniformLocation(program, "spotlightColor"), spotlightColor.x, spotlightColor.y, spotlightColor.z);
	glUniform1f(glGetUniformLocation(program, "spotlightPhi"), spotlightPhi);*/

	Core::DrawContext(context);
}

void drawObjectShip(Core::RenderContext& context, glm::mat4 modelMatrix, GLuint textureID, GLuint normalmapId) {
	glUseProgram(programShip);

	glm::mat4 viewProjectionMatrix = createPerspectiveMatrix() * createCameraMatrix();
	glm::mat4 transformation = viewProjectionMatrix * modelMatrix;

	glUniformMatrix4fv(glGetUniformLocation(programShip, "transformation"), 1, GL_FALSE, (float*)&transformation);
	glUniformMatrix4fv(glGetUniformLocation(programShip, "modelMatrix"), 1, GL_FALSE, (float*)&modelMatrix);
	glUniform3f(glGetUniformLocation(programShip, "cameraPos"), cameraPos.x, cameraPos.y, cameraPos.z);
	glUniform3f(glGetUniformLocation(programShip, "lightPos"), lightPos.x, lightPos.y, lightPos.z);

	// Aktywuj jednostkê teksturowania nr 0 dla kolorowej tekstury
	Core::SetActiveTexture(textureID, "colorTexture", programShip, 0);

	// Aktywuj jednostkê teksturowania nr 1 dla mapy normalnej
	Core::SetActiveTexture(normalmapId, "normalSampler", programShip, 1);


	/*glUniform1f(glGetUniformLocation(programShip, "exposition"), exposition);
	glUniform3f(glGetUniformLocation(programShip, "lightColor"), lightColor.x, lightColor.y, lightColor.z);
	glUniform3f(glGetUniformLocation(programShip, "spotlightConeDir"), spotlightConeDir.x, spotlightConeDir.y, spotlightConeDir.z);
	glUniform3f(glGetUniformLocation(programShip, "spotlightPos"), spotlightPos.x, spotlightPos.y, spotlightPos.z);
	glUniform3f(glGetUniformLocation(programShip, "spotlightColor"), spotlightColor.x, spotlightColor.y, spotlightColor.z);
	glUniform1f(glGetUniformLocation(programShip, "spotlightPhi"), spotlightPhi);*/

	Core::DrawContext(context);
}

void drawEarth(Core::RenderContext& context, glm::mat4 modelMatrix, GLuint textureID, GLuint textureID2) {

	glUseProgram(programEarth);
	glm::mat4 viewProjectionMatrix = createPerspectiveMatrix() * createCameraMatrix();
	glm::mat4 transformation = viewProjectionMatrix * modelMatrix;
	glUniformMatrix4fv(glGetUniformLocation(programEarth, "transformation"), 1, GL_FALSE, (float*)&transformation);
	glUniformMatrix4fv(glGetUniformLocation(programEarth, "modelMatrix"), 1, GL_FALSE, (float*)&modelMatrix);

	Core::SetActiveTexture(textureID, "colorTexture", programEarth, 0);
	Core::SetActiveTexture(textureID2, "clouds", programEarth, 2);

	glUniform3f(glGetUniformLocation(programEarth, "lightPos"), lightPos.x, lightPos.y, lightPos.z);
	Core::DrawContext(context);

}

void drawObjectProc(Core::RenderContext& context, glm::mat4 modelMatrix, glm::vec3 color1, glm::vec3 color2) {

	glUseProgram(programProcTex);
	glm::mat4 viewProjectionMatrix = createPerspectiveMatrix() * createCameraMatrix();
	glm::mat4 transformation = viewProjectionMatrix * modelMatrix;
	glUniformMatrix4fv(glGetUniformLocation(programProcTex, "transformation"), 1, GL_FALSE, (float*)&transformation);
	glUniformMatrix4fv(glGetUniformLocation(programProcTex, "modelMatrix"), 1, GL_FALSE, (float*)&modelMatrix);
	glUniform3f(glGetUniformLocation(programProcTex, "color1"), color1.x, color1.y, color1.z);
	glUniform3f(glGetUniformLocation(programProcTex, "color2"), color2.x, color2.y, color2.z);
	glUniform3f(glGetUniformLocation(programProcTex, "lightPos"), lightPos.x, lightPos.y, lightPos.z);
	Core::DrawContext(context);

}

void drawSkyBox(Core::RenderContext& context, glm::mat4 modelMatrix, GLuint cubemapTexture) {

	glUseProgram(programSkyBox);
	glm::mat4 viewProjectionMatrix = createPerspectiveMatrix() * createCameraMatrix();
	glm::mat4 transformation = viewProjectionMatrix * modelMatrix;
	glUniformMatrix4fv(glGetUniformLocation(programSkyBox, "transformation"), 1, GL_FALSE, (float*)&transformation);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
	glUniform1i(glGetUniformLocation(programSkyBox, "skybox"), 0);
	Core::DrawContext(context);

}

GLuint loadCubeMap(const std::vector<std::string>& filePaths)
{
	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int w, h;
	for (unsigned int i = 0; i < 6; i++)
	{
		unsigned char* image = SOIL_load_image(filePaths[i].c_str(), &w, &h, 0, SOIL_LOAD_RGBA);
		if (image)
		{
			glTexImage2D(
				GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image
			);
			SOIL_free_image_data(image);
		}
		else
		{
			std::cout << "Cubemap tex failed to load at path: " << filePaths[i] << std::endl;
			SOIL_free_image_data(image);
		}
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return textureID;
}

void renderScene(GLFWwindow* window)
{
	glClearColor(0.0f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glDisable(GL_DEPTH_TEST);
	drawSkyBox(cubeContext, glm::translate(glm::mat4(1.0f), cameraPos), cubemapTexture);
	glEnable(GL_DEPTH_TEST);

	glm::mat4 transformation;
	float time = glfwGetTime();


	glUseProgram(program);
	//drawObjectColor(sphereContext, glm::mat4(), glm::vec3(0.95, 0.1, 0.1));

	glm::vec3 spaceshipSide = glm::normalize(glm::cross(spaceshipDir, glm::vec3(0.f, 1.f, 0.f)));
	glm::vec3 spaceshipUp = glm::normalize(glm::cross(spaceshipSide, spaceshipDir));
	glm::mat4 spaceshipCameraRotationMatrix = glm::mat4({
		spaceshipSide.x,spaceshipSide.y,spaceshipSide.z,0,
		spaceshipUp.x,spaceshipUp.y,spaceshipUp.z ,0,
		-spaceshipDir.x,-spaceshipDir.y,-spaceshipDir.z,0,
		0.,0.,0.,1.,
		});

	// Player
	drawObjectShip(shipContext,
		glm::translate(spaceshipPos) * spaceshipCameraRotationMatrix * glm::eulerAngleY(glm::pi<float>()) * glm::scale(glm::vec3(0.1)),
		texture::spaceship, texture::spaceship_normal
	);

	// Bot1
	drawObjectShip(shipContext,
		glm::translate(Bot1Pos) * BotCameraRotationMatrix(Bot1Dir) * glm::eulerAngleY(glm::pi<float>()) * glm::scale(glm::vec3(0.1)),
		texture::spaceship, texture::spaceship_normal
	);

	// Bot2
	drawObjectShip(shipContext,
		glm::translate(Bot2Pos) * BotCameraRotationMatrix(Bot2Dir) * glm::eulerAngleY(glm::pi<float>()) * glm::scale(glm::vec3(0.1)),
		texture::spaceship, texture::spaceship_normal
	);

	// Checkpoint
	drawObjectColor(checkpointContext,
		glm::translate(checkpointPos[currentCheckpointIndex]) * calculateCheckpointRotationMatrix(checkpointPos[currentCheckpointIndex], checkpointPos[currentCheckpointIndex + 1]),
		colors[colorIndex], lightPos
	);
	// Arrow
	if (colorIndex != 1) {
		drawObjectColor(arrowContext,
			glm::translate(checkpointPos[currentCheckpointIndex]) * calculateCheckpointRotationMatrix(checkpointPos[currentCheckpointIndex], checkpointPos[currentCheckpointIndex + 1]),
			colors[colorIndex], lightPos
		);
	}

	// 1st place
	drawObjectColor(place1Context,
		glm::translate(glm::vec3(-100.f, -100.0f, -100.0f)),
		glm::vec3(1.0, 1.0, 0.0), cameraPos
	);
	// 2nd place
	drawObjectColor(place2Context,
		glm::translate(glm::vec3(-100.f, -100.0f, -100.0f)),
		glm::vec3(0.5, 0.5, 0.5), cameraPos
	);
	// 3rd place
	drawObjectColor(place3Context,
		glm::translate(glm::vec3(-100.f, -100.0f, -100.0f)),
		glm::vec3(0.5, 0.2, 0.0), cameraPos
	);
	// You
	drawObjectColor(youContext,
		glm::translate(youPos) * BotCameraRotationMatrix(youDir),
		glm::vec3(1.0, 1.0, 1.0), cameraPos
	);


	// Moon_Normal
	drawObjectTextureNormal(sphereContext,
		/*glm::eulerAngleY(time / 3) * */ glm::translate(glm::vec3(3.f, 0, 2.0f)) * /*glm::eulerAngleY(time) * */ glm::translate(glm::vec3(1.f, 0, 0)) * glm::scale(glm::vec3(0.1f)), texture::moon, texture::moon_normal);

	//Earth_Normal
	drawObjectTextureNormal(sphereContext,
		/*glm::eulerAngleY(time / 1) * */ glm::translate(glm::vec3(5.f, 0, 0)) * glm::scale(glm::vec3(0.3f)), texture::earth2, texture::earth2_normal);
	
	//Water
	drawObjectTextureNormal(sphereContext,
		/*glm::eulerAngleY(time / 1) * */ glm::translate(glm::vec3(6.f, 8, 0)) * glm::scale(glm::vec3(0.3f)), texture::water, texture::water_normal);
	//Brain
	drawObjectTextureNormal(sphereContext,
		/*glm::eulerAngleY(time / 1) * */ glm::translate(glm::vec3(6.f, 6, 0)) * glm::scale(glm::vec3(0.3f)), texture::brain, texture::brain_normal);
	//Tiles
	drawObjectTextureNormal(sphereContext,
		/*glm::eulerAngleY(time / 1) * */ glm::translate(glm::vec3(6.f, 4, 0)) * glm::scale(glm::vec3(0.3f)), texture::tiles, texture::tiles_normal);
	//Brick
	drawObjectTextureNormal(sphereContext,
		/*glm::eulerAngleY(time / 1) * */ glm::translate(glm::vec3(6.f, 2, 0)) * glm::scale(glm::vec3(0.3f)), texture::brick, texture::brick_normal);
	//Stone
	drawObjectTextureNormal(sphereContext,
		/*glm::eulerAngleY(time / 1) * */ glm::translate(glm::vec3(6.f, 0, 0)) * glm::scale(glm::vec3(0.3f)), texture::stone, texture::stone_normal);
	//Gold
	drawObjectTextureNormal(sphereContext,
		/*glm::eulerAngleY(time / 1) * */ glm::translate(glm::vec3(6.f, -2, 0)) * glm::scale(glm::vec3(0.3f)), texture::gold, texture::gold_normal);
	//Rock
	drawObjectTextureNormal(sphereContext,
		/*glm::eulerAngleY(time / 1) * */ glm::translate(glm::vec3(6.f, -4, 0)) * glm::scale(glm::vec3(0.3f)), texture::rock, texture::rock_normal);
	//Rock2
	drawObjectTextureNormal(sphereContext,
		/*glm::eulerAngleY(time / 1) * */ glm::translate(glm::vec3(6.f, -6, 0)) * glm::scale(glm::vec3(0.3f)), texture::rock2, texture::rock2_normal);
	//Rope
	drawObjectTextureNormal(sphereContext,
		/*glm::eulerAngleY(time / 1) * */ glm::translate(glm::vec3(6.f, -8, 0)) * glm::scale(glm::vec3(0.3f)), texture::rope, texture::rope_normal);
	//Sand
	drawObjectTextureNormal(sphereContext,
		/*glm::eulerAngleY(time / 1) * */ glm::translate(glm::vec3(6.f, -10, 0)) * glm::scale(glm::vec3(0.3f)), texture::sand, texture::sand_normal);
	//Grass
	drawObjectTextureNormal(sphereContext,
		/*glm::eulerAngleY(time / 1) * */ glm::translate(glm::vec3(6.f, -12, 0)) * glm::scale(glm::vec3(0.3f)), texture::grass, texture::grass_normal);
	//Mud
	drawObjectTextureNormal(sphereContext,
		/*glm::eulerAngleY(time / 1) * */ glm::translate(glm::vec3(6.f, -14, 0))* glm::scale(glm::vec3(0.3f)), texture::mud, texture::mud_normal);
	//Moss
	drawObjectTextureNormal(sphereContext,
		/*glm::eulerAngleY(time / 1) * */ glm::translate(glm::vec3(6.f, -16, 0))* glm::scale(glm::vec3(0.3f)), texture::moss, texture::moss_normal);
	//Ground
	drawObjectTextureNormal(sphereContext,
		/*glm::eulerAngleY(time / 1) * */ glm::translate(glm::vec3(6.f, -18, 0)) * glm::scale(glm::vec3(0.3f)), texture::ground, texture::ground_normal);

	spotlightPos = spaceshipPos + 0.5 * spaceshipDir;
	spotlightConeDir = spaceshipDir;

	glUseProgram(0);
	glfwSwapBuffers(window);
}
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	aspectRatio = float(width) / float(height);
	glViewport(0, 0, width, height);
}
void loadModelToContext(std::string path, Core::RenderContext& context)
{
	Assimp::Importer import;
	const aiScene * scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_CalcTangentSpace);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl;
		return;
	}
	context.initFromAssimpMesh(scene->mMeshes[0]);
}

void init(GLFWwindow* window)
{
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	glEnable(GL_DEPTH_TEST);
	program = shaderLoader.CreateProgram("shaders/shader_8_1.vert", "shaders/shader_8_1.frag");
	programSun = shaderLoader.CreateProgram("shaders/shader_8_sun.vert", "shaders/shader_8_sun.frag");
	programTex = shaderLoader.CreateProgram("shaders/shader_5_1_tex.vert", "shaders/shader_5_1_tex.frag");
	programEarth = shaderLoader.CreateProgram("shaders/shader_earth.vert", "shaders/shader_earth.frag");
	programProcTex = shaderLoader.CreateProgram("shaders/shader_proc_tex.vert", "shaders/shader_proc_tex.frag");
	programSkyBox = shaderLoader.CreateProgram("shaders/shader_skybox.vert", "shaders/shader_skybox.frag");
	programTexNormal = shaderLoader.CreateProgram("shaders/shader_5_tex_normal.vert", "shaders/shader_5_tex_normal.frag");
	programShip = shaderLoader.CreateProgram("shaders/shader_ship.vert", "shaders/shader_ship.frag");


	loadModelToContext("./models/sphere.obj", sphereContext);
	loadModelToContext("./models/spaceship.obj", shipContext);
	loadModelToContext("./models/cube.obj", cubeContext);
	loadModelToContext("./models/checkpoint.obj", checkpointContext);
	loadModelToContext("./models/first.obj", place1Context);
	loadModelToContext("./models/second.obj", place2Context);
	loadModelToContext("./models/third.obj", place3Context);
	loadModelToContext("./models/you.obj", youContext);
	loadModelToContext("./models/arrow.obj", arrowContext);

	texture::earth = Core::LoadTexture("textures/earth.png");
	texture::earth2 = Core::LoadTexture("textures/earth2.png");
	texture::moon = Core::LoadTexture("textures/moon.jpg");
	texture::clouds = Core::LoadTexture("./textures/clouds.jpg");
	texture::grid = Core::LoadTexture("./textures/grid.png");
	texture::sun = Core::LoadTexture("./textures/sun.jg");
	texture::mars = Core::LoadTexture("./textures/mars.jpg");
	texture::spaceship = Core::LoadTexture("./textures/spaceshipPBR/StarSparrow_albedo.png");

	texture::earth_normal = Core::LoadTexture("./textures/earth_normalmap.png");
	texture::earth2_normal = Core::LoadTexture("./textures/earth2_normals.png");
	texture::spaceship_normal = Core::LoadTexture("./textures/spaceshipPBR/StarSparrow_Normal.png");
	texture::moon_normal = Core::LoadTexture("./textures/moon_normals.png");

	//new
	texture::brain = Core::LoadTexture("./textures/new/brain_albedo.jpg");
	texture::brain_normal = Core::LoadTexture("./textures/new/brain_normal.jpg");
	texture::rope = Core::LoadTexture("./textures/new/rope_albedo.jpg");
	texture::rope_normal = Core::LoadTexture("./textures/new/rope_normal.jpg");
	texture::sand = Core::LoadTexture("./textures/new/sand_albedo.jpg");
	texture::sand_normal = Core::LoadTexture("./textures/new/sand_normal.jpg");
	texture::brick = Core::LoadTexture("./textures/new/brick_albedo.jpg");
	texture::brick_normal = Core::LoadTexture("./textures/new/brick_normal.jpg");
	texture::stone = Core::LoadTexture("./textures/new/stone_albedo.jpg");
	texture::stone_normal = Core::LoadTexture("./textures/new/stone_normal.jpg");
	texture::gold = Core::LoadTexture("./textures/new/gold_albedo.jpg");
	texture::gold_normal = Core::LoadTexture("./textures/new/gold_normal.jpg");
	texture::rock = Core::LoadTexture("./textures/new/rock_albedo.jpg");
	texture::rock_normal = Core::LoadTexture("./textures/new/rock_normal.jpg");
	texture::rock2 = Core::LoadTexture("./textures/new/rock2_albedo.jpg");
	texture::rock2_normal = Core::LoadTexture("./textures/new/rock2_normal.jpg");
	texture::tiles = Core::LoadTexture("./textures/new/tiles_albedo.jpg");
	texture::tiles_normal = Core::LoadTexture("./textures/new/tiles_normal.jpg");

	texture::water = Core::LoadTexture("./textures/water/Pool_Water_Texture_Diff.jpg");
	texture::water_normal = Core::LoadTexture("./textures/water/Pool_Water_Texture_nrml.jpg");
	texture::grass = Core::LoadTexture("./textures/grassy/coast_sand_rocks_02_diff_1k.jpg");
	texture::grass_normal = Core::LoadTexture("./textures/grassy/coast_sand_rocks_02_nor_gl_1k.jpg");
	texture::moss = Core::LoadTexture("./textures/mossy_rock/mossy_rock_diff_1k.jpg");
	texture::moss_normal = Core::LoadTexture("./textures/mossy_rock/mossy_rock_nor_gl_1k.jpg");
	texture::mud = Core::LoadTexture("./textures/mud/brown_mud_03_diff_1k.jpg");
	texture::mud_normal = Core::LoadTexture("./textures/mud/brown_mud_03_nor_gl_1k.jpg");
	texture::ground = Core::LoadTexture("./textures/rocks_ground/rocks_ground_01_diff_1k.jpg");
	texture::ground_normal = Core::LoadTexture("./textures/rock_ground/rocks_ground_01_nor_gl_1k.jpg");



	std::vector<std::string> skyboxTextures = {
	"./textures/skybox/space_rt.png",
	"./textures/skybox/space_lf.png",
	"./textures/skybox/space_up.png",
	"./textures/skybox/space_dn.png",
	"./textures/skybox/space_ft.png",
	"./textures/skybox/space_bk.png"
	};

	cubemapTexture = loadCubeMap(skyboxTextures);

}

void shutdown(GLFWwindow* window)
{
	shaderLoader.DeleteProgram(program);
	shaderLoader.DeleteProgram(programTex);
	shaderLoader.DeleteProgram(programSkyBox);
	shaderLoader.DeleteProgram(programSun);
	shaderLoader.DeleteProgram(programEarth);
	shaderLoader.DeleteProgram(programProcTex);
	shaderLoader.DeleteProgram(cubemapTexture);
	shaderLoader.DeleteProgram(programTexNormal);
	shaderLoader.DeleteProgram(programShip);
}

void processInput(GLFWwindow* window)
{
	glm::vec3 spaceshipSide = glm::normalize(glm::cross(spaceshipDir, glm::vec3(0.f, 1.f, 0.f)));
	glm::vec3 spaceshipUp = glm::vec3(0.f, 1.f, 0.f);

	float time = glfwGetTime();
	deltaTime = time - lastFrameTime;
	lastFrameTime = time;
	float fps = 1 / deltaTime;

	float angleSpeed = 2.f / fps;
	float moveSpeed = 4.5f / fps;
	
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}

	// Coordinates
	if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
	{
		std::cout << "Spaceship Position: (" << spaceshipPos.x << ", " << spaceshipPos.y << ", " << spaceshipPos.z << ")\n";
		std::cout << "Spaceship Direction: (" << spaceshipDir.x << ", " << spaceshipDir.y << ", " << spaceshipDir.z << ")\n";
	}

	// Checkpoint
	if (checkpointReached2(currentCheckpointIndex, spaceshipPos, calculateCheckpointRotationMatrix(checkpointPos[currentCheckpointIndex], checkpointPos[currentCheckpointIndex + 1])))
	{
		currentCheckpointIndex += 1;
	}

	if (currentCheckpointIndex == 13) {
		colorIndex = 1;
	}


	// Finishing the race
	if (checkpointReached2(13, spaceshipPos, calculateCheckpointRotationMatrix(checkpointPos[currentCheckpointIndex], checkpointPos[currentCheckpointIndex + 1])) &&
		currentCheckpointIndex == 14)
	{
		PlayerEnd = true;
		// Teleport
		if (Bot1End == false && Bot2End == false) {
			spaceshipPos = glm::vec3(-100.0f, -97.0f, -100.0f);
		}
		else if (Bot1End == true && Bot2End == true) {
			spaceshipPos = glm::vec3(-97.8f, -98.0f, -100.0f);
			youDir = glm::vec3(0.3f, 0.0f, -1.0f);
		}
		else {
			spaceshipPos = glm::vec3(-102.2f, -97.5f, -100.0f);
			youDir = glm::vec3(-0.4f, 0.0f, -1.0f);
		}
		spaceshipDir = glm::vec3(0.f, 0.f, 1.0f);
		youPos.x = spaceshipPos.x;
		youPos.y = spaceshipPos.y + 1.0f;
		youPos.z = spaceshipPos.z;
		cameraPos = glm::vec3(-100.0f, -96.5f, -95.0f);
		cameraDir = glm::vec3(0.f, -0.2f, -1.0f);

	}
	if (PlayerEnd == false) {
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
			spaceshipPos += spaceshipDir * moveSpeed;
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
			spaceshipPos -= spaceshipDir * moveSpeed;
		if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
			spaceshipPos += spaceshipSide * moveSpeed;
		if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
			spaceshipPos -= spaceshipSide * moveSpeed;
		if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS)
			spaceshipPos += spaceshipUp * moveSpeed;
		if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
			spaceshipPos -= spaceshipUp * moveSpeed;
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
			spaceshipDir = glm::vec3(glm::eulerAngleY(angleSpeed) * glm::vec4(spaceshipDir, 0));
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
			spaceshipDir = glm::vec3(glm::eulerAngleY(-angleSpeed) * glm::vec4(spaceshipDir, 0));
		cameraPos = spaceshipPos - 2.0 * spaceshipDir + glm::vec3(0, 1, 0) * 0.5f;
		cameraDir = spaceshipDir;
	}

	float tBot1 = (glfwGetTime() * 0.5f - 3.0f);
	float tBot2 = (glfwGetTime() * 0.3f - 1.0f);

	//spaceshipPos = Bot1Pos;
	//std::cout << "tBot1: " << tBot1 << ", " << "Time: " << raceStartTime << ")\n";

	//if (tBot1 < 1.0f) {
	//	Bot1Pos = CalculateCatmullRomSpline(tBot1, Bot1Spawn, Bot1Spawn, checkpointPos[0], checkpointPos[1]);
	//	Bot1Dir = CalculateCatmullRomSplineDerivative(tBot1, Bot1Spawn, Bot1Spawn, checkpointPos[0], checkpointPos[1]);
	//}
	//else if (tBot1 < 2.0f) {
	//	Bot1Pos = CalculateCatmullRomSpline(tBot1 - 1.0f, Bot1Spawn, checkpointPos[0], checkpointPos[1], checkpointPos[2]);
	//	Bot1Dir = CalculateCatmullRomSplineDerivative(tBot1 - 1.0f, Bot1Spawn, checkpointPos[0], checkpointPos[1], checkpointPos[2]);
	//}
	//else if (tBot1 < 3.0f) {
	//	Bot1Pos = CalculateCatmullRomSpline(tBot1 - 2.0f, checkpointPos[0], checkpointPos[1], checkpointPos[2], checkpointPos[3]);
	//	Bot1Dir = CalculateCatmullRomSplineDerivative(tBot1 - 2.0f, checkpointPos[0], checkpointPos[1], checkpointPos[2], checkpointPos[3]);
	//}
	//else if (tBot1 < 4.0f) {
	//	Bot1Pos = CalculateCatmullRomSpline(tBot1 - 3.0f, checkpointPos[1], checkpointPos[2], checkpointPos[3], checkpointPos[4]);
	//	Bot1Dir = CalculateCatmullRomSplineDerivative(tBot1 - 3.0f, checkpointPos[1], checkpointPos[2], checkpointPos[3], checkpointPos[4]);
	//}
	//else if (tBot1 < 5.0f) {
	//	Bot1Pos = CalculateCatmullRomSpline(tBot1 - 4.0f, checkpointPos[2], checkpointPos[3], checkpointPos[4], checkpointPos[5]);
	//	Bot1Dir = CalculateCatmullRomSplineDerivative(tBot1 - 4.0f, checkpointPos[2], checkpointPos[3], checkpointPos[4], checkpointPos[5]);
	//	if (checkpointReached2(4, Bot1Pos, calculateCheckpointRotationMatrix(checkpointPos[currentCheckpointIndex], checkpointPos[currentCheckpointIndex + 1])) && !(PlayerEnd == true)) {
	//		Bot1End = true;
	//		Bot1Pos = glm::vec3(-100.0f, -97.0f, -100.0f);
	//		Bot1Dir = glm::vec3(0.0f, 0.0f, 1.0f);
	//	}
	//	else if (checkpointReached2(4, Bot1Pos, calculateCheckpointRotationMatrix(checkpointPos[currentCheckpointIndex], checkpointPos[currentCheckpointIndex + 1])) && (PlayerEnd == true)) {
	//		Bot1Pos = glm::vec3(-102.2f, -97.5f, -100.0f);
	//		Bot1Dir = glm::vec3(0.0f, 0.0f, 1.0f);
	//	}
	//}

	if (tBot1 < 1.0f) {
		Bot1Pos = CalculateCatmullRomSpline(tBot1, Bot1Spawn, Bot1Spawn, checkpointPos[0], checkpointPos[1]);
		Bot1Dir = CalculateCatmullRomSplineDerivative(tBot1, Bot1Spawn, Bot1Spawn, checkpointPos[0], checkpointPos[1]);
	}
	else if (tBot1 < 2.0f) {
		Bot1Pos = CalculateCatmullRomSpline(tBot1 - 1.0f, Bot1Spawn, checkpointPos[0], checkpointPos[1], checkpointPos[2]);
		Bot1Dir = CalculateCatmullRomSplineDerivative(tBot1 - 1.0f, Bot1Spawn, checkpointPos[0], checkpointPos[1], checkpointPos[2]);
	} else if (Bot1End == false && Bot1Endc == false) { 
		for (float i = 0; i < 13; ++i) {
			if (tBot1 < (i + 3)) {
				Bot1Pos = CalculateCatmullRomSpline(tBot1 - 2 - i, checkpointPos[i], checkpointPos[i + 1], checkpointPos[i + 2], checkpointPos[i + 3]);
				Bot1Dir = CalculateCatmullRomSplineDerivative(tBot1 - 2 - i, checkpointPos[i], checkpointPos[i + 1], checkpointPos[i + 2], checkpointPos[i + 3]);
				break;
			}
		}
	}
	if (checkpointReached2(13, Bot1Pos, calculateCheckpointRotationMatrix(checkpointPos[13], checkpointPos[14])) && !(PlayerEnd == true)) {
		Bot1End = true;
		Bot1Pos = glm::vec3(-100.0f, -97.0f, -100.0f);
		Bot1Dir = glm::vec3(0.0f, 0.0f, 1.0f);
	}
	else if (checkpointReached2(13, Bot1Pos, calculateCheckpointRotationMatrix(checkpointPos[13], checkpointPos[14])) && (PlayerEnd == true)) {
		Bot1Endc = true;
		Bot1Pos = glm::vec3(-102.2f, -97.5f, -100.0f);
		Bot1Dir = glm::vec3(0.0f, 0.0f, 1.0f);
	}



	//if (tBot2 < 1.0f) {
	//	Bot2Pos = CalculateCatmullRomSpline(tBot2, Bot2Spawn, Bot2Spawn, Bot2Spawn, checkpointPos[0]);
	//	Bot2Dir = CalculateCatmullRomSplineDerivative(tBot2, Bot2Spawn, Bot2Spawn, Bot2Spawn, checkpointPos[0]);
	//}
	//else if (tBot2 < 2.0f) {
	//	Bot2Pos = CalculateCatmullRomSpline(tBot2 - 1.0f, Bot2Spawn, Bot2Spawn, checkpointPos[0], checkpointPos[1]);
	//	Bot2Dir = CalculateCatmullRomSplineDerivative(tBot2 - 1.0f, Bot2Spawn, Bot2Spawn, checkpointPos[0], checkpointPos[1]);
	//}
	//else if (tBot2 < 3.0f) {
	//	Bot2Pos = CalculateCatmullRomSpline(tBot2 - 2.0f, Bot2Spawn, checkpointPos[0], checkpointPos[1], checkpointPos[2]);
	//	Bot2Dir = CalculateCatmullRomSplineDerivative(tBot2 - 2.0f, Bot2Spawn, checkpointPos[0], checkpointPos[1], checkpointPos[2]);
	//}
	//else if (tBot2 < 4.0f) {
	//	Bot2Pos = CalculateCatmullRomSpline(tBot2 - 3.0f, checkpointPos[0], checkpointPos[1], checkpointPos[2], checkpointPos[3]);
	//	Bot2Dir = CalculateCatmullRomSplineDerivative(tBot2 - 3.0f, checkpointPos[0], checkpointPos[1], checkpointPos[2], checkpointPos[3]);
	//}
	//else if (tBot2 < 5.0f) {
	//	Bot2Pos = CalculateCatmullRomSpline(tBot2 - 4.0f, checkpointPos[1], checkpointPos[2], checkpointPos[3], checkpointPos[4]);
	//	Bot2Dir = CalculateCatmullRomSplineDerivative(tBot2 - 4.0f, checkpointPos[1], checkpointPos[2], checkpointPos[3], checkpointPos[4]);
	//}
	//else if (tBot2 < 6.0f) {
	//	Bot2Pos = CalculateCatmullRomSpline(tBot2 - 5.0f, checkpointPos[2], checkpointPos[3], checkpointPos[4], checkpointPos[5]);
	//	Bot2Dir = CalculateCatmullRomSplineDerivative(tBot2 - 5.0f, checkpointPos[2], checkpointPos[3], checkpointPos[4], checkpointPos[5]);
	//	if (checkpointReached2(4, Bot2Pos, calculateCheckpointRotationMatrix(checkpointPos[currentCheckpointIndex], checkpointPos[currentCheckpointIndex + 1])) && !(PlayerEnd == true)) {
	//		Bot2End = true;
	//		Bot2Pos = glm::vec3(-102.2f, -97.5f, -100.0f);
	//		Bot2Dir = glm::vec3(0.0f, 0.0f, 1.0f);
	//	}
	//	else if (checkpointReached2(4, Bot2Pos, calculateCheckpointRotationMatrix(checkpointPos[currentCheckpointIndex], checkpointPos[currentCheckpointIndex + 1])) && (PlayerEnd == true)) {
	//		Bot2Pos = glm::vec3(-97.8f, -98.0f, -100.0f);
	//		Bot2Dir = glm::vec3(0.0f, 0.0f, 1.0f);
	//	}
	//}

	if (tBot2 < 1.0f) {
		Bot2Pos = CalculateCatmullRomSpline(tBot2, Bot2Spawn, Bot2Spawn, Bot2Spawn, checkpointPos[0]);
		Bot2Dir = CalculateCatmullRomSplineDerivative(tBot2, Bot2Spawn, Bot2Spawn, Bot2Spawn, checkpointPos[0]);
	}
	else if (tBot2 < 2.0f) {
		Bot2Pos = CalculateCatmullRomSpline(tBot2 - 1.0f, Bot2Spawn, Bot2Spawn, checkpointPos[0], checkpointPos[1]);
		Bot2Dir = CalculateCatmullRomSplineDerivative(tBot2 - 1.0f, Bot2Spawn, Bot2Spawn, checkpointPos[0], checkpointPos[1]);
	}
	else if (tBot2 < 3.0f) {
		Bot2Pos = CalculateCatmullRomSpline(tBot2 - 2.0f, Bot2Spawn, checkpointPos[0], checkpointPos[1], checkpointPos[2]);
		Bot2Dir = CalculateCatmullRomSplineDerivative(tBot2 - 2.0f, Bot2Spawn, checkpointPos[0], checkpointPos[1], checkpointPos[2]);
	}
	else if (Bot2End == false && Bot2Endc == false) {
		for (float j = 0; j < 13; ++j) {
			if (tBot2 < (j + 4)) {
				Bot2Pos = CalculateCatmullRomSpline(tBot2 - 3 - j, checkpointPos[j], checkpointPos[j + 1], checkpointPos[j + 2], checkpointPos[j + 3]);
				Bot2Dir = CalculateCatmullRomSplineDerivative(tBot2 - 3 - j, checkpointPos[j], checkpointPos[j + 1], checkpointPos[j + 2], checkpointPos[j + 3]);
				break;
			}
		}
	}
	if (checkpointReached2(13, Bot2Pos, calculateCheckpointRotationMatrix(checkpointPos[13], checkpointPos[14])) && !(PlayerEnd == true)) {
		Bot2End = true;
		Bot2Pos = glm::vec3(-102.2f, -97.5f, -100.0f);
		Bot2Dir = glm::vec3(0.0f, 0.0f, 1.0f);
	}
	else if (checkpointReached2(13, Bot2Pos, calculateCheckpointRotationMatrix(checkpointPos[13], checkpointPos[14])) && (PlayerEnd == true)) {
		Bot2Endc = true;
		Bot2Pos = glm::vec3(-97.8f, -98.0f, -100.0f);
		Bot2Dir = glm::vec3(0.0f, 0.0f, 1.0f);
	}
	

	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
		exposition -= 0.05;
	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
		exposition += 0.05;

	//cameraDir = glm::normalize(-cameraPos);

}

// main loop
void renderLoop(GLFWwindow* window) {

	while (!glfwWindowShouldClose(window))
	{
		processInput(window);

		renderScene(window);
		
		glfwPollEvents();
	}
}
//}