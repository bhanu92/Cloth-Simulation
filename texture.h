#pragma once

#include "GL/glew.h"
#include <GLFW/glfw3.h>
#include <SOIL/SOIL.h>


class Texture {
      public:
	Texture(const char* path);
	Texture(const char* path, int w, int h);
	~Texture();
	GLuint texture;

      private:
	int width, height;
	unsigned char* image;
};
