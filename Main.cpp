#include <iostream>
#include <algorithm>
#include <vector>


#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <GL/gl.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

#include "Rotator.h"
#include "Texture.h"
#include "Cloth.h"
#include "LoadShaders.h"

using namespace std;
using namespace glm;

#define HEIGHT 1000
#define WIDTH 1200

GLuint program;
GLint texLoc;
float currentTime, deltaTime, lastTime = 0.0f;

void display(GLFWwindow* window, MouseRotator rotator, Cloth* cloth, Texture* carpet) {
	vec3 clear_color = vec3(0.3, 0.45, 0.6);
	glClearColor(clear_color.x, clear_color.y, clear_color.z, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  // We're not using stencil buffer now
	glEnable(GL_DEPTH_TEST);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_TEXTURE_2D);

	currentTime = glfwGetTime();
	deltaTime = currentTime - lastTime;
	lastTime = currentTime;

	glUseProgram(program);
	// Uniforms
	GLint MV_Loc, P_Loc, lDir_Loc, camPos_Loc, clear_color_Loc, time_Loc, window_dim_Loc = -1;
	// time_Loc = glGetUniformLocation(program, "time");
	MV_Loc = glGetUniformLocation(program, "MV");
	P_Loc = glGetUniformLocation(program, "P");
	camPos_Loc = glGetUniformLocation(program, "camPos");
	// lDir_Loc = glGetUniformLocation(program, "lDir");
	// clear_color_Loc = glGetUniformLocation(program, "clear_color");
	// window_dim_Loc = glGetUniformLocation(program, "window_dim");

	vec2 window_dim = vec2(WIDTH, HEIGHT);

	mat4 MV, P;
	mat4 M = mat4(1.0f);
	mat4 VRotX = rotate(M, (rotator.phi), vec3(0.0f, -1.0f, 0.0f));  // Rotation about y-axis
	mat4 VRotY = rotate(M, (rotator.theta + 0.2f), vec3(-1.0f, 0.0f, 0.0f));  // Rotation about x-axis
	vec4 camPos = VRotX * VRotY * vec4(0.0f, 20.0f, 200.0f + 0.2f * rotator.zoom, 1.0f);
	vec3 scene_center(0.0f, 10.0f, 0.0f);
	mat4 V = lookAt(vec3(camPos), scene_center, vec3(0.0f, -1.0f, 0.0f));
	P = perspectiveFov(50.0f, static_cast<float>(WIDTH), static_cast<float>(HEIGHT), 0.1f, 4000.0f);
	MV = V * M;

	vec3 lDir = vec3(1.0f, 0.0f, 0.0f);

	// Send uniform variables
	glProgramUniform1f(program, time_Loc, currentTime);
	glUniformMatrix4fv(MV_Loc, 1, GL_FALSE, &MV[0][0]);
	glUniformMatrix4fv(P_Loc, 1, GL_FALSE, &P[0][0]);
	glUniform3fv(camPos_Loc, 1, &camPos[0]);
	// glUniform3fv(lDir_Loc, 1, &lDir[0]);
	// glUniform3fv(clear_color_Loc, 1, &clear_color[0]);
	// glUniform2fv(window_dim_Loc, 1, &window_dim[0]);

	texLoc = glGetUniformLocation(program, "carpetTexture");
	glUniform1i(texLoc, 0);
	glActiveTexture(GL_TEXTURE0);
	carpet->bindTexture();
	if (!glfwGetKey(window, GLFW_KEY_P)) cloth->updateSimulation(deltaTime, window);
	cloth->draw(window);
}


int main() {
	if (!glfwInit()) {
		cerr << "GLFW initialization failure" << endl;
		exit(EXIT_FAILURE);
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Cloth Simulation", NULL, NULL);

	// glfwSetWindowPos(window, 250, 400);

	// Define the viewport dimensions

	glfwMakeContextCurrent(window);  // Initialize GLEW
	glewExperimental = true;  // Needed in core profile
	if (glewInit() != GLEW_OK) {
		cerr << "GLEW initialization failure" << endl;
		exit(EXIT_FAILURE);
	}

	vec3 clear_color = vec3(0.3, 0.45, 0.6);

	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	// Controls
	MouseRotator rotator;
	rotator.init(window);

	// Create cloth
	Cloth* cloth = new Cloth(35, 35, 15, 15);
	Texture* carpet = new Texture("cat.jpg");

	ShaderInfo shaders[] = {
	    {GL_VERTEX_SHADER, "shaders/cloth.vert"}, {GL_FRAGMENT_SHADER, "shaders/cloth.frag"}, {GL_NONE, NULL}};

	program = LoadShaders(shaders);
	if (program == NULL) {
		cerr << "Failed shader load" << endl;
		exit(EXIT_FAILURE);
	}

	do {
		rotator.poll(window);
		display(window, rotator, cloth, carpet);
		glfwSwapInterval(0);
		glfwSwapBuffers(window);
		glfwPollEvents();


	} while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0);

	glfwDestroyWindow(window);

	glfwTerminate();

	return 0;
}
