#include "glew.h"
#include <GLFW/glfw3.h>
#include "glm.hpp"
#include "ext.hpp"
#include <iostream>
#include <cmath>

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
}

GLuint program;
GLuint programSun;
GLuint programTex;
Core::Shader_Loader shaderLoader;

Core::RenderContext shipContext;
Core::RenderContext sphereContext;

Core::RenderContext checkpointContext;
Core::RenderContext place1Context;
Core::RenderContext s2Context;
Core::RenderContext s3Context;

// Checkpoint positions
const int numCheckpoints = 5;
std::vector<glm::vec3> checkpointPos = {
	glm::vec3(0.0f, 1.0f, 4.0f),
	glm::vec3(0.0f, 1.0f, 8.0f),
	glm::vec3(0.0f, 1.0f, 12.0f),
	glm::vec3(0.0f, 1.0f, 16.0f),
	glm::vec3(0.0f, 1.0f, 20.0f)
};

int currentCheckpointIndex = 0;

bool reachedCheckpoint = false;

std::vector<glm::vec3> colors = {
	glm::vec3(1.0f, 1.0f, 0.0f),
	glm::vec3(1.0f, 0.0f, 0.0f)
};

int colorIndex = 0;

glm::vec3 cameraPos = glm::vec3(-4.f, 0, 0);
glm::vec3 cameraDir = glm::vec3(1.f, 0.f, 0.f);


glm::vec3 spaceshipPos = glm::vec3(0.0f, 1.000000f, -3.0f);
glm::vec3 spaceshipDir = glm::vec3(0.0f, 0.000000f, 1.0f);

glm::vec3 Bot1Pos = glm::vec3(2.0f, 1.0f, -2.0f);
glm::vec3 Bot2Pos = glm::vec3(-2.0f, 1.0f, -2.0f);
glm::vec3 Bot1Dir = glm::normalize(glm::vec3(0.354510f, 0.000000f, 0.935054f));
glm::vec3 Bot2Dir = glm::normalize(glm::vec3(-0.354510f, 0.000000f, 0.935054f));

//glm::vec3 checkpointDir = glm::vec3(0.0f, 1.0f, 0.0f);

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
	float n = 0.05;
	float f = 20.;
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

void renderScene(GLFWwindow* window)
{
	glClearColor(0.0f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
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


	//drawObjectColor(shipContext,
	//	glm::translate(cameraPos + 1.5 * cameraDir + cameraUp * -0.5f) * inveseCameraRotrationMatrix * glm::eulerAngleY(glm::pi<float>()),
	//	glm::vec3(0.3, 0.3, 0.5)
	//	);
	// Player
	drawObjectColor(shipContext,
		glm::translate(spaceshipPos) * spaceshipCameraRotationMatrix * glm::eulerAngleY(glm::pi<float>()) * glm::scale(glm::vec3(0.1)),
		glm::vec3(0.3, 0.3, 0.5)
	);

	// Bot1
	drawObjectColor(shipContext,
		glm::translate(Bot1Pos) * glm::scale(glm::vec3(0.1)),
		glm::vec3(0.3, 0.3, 0.5)
	);

	// Bot2
	drawObjectColor(shipContext,
		glm::translate(Bot2Pos) * glm::scale(glm::vec3(0.1)),
		glm::vec3(0.3, 0.3, 0.5)
	);

	// Checkpoint 0
	drawObjectColor(checkpointContext,
		glm::translate(checkpointPos[currentCheckpointIndex]),
		colors[colorIndex]
	);

	// 1st place
	drawObjectColor(place1Context,
		glm::translate(glm::vec3(-20.f, -20.0f, -20.0f)),
		glm::vec3(1.0, 1.0, 0.0)
	);

	// Earth
	drawObjectTexture(sphereContext, glm::eulerAngleY(time / 3) * glm::translate(glm::vec3(4.f, 0, 0)) * glm::eulerAngleY(time) * glm::scale(glm::vec3(0.3f)), texture::earth);

	// Moon
	drawObjectTexture(sphereContext,
		glm::eulerAngleY(time / 3) * glm::translate(glm::vec3(4.f, 0, 0)) * glm::eulerAngleY(time) * glm::translate(glm::vec3(1.f, 0, 0)) * glm::scale(glm::vec3(0.1f)), texture::moon);

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

	loadModelToContext("./models/sphere.obj", sphereContext);
	loadModelToContext("./models/spaceship.obj", shipContext);
	loadModelToContext("./models/checkpoint.obj", checkpointContext);
	loadModelToContext("./models/place1.obj", place1Context);
	loadModelToContext("./models/s2.obj", s2Context);
	loadModelToContext("./models/s3.obj", s3Context);

	texture::earth = Core::LoadTexture("textures/earth.png");
	texture::moon = Core::LoadTexture("textures/moon.jpg");

}

void shutdown(GLFWwindow* window)
{
	shaderLoader.DeleteProgram(program);
	shaderLoader.DeleteProgram(programTex);
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
	if (spaceshipPos.x > -1.0f && spaceshipPos.x < 1.0f &&
		spaceshipPos.y > -1.26f && spaceshipPos.y < 1.9f &&
		spaceshipPos.z > 3.9f && spaceshipPos.z < 4.1f &&
		currentCheckpointIndex == 0)
	{
		currentCheckpointIndex = 1;
	}

	// Checkpoint 1
	if (spaceshipPos.x > -1.0f && spaceshipPos.x < 1.0f &&
		spaceshipPos.y > -1.26f && spaceshipPos.y < 1.9f &&
		spaceshipPos.z > 7.9f && spaceshipPos.z < 8.1f &&
		currentCheckpointIndex == 1)
	{
		currentCheckpointIndex = 2;
	}

	// Checkpoint 2
	if (spaceshipPos.x > -1.0f && spaceshipPos.x < 1.0f &&
		spaceshipPos.y > -1.26f && spaceshipPos.y < 1.9f &&
		spaceshipPos.z > 11.9f && spaceshipPos.z < 12.1f &&
		currentCheckpointIndex == 2)
	{
		currentCheckpointIndex = 3;
	}

	// Checkpoint 3
	if (spaceshipPos.x > -1.0f && spaceshipPos.x < 1.0f &&
		spaceshipPos.y > -1.26f && spaceshipPos.y < 1.9f &&
		spaceshipPos.z > 15.9f && spaceshipPos.z < 16.1f &&
		currentCheckpointIndex == 3)
	{
		currentCheckpointIndex = 4;
		colorIndex = 1;
	}


	// Finishing the race
	if (spaceshipPos.x > -1.0f && spaceshipPos.x < 1.0f &&
		spaceshipPos.y > -1.26f && spaceshipPos.y < 1.9f &&
		spaceshipPos.z > 19.9f && spaceshipPos.z < 20.1f &&
		currentCheckpointIndex == 4)
	{
		// Teleport
		cameraPos = glm::vec3(-20.0f, -20.0f, -17.5f);
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
	while (!glfwWindowShouldClose(window))
	{
		processInput(window);

		renderScene(window);
		
		glfwPollEvents();
	}
}
//}