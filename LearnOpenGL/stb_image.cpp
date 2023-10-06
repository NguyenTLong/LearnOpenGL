#pragma once
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

int width, height, nrChannerls;
unsigned char* data = stbi_load("container.jpg", &width, &height, &nrChannerls, 0);
