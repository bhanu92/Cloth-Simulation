#include "texture.h"
#include <iostream>


using namespace std;


Texture::Texture(const char* path, int w, int h) : width(w), height(h) {
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glActiveTexture(GL_TEXTURE0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	int channels;
	cout << "Image Path: " << path << endl;
	unsigned char* image = SOIL_load_image(path, &width, &height, &channels, SOIL_LOAD_AUTO);

	if (image == NULL) {
		cerr << "Failed Image Load" << endl;
		exit(EXIT_FAILURE);
	}

	// Flipping image values around yaxis
	for (int j = 0; j * 2 < height; ++j) {
		int index1 = j * width * channels;
		int index2 = (height - 1 - j) * width * channels;
		for (int i = width * channels; i > 0; --i) {
			unsigned char temp = image[index1];
			image[index1] = image[index2];
			image[index2] = temp;
			++index1;
			++index2;
		}
	}

	if (channels == 3)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	else if (channels == 4) {
		// image = SOIL_load_image(path, &width, &height, &channels, SOIL_LOAD_RGBA);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	}

	SOIL_free_image_data(image);
}

Texture::~Texture() {}
