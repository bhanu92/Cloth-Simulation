#pragma once
#include <vector>
#include <glm/glm.hpp>


#include <GL/glew.h>
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

class Cloth {
      public:
	Cloth();
	Cloth(const int rx, const int rz, const int w, const int l);
	~Cloth();
	void draw(GLFWwindow* window);
	void update(GLFWwindow* window, float delta);

      private:
	void Forces(GLFWwindow* window);
	void verletIntegration(float delta, int n_iterations);
	void eulerIntegration(float delta);

	Directions directions;
	int getVertex(int direction, int vertex);
	glm::vec3 getSpringForce(int direction, int vertex);

	// Properties
	std::vector<glm::vec3> forces;
	std::vector<glm::vec3> velocities;

	std::vector<glm::vec3> initPositions;
	std::vector<glm::vec3> prevPositions;

	bool playSimulation = false;
	float restLengthX, restLengthZ, restLengthXZ = 0;
	float springFactor = 1500.f;
	float dampingFactor = 0.996f;
	float airResistance = .05f;
	glm::vec3 gravity = glm::vec3(0, -9.82f, 0);

	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> uvs;
	std::vector<unsigned int> indices;
	GLuint VAO, IBO, VBO, NBO, UVBO;
	enum Attrib_IDs { vPosition1, vPosition2, vPosition3 };

	int width, height;
	const int res_x, res_z;
};
