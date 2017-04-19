#include <iostream>
#include <algorithm>
#include <vector>


#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <GL/gl.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/string_cast.hpp>

#include "rotator.h"
#include "texture.h"
#include "cloth.h"
#include "LoadShaders.h"

#define HEIGHT 1000
#define WIDTH 1200

using namespace std;
using namespace glm;

GLuint program;
GLint texLoc;
float currentTime, deltaTime, lastTime = 0.0f;

vec3 initcamPos(0.f, 59.f, 192.f);
vec3 initscene_center(0.f, 10.f, 0.f);
vec3 initViewUP(0.0f, -1.0f, 0.0f);

mat4 viewMatrix;

// GLfloat nearVal = 1.0f, farVal = 10000.0f;
bool wireFrame = false;
bool north = false, south = false, west = false, east = false;
bool top = false, down = false;
bool rotateViewClock = false, rotateViewCounterClock = false;
bool rotateGazeClock = false, rotateGazeCounterClock = false;
// int initWidth = 800, initHeight = 800, currWidth, currHeight;
vec3 camPos(0.f, 59.f, 192.f);
vec3 scene_center(0.f, 10.f, 0.f);
vec3 viewUP(0.0f, -1.0f, 0.0f);

void cbfun(GLFWwindow* window, int key, int scancode, int action, int mods) {
	vec3 gazeVector = scene_center - camPos;  // Gaze Vector
	vec3 gaze = gazeVector / length(gazeVector);  // GazeVector/Magnitude(W-Axis)

	vec3 uAxis = cross(viewUP, -gaze);  // u-axis
	vec3 u = uAxis / length(uAxis);

	vec3 vAxis = cross(-gazeVector, uAxis);  // v-axis

	if (action == GLFW_PRESS || action != GLFW_RELEASE) {
		if (key == GLFW_KEY_ESCAPE) {
			exit(EXIT_SUCCESS);
		}

		else {
			switch (key) {
				case GLFW_KEY_I:
					camPos = initcamPos;
					scene_center = initscene_center;
					viewUP = initViewUP;
					// viewMatrix = lookAt(camPos, scene_center, viewUP);
					break;

				case GLFW_KEY_O:
					if (wireFrame)
						wireFrame = false;
					else
						wireFrame = true;
					break;

				case GLFW_KEY_UP:
					if (north == true) {
						camPos += 1.5f * gaze;
						scene_center += 1.5f * gaze;
						// viewMatrix = lookAt(camPos, scene_center, viewUP);
						north = false;
					}
					else
						north = true;
					break;

				case GLFW_KEY_DOWN:
					if (south == true) {
						camPos -= 2.0f * gaze;
						scene_center -= 2.0f * gaze;
						// viewMatrix = lookAt(camPos, scene_center, viewUP);
						south = false;
					}
					else
						south = true;
					break;

				case GLFW_KEY_LEFT:
					if (east == true) {
						camPos -= 2.0f * u;
						scene_center -= 2.0f * u;
						// viewMatrix = lookAt(camPos, scene_center, viewUP);
						east = false;
					}
					else
						east = true;
					break;

				case GLFW_KEY_RIGHT:
					if (west == true) {
						camPos += 2.0f * u;
						scene_center += 2.0f * u;
						// viewMatrix = lookAt(camPos, scene_center, viewUP);
						west = false;
					}
					else
						west = true;
					break;

				case GLFW_KEY_R:
					if (top == true) {
						camPos -= 0.5f * viewUP;
						scene_center -= 0.5f * viewUP;
						// viewMatrix = lookAt(camPos, scene_center, viewUP);
						top = false;
					}
					else
						top = true;
					break;

				case GLFW_KEY_T:
					if (down == true) {
						camPos += 0.5f * viewUP;
						scene_center += 0.5f * viewUP;
						// viewMatrix = lookAt(camPos, scene_center, viewUP);
						down = false;
					}
					else
						down = true;
					break;

				case GLFW_KEY_Z:
					if (rotateViewClock == true) {
						mat4 matrix = rotate(mat4(1.0f), -(1.0f * 3.14f) / 180.0f, viewUP);
						camPos = vec3(matrix * vec4(camPos, 1.0f));
						scene_center = vec3(matrix * vec4(scene_center, 1.0f));
						// viewMatrix = lookAt(camPos, scene_center, viewUP);
						rotateViewClock = false;
					}
					else
						rotateViewClock = true;
					break;

				case GLFW_KEY_X:
					if (rotateViewCounterClock == true) {
						mat4 matrix = rotate(mat4(1.0f), (1.0f * 3.14f) / 180.0f, viewUP);
						camPos = vec3(matrix * vec4(camPos, 1.0f));
						scene_center = vec3(matrix * vec4(scene_center, 1.0f));
						// viewMatrix = lookAt(camPos, scene_center, viewUP);
						rotateViewCounterClock = false;
					}
					else
						rotateViewCounterClock = true;
					break;

				case GLFW_KEY_C:
					if (rotateGazeClock == true) {
						mat4 matrix = rotate(mat4(1.0f), (1.0f * 3.14f) / 180.0f, gaze);
						viewUP = vec3(matrix * vec4(viewUP, 1.0f));
						// viewMatrix = lookAt(camPos, scene_center, viewUP);
						rotateGazeClock = false;
					}
					else
						rotateGazeClock = true;
					break;

				case GLFW_KEY_V:
					if (rotateGazeCounterClock == true) {
						mat4 matrix = rotate(mat4(1.0f), -(1.0f * 3.14f) / 180.0f, gaze);
						viewUP = vec3(matrix * vec4(viewUP, 1.0f));
						// viewMatrix = lookAt(camPos, scene_center, viewUP);
						rotateGazeCounterClock = false;
					}
					else
						rotateGazeCounterClock = true;
					break;

				default:
					return;
			}
		}
	}
}


void display(GLFWwindow* window, MouseRotator rotator, Cloth* cloth, Texture* carpet) {
	vec3 clear_color = vec3(0.3, 0.45, 0.6);
	glClearColor(clear_color.x, clear_color.y, clear_color.z, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glClearDepthf(1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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

	// cout << "Campos" << to_string(vec3(camPos));
	// vec3 scene_center(0.0f, 10.0f, 0.0f);
	viewMatrix = lookAt(vec3(camPos), scene_center, viewUP);
	P = perspective(50.0f, float(WIDTH / HEIGHT), 0.1f, 4000.0f);
	MV = viewMatrix * M;

	// vec3 lDir = vec3(1.0f, 0.0f, 0.0f);

	// Send uniform variables
	// glProgramUniform1f(program, time_Loc, currentTime);
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
	if (!glfwGetKey(window, GLFW_KEY_P)) cloth->update(window, deltaTime);
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

	// glfwSetKeyCallback(window, cbfun);

	while (!glfwWindowShouldClose(window)) {
		rotator.poll(window);
		display(window, rotator, cloth, carpet);
		glfwSwapInterval(0);
		glfwSwapBuffers(window);
		glfwPollEvents();
		if (glfwGetKey(window, GLFW_KEY_ESCAPE)) break;
	}

	glfwDestroyWindow(window);

	glfwTerminate();

	return 0;
}
