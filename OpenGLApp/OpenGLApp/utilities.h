#ifndef UTILITIES_H
#define UTILITIES

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "game_object.h"
//#include "irrKlang.h"
#include "resource_manager.h"

//using namespace irrklang;

enum Direction {
	DIR_UP,
	DIR_RIGHT,
	DIR_DOWN,
	DIR_LEFT
};

typedef std::tuple<bool, Direction, glm::vec3> Collision;

class Utilities {
public:
	static bool CheckDistance(glm::vec3 v1, glm::vec3 v2, float distance);
	static glm::vec3 NormalizeVector(glm::vec3 v);
	static Collision CheckCollision(GameObject& one, GameObject& two);
	static void PlaySound(std::string soundName);
	static Direction VectorDirection(glm::vec3 target);
	static glm::vec2 worldToScreen(glm::vec3 worldPos, glm::mat4 view, glm::mat4 projection, int screenWidth, int screenHeight);
	static bool clickOverObject(GameObject gameObjectPosition, float distance = 100.0f);
};

#endif // !UTILITIES_H

