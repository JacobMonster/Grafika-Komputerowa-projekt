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


namespace texture {
	GLuint earth;
	GLuint clouds;
	GLuint moon;
	GLuint ship;

	GLuint grid;

	GLuint earthNormal;
	GLuint asteroidNormal;
	GLuint shipNormal;

	GLuint sun;
	GLuint mars;
}

GLuint program;
GLuint programSun;
GLuint programTex;
GLuint programSkyBox;
GLuint programEarth;
GLuint programProcTex;
GLuint cubemapTexture;

Core::Shader_Loader shaderLoader;

Core::RenderContext shipContext;
Core::RenderContext sphereContext;
Core::RenderContext cubeContext;
Core::RenderContext checkpointContext;
Core::RenderContext place1Context;
Core::RenderContext place2Context;
Core::RenderContext place3Context;
Core::RenderContext s2Context;
Core::RenderContext s3Context;

// Checkpoint positions
const int numCheckpoints = 5;
std::vector<glm::vec3> checkpointPos = {
	glm::vec3(0.0f, 1.0f, 4.0f),
	//glm::vec3(0.0f, 1.0f, 8.0f),
	//glm::vec3(0.0f, 1.0f, 12.0f),
	//glm::vec3(0.0f, 1.0f, 16.0f),
	//glm::vec3(0.0f, 1.0f, 20.0f)
	glm::vec3(-12.5f, 1.0f, 10.5f),
	glm::vec3(-6.5f, 1.0f, 25.5f),
	glm::vec3(-3.5f, 1.0f, 43.5f),
	glm::vec3(-6.5f, 1.0f, 54.0f),
	// Assistans
	glm::vec3(-6.5f, 1.0f, 60.0f)
};

int currentCheckpointIndex = 0;

bool reachedCheckpoint = false;

std::vector<glm::vec3> colors = {
	glm::vec3(1.0f, 1.0f, 0.0f),
	glm::vec3(1.0f, 0.0f, 0.0f)
};

int colorIndex = 0;

glm::vec3 CalculateCatmullRomSpline(float t, const glm::vec3& p0, const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3)
{
	float t2 = t * t;
	float t3 = t * t2;

	glm::vec4 coefficients(-0.5f * t3 + t2 - 0.5f * t,
		1.5f * t3 - 2.5f * t2 + 1.0f,
		-1.5f * t3 + 2.0f * t2 + 0.5f * t,
		0.5f * t3 - 0.5f * t2);

	glm::mat4x3 controlPointsMatrix(p0, p1, p2, p3);

	glm::vec3 result = controlPointsMatrix * coefficients;

	return result;
}

glm::vec3 CalculateBotDirection(const glm::vec3& currentPosition, const glm::vec3& nextPosition) {
	return glm::normalize(nextPosition - currentPosition);
}

glm::vec3 p01 = glm::vec3(2.0f, 1.0f, -1.0f);
glm::vec3 p02 = glm::vec3(-2.0f, 1.0f, -1.0f);
glm::vec3 p1 = checkpointPos[0];
glm::vec3 p2 = checkpointPos[1];
glm::vec3 p3 = checkpointPos[2];
glm::vec3 p4 = checkpointPos[3];
glm::vec3 p5 = checkpointPos[4];

bool botRaceStarted = false;
bool Bot1End = false;
bool Bot2End = false;
float raceStartTime = 0.0f;

glm::vec3 cameraPos = glm::vec3(-4.f, 0, 0);
glm::vec3 cameraDir = glm::vec3(1.f, 0.f, 0.f);


glm::vec3 spaceshipPos = glm::vec3(0.0f, 1.000000f, -5.0f);
glm::vec3 spaceshipDir = glm::vec3(0.0f, 0.000000f, 1.0f);

