#ifndef _COMPONENT_
#define _COMPONENT_

//Component list

#include<iostream>

#include<vector>
#include<math.h>
#include"Loader.h"

#include<glm\glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/mat4x4.hpp>

#include <string>

class Transform {
public:
	float Position[3];
	float Rotation[3];
	float Scale[3];
};

#endif