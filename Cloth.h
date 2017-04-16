#pragma once
#include <vector>
#include <glm/glm.hpp>


#include "GL/glew.h"
#include <GLFW/glfw3.h>


class Directions {
public:
const int WEST = 0;
const int NORTHWEST = 1;
const int NORTH = 2;
const int NORTHEAST = 3;
const int EAST = 4;
const int SOUTHEAST = 5;
const int SOUTH = 6;
const int SOUTHWEST = 7;
};

class Cloth
{
public:
Cloth();
Cloth(const int rx, const int rz, const int w, const int l);
~Cloth();
void draw(GLFWwindow *window);
void updateSimulation(float dt, GLFWwindow *window);

private:

void accumulateForces(GLFWwindow *window);
void verletIntegration(float dt, int n_iterations);
void forwardEulerIntegration(float dt);

Directions DIRS;

int getId(int direction, int id);
glm::vec3 getSpringForce(int direction, int id);

void create_plane();
std::vector<glm::vec3> vertices;
std::vector<glm::vec3> normals;
std::vector<glm::vec2> uvs;
std::vector<unsigned int> indices;

// Properties
std::vector<glm::vec3> forces;
std::vector<glm::vec3> velocities;

std::vector<glm::vec3> init_positions;
std::vector<glm::vec3> prev_positions;

bool playSimulation = false;
float restLengthX, restLengthZ, restLengthXZ = 0;
float spring_factor = 2000.f;
float damping_factor = 0.996f;
float wind_factor = 0.0f;
float air_resistance = .05f;
glm::vec3 gravity = glm::vec3(0, -9.82f, 0);

GLuint VAO, IBO, VBO, NBO, UVBO;

int width, length;
const int res_x, res_z;
};