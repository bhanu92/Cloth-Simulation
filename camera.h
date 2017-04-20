#pragma once

#include <GLFW/glfw3.h>
#include <cmath>

#define PI 3.14

class KeyTranslator {
      public:
	float horizontal;
	float zoom;

      private:
	double lastTime;

      public:
	void init(GLFWwindow* window);
	void poll(GLFWwindow* window);
};

class MouseRotator {
      public:
	float phi;
	float theta;
	float transX;
	float transY;
	float zoom;

      private:
	double lastX;
	double lastY;
	int lastLeft;
	int lastRight;
	int lastMiddle;
	bool rotStarted;

      public:
	void init(GLFWwindow* window);
	void poll(GLFWwindow* window);
};
