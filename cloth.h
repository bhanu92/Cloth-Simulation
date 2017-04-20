#pragma once
#include <vector>
#include <glm/glm.hpp>


#include <GL/glew.h>
#include <GLFW/glfw3.h>

using namespace glm;
using namespace std;

class Directions {
      public:
	const int WEST = 1;
	const int NORTHWEST = 2;
	const int NORTH = 3;
	const int NORTHEAST = 4;
	const int EAST = 5;
	const int SOUTHEAST = 6;
	const int SOUTH = 7;
	const int SOUTHWEST = 8;
};

class Cloth {
      public:
	Cloth();
	Cloth(const int rx, const int rz, const int w, const int l);
	~Cloth();
	void draw(GLFWwindow* window);
	void update(GLFWwindow* window, float delta);
	vec3 gravity = vec3(0, -9.82f, 0);

      private:
	vector<vec3> initPositions;
	vector<vec3> prevPositions;
	vector<vec3> vertices;
	vector<vec3> normals;
	vector<vec2> uvs;
	vector<vec3> forces;
	vector<vec3> velocities;
	vector<unsigned int> indices;
	GLuint VAO, IBO, VBO, NBO, UVBO;
	enum Attrib_IDs { vPosition1, vPosition2, vPosition3 };
	int getVertex(int direction, int vertex);
	vec3 getSpringForce(int direction, int vertex);
	int width, height;
	const int xParticles, zParticles;
	bool playSimulation = false;
	float restLengthX, restLengthZ, restLengthXZ = 0;
	float springFactor = 1500.f;
	float dampingFactor = 0.996f;
	float airResistance = .05f;

	Directions directions;
	void Forces(GLFWwindow* window);
	void verletIntegration(float delta, int n_iterations);
	void eulerIntegration(float delta);
};
