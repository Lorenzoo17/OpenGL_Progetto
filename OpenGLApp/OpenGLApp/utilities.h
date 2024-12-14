#ifndef UTILITIES_H
#define UTILITIES

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Utilities {
public:
	static bool CheckDistance(glm::vec3 v1, glm::vec3 v2, float distance);
	static glm::vec3 NormalizeVector(glm::vec3 v);
};

#endif // !UTILITIES_H