glm::vec3 Bot1Pos = glm::vec3(2.0f, 1.0f, -2.0f);
glm::vec3 Bot2Pos = glm::vec3(-2.0f, 1.0f, -2.0f);
glm::vec3 Bot1Dir = glm::normalize(glm::vec3(0.354510f, 0.000000f, 0.935054f));
glm::vec3 Bot2Dir = glm::normalize(glm::vec3(-0.354510f, 0.000000f, 0.935054f));

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

glm::mat4 BotCameraRotationMatrix(glm::vec3 BotPos, glm::vec3 BotDir) {
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

GLuint VAO, VBO;

float aspectRatio = 1.f;

float exposition = 1.f;

glm::vec3 lightPos = glm::vec3(-8, 4, 2);
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

void drawObjectColor(Core::RenderContext& context, glm::mat4 modelMatrix, glm::vec3 color) {


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
	glUniform3f(glGetUniformLocation(programTex, "lightPos"), 0, 0, 0);
	Core::SetActiveTexture(textureID, "colorTexture", programTex, 0);
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

	glUniform3f(glGetUniformLocation(programEarth, "lightPos"), 0, 0, 0);
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
	glUniform3f(glGetUniformLocation(programProcTex, "lightPos"), 0, 0, 0);
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

	// Mars
	//drawObjectTexture(sphereContext, glm::eulerAngleY(time / 3) * glm::translate(glm::vec3(4.f, 0, 0)) * glm::scale(glm::vec3(0.3f)), texture::mars);

	//drawObjectProc(sphereContext,
		//glm::eulerAngleY(time / 2) * glm::translate(glm::vec3(2.f, 2.f, 0)) * glm::scale(glm::vec3(0.3f)),
		//glm::vec3(1.0, 0.0, 0.0), glm::vec3(1.0, 1.0, 0.0));

	//drawObjectTexture(sphereContext, glm::eulerAngleY(time / 2) * glm::translate(glm::vec3(3.f, 0, 0)) * glm::scale(glm::vec3(0.3f)), texture::earth);

	//drawObjectColor(shipContext,
	//	glm::translate(cameraPos + 1.5 * cameraDir + cameraUp * -0.5f) * inveseCameraRotrationMatrix * glm::eulerAngleY(glm::pi<float>()),
	//	glm::vec3(0.3, 0.3, 0.5)
	//	);
	// 
	// Player
	drawObjectColor(shipContext,
		glm::translate(spaceshipPos) * spaceshipCameraRotationMatrix * glm::eulerAngleY(glm::pi<float>()) * glm::scale(glm::vec3(0.1)),
		glm::vec3(0.3, 0.3, 0.5)
	);

	// Bot1
	drawObjectColor(shipContext,
		glm::translate(Bot1Pos) /** BotCameraRotationMatrix(Bot1Pos, Bot1Dir) * glm::eulerAngleY(glm::pi<float>())*/ * glm::scale(glm::vec3(0.1)),
		glm::vec3(0.3, 0.3, 0.5)
	);

	// Bot2
	drawObjectColor(shipContext,
		glm::translate(Bot2Pos) * glm::scale(glm::vec3(0.1)),
		glm::vec3(0.3, 0.3, 0.5)
	);

	// Checkpoint
	drawObjectColor(checkpointContext,
		glm::translate(checkpointPos[currentCheckpointIndex]),
		colors[colorIndex]
	);

	// 1st place
	drawObjectColor(place1Context,
		glm::translate(glm::vec3(-20.f, -20.0f, -20.0f)),
		glm::vec3(1.0, 1.0, 0.0)
	);
	// 2nd place
	drawObjectColor(place2Context,
		glm::translate(glm::vec3(-20.f, -15.0f, -20.0f)),
		glm::vec3(0.5, 0.5, 0.5)
	);
	// 3rd place
	drawObjectColor(place3Context,
		glm::translate(glm::vec3(-20.f, -10.0f, -20.0f)),
		glm::vec3(0.5, 0.2, 0.0)
	);


	// Earth
	//drawObjectTexture(sphereContext, glm::eulerAngleY(time / 3) * glm::translate(glm::vec3(4.f, 0, 0)) * glm::eulerAngleY(time) * glm::scale(glm::vec3(0.3f)), texture::earth);

	

	// Moon
	drawObjectTexture(sphereContext,
		glm::eulerAngleY(time / 3) * glm::translate(glm::vec3(4.f, 0, 0)) * glm::eulerAngleY(time) * glm::translate(glm::vec3(1.f, 0, 0)) * glm::scale(glm::vec3(0.1f)), texture::moon);

	// Earth
	drawEarth(sphereContext, glm::eulerAngleY(time / 1) * glm::translate(glm::vec3(2.f, 0, 0)) * glm::scale(glm::vec3(0.3f)), texture::earth, texture::clouds);

	spotlightPos = spaceshipPos + 0.5 * spaceshipDir;
	spotlightConeDir = spaceshipDir;

	glUseProgram(0);
	glfwSwapBuffers(window);
}
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	aspectRatio = width / float(height);
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

	loadModelToContext("./models/sphere.obj", sphereContext);
	loadModelToContext("./models/spaceship.obj", shipContext);
	loadModelToContext("./models/cube.obj", cubeContext);
	loadModelToContext("./models/checkpoint.obj", checkpointContext);
	loadModelToContext("./models/place1.obj", place1Context);
	loadModelToContext("./models/place2.obj", place2Context);
	loadModelToContext("./models/place3.obj", place3Context);
	loadModelToContext("./models/s2.obj", s2Context);
	loadModelToContext("./models/s3.obj", s3Context);

	texture::earth = Core::LoadTexture("textures/earth.png");
	texture::moon = Core::LoadTexture("textures/moon.jpg");
	texture::clouds = Core::LoadTexture("./textures/clouds.jpg");
	texture::grid = Core::LoadTexture("./textures/grid.png");
	texture::sun = Core::LoadTexture("./textures/sun.jg");
	texture::mars = Core::LoadTexture("./textures/mars.jpg");

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
}

