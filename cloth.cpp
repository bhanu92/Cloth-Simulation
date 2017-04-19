#include <iostream>
#include "cloth.h"

using namespace glm;
using namespace std;

Cloth::Cloth(const int rx, const int rz, int w, int l) : res_x(rx), res_z(rz), width(w), length(l) {
	float height = 20.0f;

	float leap_x = (float)width / (float)res_x;
	float offset_x = (float)width / 2.0f - (float)leap_x / 2.0f;  // center

	float leap_z = (float)length / (float)res_z;
	float offset_z = (float)length / 2.0f - (float)leap_z / 2.0f;  // center

	restLengthX = leap_x;
	restLengthZ = leap_z;
	restLengthXZ = sqrt(pow(leap_x, 2) + pow(leap_z, 2));

	for (float z = 0; z < res_z; z += 1.0f) {
		for (float x = 0; x < res_x; x += 1.0f) {
			vertices.push_back(vec3(x * leap_x - offset_x, height, z * leap_z - offset_z));
			initPositions.push_back(vec3(x * leap_x - offset_x, height, z * leap_z - offset_z));
			normals.push_back(vec3(0, 1, 0));
			uvs.push_back(vec2(x / res_x, z / res_z));

			// Properties
			forces.push_back(vec3(0));
			velocities.push_back(vec3(0));
			prevPositions.push_back(vertices[vertices.size() - 1]);
		}
	}

	for (int i = 0; i < res_x - 1; i++) {
		for (int j = 0; j < res_z - 1; j++) {
			// Triangle 1
			indices.push_back(i * res_x + j);
			indices.push_back((i + 1) * res_x + j);
			indices.push_back(i * res_x + (j + 1));

			// Triangle 2
			indices.push_back((i + 1) * res_x + j);
			indices.push_back((i + 1) * res_x + (j + 1));
			indices.push_back((i)*res_x + (j + 1));
		}
	}
	for (int i = 0; i < indices.size(); i++) cout << indices.at(i) << endl;
	cout << indices.size() << endl;
	cout << vertices.size() << endl;

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	// Vertices(Particles for the cloth)
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float) * 3, &vertices[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);
	glEnableVertexAttribArray(vPosition1);

	// Normals
	glGenBuffers(1, &NBO);
	glBindBuffer(GL_ARRAY_BUFFER, NBO);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(float) * 3, &normals[0], GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);
	glEnableVertexAttribArray(vPosition2);

	// UV's for texture
	glGenBuffers(1, &UVBO);
	glBindBuffer(GL_ARRAY_BUFFER, UVBO);
	glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(float) * 2, &uvs[0], GL_STATIC_DRAW);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);
	glEnableVertexAttribArray(vPosition3);
}


Cloth::~Cloth() {}

int Cloth::getDirection(int direction, int id) {
	if (directions.WEST == direction) {
		return id - 1;
	}
	else if (directions.NORTHWEST == direction) {
		return id + res_x - 1;
	}
	else if (directions.NORTH == direction) {
		return id + res_x;
	}
	else if (directions.NORTHEAST == direction) {
		return id + res_x + 1;
	}
	else if (directions.EAST == direction) {
		return id + 1;
	}
	else if (directions.SOUTHEAST == direction) {
		return id - res_x + 1;
	}
	else if (directions.SOUTH == direction) {
		return id - res_x;
	}
	else {
		return id - res_x - 1;
	}
}

vec3 Cloth::getSpringForce(int direction, int id) {
	float restLength = 0;

	if (direction == directions.WEST || direction == directions.EAST)
		restLength = restLengthX;

	else if (direction == directions.NORTH || direction == directions.SOUTH)
		restLength = restLengthZ;

	else
		restLength = restLengthXZ;

	vec3 delta = vertices[id] - vertices[getDirection(direction, id)];
	float deltaLength = glm::length(delta);  // distance
	float diff = (deltaLength - restLength) / deltaLength;

	return delta * diff * springFactor;
}


