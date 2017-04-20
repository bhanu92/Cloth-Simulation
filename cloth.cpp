#include <iostream>
#include "cloth.h"

using namespace glm;
using namespace std;

Cloth::Cloth(const int rx, const int rz, int w, int h) : xParticles(rx), zParticles(rz), width(w), height(h) {
	float height = 15.0f;

	float leap_x = (float)width / (float)xParticles;
	float offset_x = ((float)width - (float)leap_x) / 2.0f;

	float leap_z = (float)height / (float)zParticles;
	float offset_z = ((float)height - (float)leap_z) / 2.0f;  // center

	restLengthX = leap_x;
	restLengthZ = leap_z;
	restLengthXZ = sqrt(pow(leap_x, 2) + pow(leap_z, 2));

	for (float z = 0; z < zParticles; z += 1.0f) {
		for (float x = 0; x < xParticles; x += 1.0f) {
			vertices.push_back(vec3(x * leap_x - offset_x, height, z * leap_z - offset_z));
			initPositions.push_back(vec3(x * leap_x - offset_x, height, z * leap_z - offset_z));
			normals.push_back(vec3(0, 1, 0));
			uvs.push_back(vec2(x / xParticles, z / zParticles));

			// Properties
			forces.push_back(vec3(0));
			velocities.push_back(vec3(0));
			prevPositions.push_back(vertices[vertices.size() - 1]);
		}
	}
	// Indexed rendering
	for (int i = 0; i < xParticles - 1; i++) {
		for (int j = 0; j < zParticles - 1; j++) {
			// Triangle 1
			indices.push_back(i * xParticles + j);
			indices.push_back((i + 1) * xParticles + j);
			indices.push_back(i * xParticles + (j + 1));

			// Triangle 2
			indices.push_back((i + 1) * xParticles + j);
			indices.push_back((i + 1) * xParticles + (j + 1));
			indices.push_back((i)*xParticles + (j + 1));
		}
	}
	// for (int i = 0; i < indices.size(); i++) cout << indices.at(i) << endl;
	// cout << indices.size() << endl;
	// cout << vertices.size() << endl;

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	// Vertices(Particles for the cloth)
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vec3), &vertices[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), 0);
	glEnableVertexAttribArray(vPosition1);

	// Normals
	glGenBuffers(1, &NBO);
	glBindBuffer(GL_ARRAY_BUFFER, NBO);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(vec3), &normals[0], GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), 0);
	glEnableVertexAttribArray(vPosition2);

	// UV's for texture
	glGenBuffers(1, &UVBO);
	glBindBuffer(GL_ARRAY_BUFFER, UVBO);
	glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(vec2), &uvs[0], GL_STATIC_DRAW);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(vec2), 0);
	glEnableVertexAttribArray(vPosition3);
}


Cloth::~Cloth() {}

int Cloth::getVertex(int direction, int vertex) {
	if (directions.WEST == direction) {
		return vertex - 1;
	}
	else if (directions.NORTHWEST == direction) {
		return vertex + xParticles - 1;
	}
	else if (directions.NORTH == direction) {
		return vertex + xParticles;
	}
	else if (directions.NORTHEAST == direction) {
		return vertex + xParticles + 1;
	}
	else if (directions.EAST == direction) {
		return vertex + 1;
	}
	else if (directions.SOUTHEAST == direction) {
		return vertex - xParticles + 1;
	}
	else if (directions.SOUTH == direction) {
		return vertex - xParticles;
	}
	else {
		return vertex - xParticles - 1;
	}
}

vec3 Cloth::getSpringForce(int direction, int vertex) {
	float restLength = 0;

	if (direction == directions.WEST || direction == directions.EAST)
		restLength = restLengthX;

	else if (direction == directions.NORTH || direction == directions.SOUTH)
		restLength = restLengthZ;

	else
		restLength = restLengthXZ;

	vec3 delta = vertices[vertex] - vertices[getVertex(direction, vertex)];
	float currentLength = glm::length(delta);  // distance
	float diff = (currentLength - restLength);

	return normalize(delta) * diff * springFactor;
}


// Force equation from https://graphics.stanford.edu/~mdfisher/cloth.html#EquationsOfMotion
// F(v) = Mg + Fwind + Fairresistance - k*sum(x_current - x_rest)
void Cloth::Forces(GLFWwindow* window) {
	for (int v = 0; v < vertices.size(); v++) {
		vec3 wind = vec3(0, 0, 0);
		vec3 spring = vec3(0, 0, 0);

		std::vector<vec3> spring_directions;

		if (v % xParticles != 0) {  // WEST
			vec3 force = getSpringForce(directions.WEST, v);
			spring += force;
			spring_directions.push_back((vertices[v] - vertices[getVertex(directions.WEST, v)]));
		}

		if (v < vertices.size() - xParticles) {  // NORTH

			if (v % xParticles != 0) {
				vec3 force = getSpringForce(directions.NORTHWEST, v);
				spring += force;
				spring_directions.push_back(
				    (vertices[v] - vertices[getVertex(directions.NORTHWEST, v)]));
			}

			vec3 force = getSpringForce(directions.NORTH, v);
			spring += force;
			spring_directions.push_back((vertices[v] - vertices[getVertex(directions.NORTH, v)]));

			if ((v + 1) % xParticles != 0) {
				vec3 force = getSpringForce(directions.NORTHEAST, v);
				spring += force;
				spring_directions.push_back(
				    (vertices[v] - vertices[getVertex(directions.NORTHEAST, v)]));
			}
		}

		if ((v + 1) % xParticles != 0) {  // EAST
			vec3 force = getSpringForce(directions.EAST, v);
			spring += force;
			spring_directions.push_back((vertices[v] - vertices[getVertex(directions.EAST, v)]));
		}

		if (v > xParticles - 1) {  // SOUTH

			if ((v + 1) % xParticles != 0) {
				vec3 force = getSpringForce(directions.SOUTHEAST, v);
				spring += force;
				spring_directions.push_back(
				    (vertices[v] - vertices[getVertex(directions.SOUTHEAST, v)]));
			}

			vec3 force = getSpringForce(directions.SOUTH, v);
			spring += force;
			spring_directions.push_back((vertices[v] - vertices[getVertex(directions.SOUTH, v)]));

			if (v % xParticles != 0) {
				vec3 force = getSpringForce(directions.SOUTHWEST, v);
				spring += force;
				spring_directions.push_back(
				    (vertices[v] - vertices[getVertex(directions.SOUTHWEST, v)]));
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

		vec3 forceAirResistance = vec3(0);

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

		if (playSimulation) {
			forces[v] = wind + forceAirResistance + gravity - spring;
		}


		forces[vertices.size() - xParticles] = vec3(0);
		forces[vertices.size() - 1] = vec3(0);
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
	// verletIntegration(delta, 1);
	Forces(window);
	eulerIntegration(delta);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vec3), &vertices[0], GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, NBO);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(vec3), &normals[0], GL_DYNAMIC_DRAW);
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
