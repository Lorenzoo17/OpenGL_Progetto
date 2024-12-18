#include "utilities.h"

bool Utilities::CheckDistance(glm::vec3 v1, glm::vec3 v2, float distance) {
	if (sqrt(pow((v2.x - v1.x), 2) + pow((v2.y - v1.y), 2) < distance)) {
		return true;
	}
	return false;
}

glm::vec3 Utilities::NormalizeVector(glm::vec3 v) {
	return v / sqrt(glm::dot(v, v));
}

bool Utilities::CheckCollision(GameObject& one, GameObject& two) // AABB - AABB collision
{
    // collision x-axis?
    bool collisionX = one.Position.x + one.Size.x >= two.Position.x &&
        two.Position.x + two.Size.x >= one.Position.x;
    // collision y-axis?
    bool collisionY = one.Position.y + one.Size.y >= two.Position.y &&
        two.Position.y + two.Size.y >= one.Position.y;
    // collision only if on both axes
    return collisionX && collisionY;
}