// Force equation from https://graphics.stanford.edu/~mdfisher/cloth.html#EquationsOfMotion
// F(v) = Mg + Fwind + Fairresistance - k*sum(x_current - x_rest)
void Cloth::Forces(GLFWwindow* window) {
	for (int v = 0; v < vertices.size(); v++) {
		vec3 wind = vec3(0, 0, 0);
		vec3 spring = vec3(0, 0, 0);

		std::vector<vec3> spring_directions;

		if (v % res_x != 0) {  // WEST
			vec3 force = getSpringForce(directions.WEST, v);
			spring += force;
			spring_directions.push_back(
			    normalize(vertices[v] - vertices[getDirection(directions.WEST, v)]));
		}

		if (v < vertices.size() - res_x) {  // NORTH

			if (v % res_x != 0) {
				vec3 force = getSpringForce(directions.NORTHWEST, v);
				spring += force;
				spring_directions.push_back(
				    normalize(vertices[v] - vertices[getDirection(directions.NORTHWEST, v)]));
			}

			vec3 force = getSpringForce(directions.NORTH, v);
			spring += force;
			spring_directions.push_back(
			    normalize(vertices[v] - vertices[getDirection(directions.NORTH, v)]));

			if ((v + 1) % res_x != 0) {
				vec3 force = getSpringForce(directions.NORTHEAST, v);
				spring += force;
				spring_directions.push_back(
				    normalize(vertices[v] - vertices[getDirection(directions.NORTHEAST, v)]));
			}
		}

		if ((v + 1) % res_x != 0) {  // EAST
			vec3 force = getSpringForce(directions.EAST, v);
			spring += force;
			spring_directions.push_back(
			    normalize(vertices[v] - vertices[getDirection(directions.EAST, v)]));
		}

		if (v > res_x - 1) {  // SOUTH

			if ((v + 1) % res_x != 0) {
				vec3 force = getSpringForce(directions.SOUTHEAST, v);
				spring += force;
				spring_directions.push_back(
				    normalize(vertices[v] - vertices[getDirection(directions.SOUTHEAST, v)]));
			}

			vec3 force = getSpringForce(directions.SOUTH, v);
			spring += force;
			spring_directions.push_back(
			    normalize(vertices[v] - vertices[getDirection(directions.SOUTH, v)]));

			if (v % res_x != 0) {
				vec3 force = getSpringForce(directions.SOUTHWEST, v);
				spring += force;
				spring_directions.push_back(
				    normalize(vertices[v] - vertices[getDirection(directions.SOUTHWEST, v)]));
			}
		}

		// Calculate normals
		vec3 normal = vec3(0);
		for (int i = 1; i < spring_directions.size(); i++) {
			normal += cross(spring_directions[i], spring_directions[i - 1]);
		}
		normals[v] = normalize(normal);

		// Wind
		wind.x = sin(vertices[v].x * vertices[v].y * glfwGetTime());
		wind.y = cos(vertices[v].z * glfwGetTime());
		wind.z = sin(cos(5 * vertices[v].x * vertices[v].y * vertices[v].z));


		// Wind Resistance
		vec3 F_airResistance = -airResistance * velocities[v] * abs(dot(normals[v], velocities[v]));

		// Controls
		if (glfwGetKey(window, GLFW_KEY_SPACE)) {
			playSimulation = true;
		}

		if (glfwGetKey(window, GLFW_KEY_H)) {
			vec3 dir = vec3(20, 0, 10);
			wind = dir * glm::length(normals[v] * normalize(dir));
		}

		if (glfwGetKey(window, GLFW_KEY_F)) {
			for (int v = 0; v < vertices.size(); v++) {
				vertices[v] = initPositions[v];
				velocities[v] = vec3(0);
				prevPositions[v] = vertices[v];
			}
		}

		if (glfwGetKey(window, GLFW_KEY_G)) gravity *= -1.0f;

		if (playSimulation) {
			forces[v] = wind + F_airResistance + gravity - spring;
		}


		// Pinned vertices
		if (v > vertices.size() - res_x - 1) {
			if (v % res_x == 0) {
				forces[v] = vec3(0);
			}

			if ((v + 1) % res_x == 0) {
				forces[v] = vec3(0);
			}
		}
	}
}

void Cloth::eulerIntegration(float delta) {
	for (int v = 0; v < vertices.size(); v++) {
		vec3 acceleration = forces[v] * 1.0f;  // mass
		velocities[v] = dampingFactor * velocities[v] + acceleration * delta;
		vertices[v] = vertices[v] + velocities[v] * delta;
	}
}

void Cloth::verletIntegration(float delta, int n_iterations) {
	for (int v = 0; v < vertices.size(); v++) {
		vec3 position_prim = vec3(0.f);
		vec3 acceleration = forces[v] * 1.0f;  // mass
		for (int i = 0; i < n_iterations; i++) {
			position_prim = vertices[v] + ((vertices[v] - prevPositions[v]) * dampingFactor) +
			                (acceleration * delta * delta);
			prevPositions[v] = vertices[v];
		}
		vertices[v] = position_prim;
	}
}

// Updating the simualtion every frame according to the forces
void Cloth::update(GLFWwindow* window, float delta) {
	Forces(window);
	// verletIntegration(delta, 1);
	eulerIntegration(delta);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float) * 3, &vertices[0], GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, NBO);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(float) * 3, &normals[0], GL_DYNAMIC_DRAW);
}


void Cloth::draw(GLFWwindow* window) {
	glLineWidth(1.0);
	glPointSize(4.0);

	glGenBuffers(1, &IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	if (glfwGetKey(window, GLFW_KEY_W)) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	if (glfwGetKey(window, GLFW_KEY_Q)) glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);

	// glBindVertexArray(VAO);

	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	// glDrawArrays(GL_TRIANGLES, 0, vertices.size());
}