//obsluga wejscia
void processInput(GLFWwindow* window)
{
	glm::vec3 spaceshipSide = glm::normalize(glm::cross(spaceshipDir, glm::vec3(0.f, 1.f, 0.f)));
	glm::vec3 spaceshipUp = glm::vec3(0.f, 1.f, 0.f);
	float angleSpeed = 0.05f;
	float moveSpeed = 0.05f;
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}

	// Coordinates
	if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
	{
		std::cout << "Spaceship Position: (" << spaceshipPos.x << ", " << spaceshipPos.y << ", " << spaceshipPos.z << ")\n";
		std::cout << "Spaceship Direction: (" << spaceshipDir.x << ", " << spaceshipDir.y << ", " << spaceshipDir.z << ")\n";
	}

	// Checkpoint 0
	if (checkpointReached(0, spaceshipPos) &&
		currentCheckpointIndex == 0)
	{
		currentCheckpointIndex = 1;
	}

	// Checkpoint 1
	if (checkpointReached(1, spaceshipPos) &&
		currentCheckpointIndex == 1)
	{
		currentCheckpointIndex = 2;
	}

	// Checkpoint 2
	if (checkpointReached(2, spaceshipPos) &&
		currentCheckpointIndex == 2)
	{
		currentCheckpointIndex = 3;
	}

	// Checkpoint 3
	if (checkpointReached(3, spaceshipPos) &&
		currentCheckpointIndex == 3)
	{
		currentCheckpointIndex = 4;
		colorIndex = 1;
	}


	// Finishing the race
	if (checkpointReached(4, spaceshipPos) &&
		currentCheckpointIndex == 4)
	{
		// Teleport
		if (Bot1End == false && Bot2End == false) {
			cameraPos = glm::vec3(-20.0f, -20.0f, -17.5f);
		}
		else if (Bot1End == true && Bot2End == true) {
			cameraPos = glm::vec3(-20.0f, -10.0f, -17.5f);
		}
		else {
			cameraPos = glm::vec3(-20.0f, -15.0f, -17.5f);
		}
		cameraDir = glm::vec3(0.f, 0.f, -1.0f);

	}
	else {
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
		cameraPos = spaceshipPos - 1.5 * spaceshipDir + glm::vec3(0, 1, 0) * 0.5f;
		cameraDir = spaceshipDir;
	}
	

	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
		exposition -= 0.05;
	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
		exposition += 0.05;

	//cameraDir = glm::normalize(-cameraPos);

}

