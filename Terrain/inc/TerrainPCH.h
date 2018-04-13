#pragma once;

#define NOMINMAX
#include<Windows.h>
#include <algorithm>
#include <ctime>
#include <fstream>
#include <filesystem>
#include <iostream>
#include <string>
#include <vector>
#include <sstream>

namespace fs = std::experimental::filesystem;

#define GLEW_STATIC
#include <gl/glew.h>
#include <gl/glut.h>

#define GLM_SWIZZLE_XYZW
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/swizzle.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/std_based_type.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <glm/gtx/compatibility.hpp>
#include <glm/gtx/matrix_operation.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
//#include<bass.h>
#include <SOIL.h>