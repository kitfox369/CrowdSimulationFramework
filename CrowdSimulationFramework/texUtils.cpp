
#include <iostream>
#include "texUtils.h"

#define _USE_MATH_DEFINES
#include <cmath> 

#include <ctime>


#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

GLuint globaltextureID = 0;

GLuint loadImage(const char* theFileName)
{
	GLuint textureID;

	std::string filename = std::string(theFileName);

	std::string fullname;
	fullname = std::string(filename);


	//glActiveTexture(GL_TEXTURE0 + globaltextureID);
	glActiveTexture(GL_TEXTURE0);

	glGenTextures(1, &textureID);

	int width, height, channel;

	unsigned char* image = stbi_load(fullname.c_str(), &width, &height, &channel, 0);


	if (!image) {
		std::cout << theFileName << " : Loading Error " << std::endl;

	}

	// Generate a new texture
	glGenTextures(1, &textureID);

	// Bind the texture to a name
	glBindTexture(GL_TEXTURE_2D, textureID);

	// Set texture clamping method

	// Set texture interpolation method to use linear interpolation (no MIPMAPS)
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);


	if (channel == 4)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	else
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);


	glGenerateMipmap(GL_TEXTURE_2D);


	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);



	stbi_image_free(image);

	std::cout << "Texture creation successful." << fullname.c_str() << std::endl;

	globaltextureID++;

	return textureID; // Return the GLuint to the texture so you can use it!
}
//DevIL

//https://github.com/Secumfex/VoxelizationFramework/blob/master/src/executables/VoxelizationCompute/main.cpp
std::vector< float > generateRandomSamplingPattern(int numSamples, float r_max = 1.0f)
{
	srand(time(0));
	std::vector< float > result;

	// total weight to be distributed between samples
	float totalWeight = 0.0f;

	for (int i = 0; i < numSamples; i++)
	{
		// generate uniform random number
		float r1 = (float)std::rand() / (float)RAND_MAX; // 0..1
		float r2 = (float)std::rand() / (float)RAND_MAX; // 0..1

		// generate polar coordinates and weight
		float x = r_max * r1 * sin(2.0f * M_PI * r2);
		float y = r_max * r1 * cos(2.0f * M_PI * r2);
		float weight = r1 * r1;

		totalWeight += weight;

		// save generated values
		result.push_back(x);
		result.push_back(y);
		result.push_back(weight);
	}

	// normalize weights so sum == 1.0
	for (unsigned int i = 0; i < result.size() / 3; i++)
	{
		result[i * 3 + 2] /= totalWeight;
	}

	return result;
}