// main loop
void renderLoop(GLFWwindow* window) {

	//botRaceStarted = true;

	while (!glfwWindowShouldClose(window))
	{
		processInput(window);

		//if (botRaceStarted)
		//{
			// Kod do obs�ugi ruchu bot�w
			float tBot1 = (glfwGetTime() * 0.5f - 3.0f);
			float tBot2 = (glfwGetTime() * 0.3f - 1.0f);
			
			//spaceshipPos = Bot1Pos;
			//std::cout << "tBot1: " << tBot1 << ", " << "Time: " << raceStartTime << ")\n";
			
			if (tBot1 < 1.0f) {
				Bot1Pos = CalculateCatmullRomSpline(tBot1, p01, p01, p01, p01);
			}
			else if (tBot1 < 2.0f) {
				Bot1Pos = CalculateCatmullRomSpline(tBot1 - 1.0f, p01, p01, p01, p1);
			}
			else if (tBot1 < 3.0f) {
				Bot1Pos = CalculateCatmullRomSpline(tBot1 - 2.0f, p01, p01, p1, p2);
			}
			else if (tBot1 < 4.0f) {
				Bot1Pos = CalculateCatmullRomSpline(tBot1 - 3.0f, p01, p1, p2, p3);
			}
			else if (tBot1 < 5.0f) {
				Bot1Pos = CalculateCatmullRomSpline(tBot1 - 4.0f, p1, p2, p3, p4);
			}
			else if (tBot1 < 6.0f) {
				Bot1Pos = CalculateCatmullRomSpline(tBot1 - 5.0f, p2, p3, p4, p5);
			}
			else if (tBot1 < 7.0f) {
				Bot1Pos = CalculateCatmullRomSpline(tBot1 - 6.0f, p3, p4, p5, checkpointPos[5]);
				if (checkpointReached(4, Bot1Pos) && !checkpointReached(4, spaceshipPos)) {
					Bot1End = true;
					Bot1Pos = glm::vec3(2.0f, 1.0f, -2.0f);

				}
			}

			
			if (tBot2 < 1.0f) {
				Bot2Pos = CalculateCatmullRomSpline(tBot2, p02, p02, p02, p02);
			}
			else if (tBot2 < 2.0f) {
				Bot2Pos = CalculateCatmullRomSpline(tBot2 - 1.0f, p02, p02, p02, p1);
			}
			else if (tBot2 < 3.0f) {
				Bot2Pos = CalculateCatmullRomSpline(tBot2 - 2.0f, p02, p02, p1, p2);
			}
			else if (tBot2 < 4.0f) {
				Bot2Pos = CalculateCatmullRomSpline(tBot2 - 3.0f, p02, p1, p2, p3);
			}
			else if (tBot2 < 5.0f) {
				Bot2Pos = CalculateCatmullRomSpline(tBot2 - 4.0f, p1, p2, p3, p4);
			}
			else if (tBot2 < 6.0f) {
				Bot2Pos = CalculateCatmullRomSpline(tBot2 - 5.0f, p2, p3, p4, p5);
			}
			else if (tBot2 < 7.0f) {
				Bot2Pos = CalculateCatmullRomSpline(tBot2 - 6.0f, p3, p4, p5, checkpointPos[5]);
				if (checkpointReached(4, Bot2Pos) && !checkpointReached(4, spaceshipPos)) {
					Bot2End = true;
					Bot2Pos = glm::vec3(-2.0f, 1.0f, -2.0f);

				}
			}
		//}


		renderScene(window);
		
		glfwPollEvents();
	}
}
//}