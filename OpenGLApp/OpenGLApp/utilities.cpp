#include "utilities.h"

ISoundEngine* SoundEngine = createIrrKlangDevice();

bool Utilities::CheckDistance(glm::vec3 v1, glm::vec3 v2, float distance) {
	if (sqrt(pow((v2.x - v1.x), 2) + pow((v2.y - v1.y), 2) < distance)) {
		return true;
	}
	return false;
}

glm::vec3 Utilities::NormalizeVector(glm::vec3 v) {
	return v / sqrt(glm::dot(v, v));
}

Collision Utilities::CheckCollision(GameObject& one, GameObject& two) // AABB - AABB collision
{
    // Calcolo del centro di entrambi i box collider
    glm::vec3 centerOne(
        one.Position.x + one.Size.x / 2.0f,
        one.Position.y + one.Size.y / 2.0f,
        0.0f
    );

    glm::vec3 centerTwo(
        two.Position.x + two.Size.x / 2.0f,
        two.Position.y + two.Size.y / 2.0f,
        0.0f
    );

    // Calcolo delle distanze tra i centri
    glm::vec3 difference = centerOne - centerTwo;

    // Somma delle metà delle dimensioni di ciascun box
    glm::vec3 combinedHalfExtents(
        one.Size.x / 2.0f + two.Size.x / 2.0f,
        one.Size.y / 2.0f + two.Size.y / 2.0f,
        0.0f
    );

    // Verifica della collisione su entrambi gli assi
    if (std::abs(difference.x) <= combinedHalfExtents.x &&
        std::abs(difference.y) <= combinedHalfExtents.y)
    {
        return std::make_tuple(true, VectorDirection(difference), difference);
    }

    return std::make_tuple(false, DIR_UP, glm::vec3(0.0f));; // Nessuna collisione
}

Direction Utilities::VectorDirection(glm::vec3 target)
{
    glm::vec3 compass[] = {
        glm::vec3(0.0f, 1.0f, 0.0f),	// up
        glm::vec3(1.0f, 0.0f, 0.0f),	// right
        glm::vec3(0.0f, -1.0f, 0.0f),	// down
        glm::vec3(-1.0f, 0.0f, 0.0f)	// left
    };
    float max = 0.0f;
    unsigned int best_match = -1;
    for (unsigned int i = 0; i < 4; i++)
    {
        float dot_product = glm::dot(glm::normalize(target), compass[i]);
        if (dot_product > max)
        {
            max = dot_product;
            best_match = i;
        }
    }
    return (Direction)best_match;
}

void Utilities::PlaySound(std::string soundName) {
    SoundEngine->play2D(ResourceManager::GetSound(soundName).c_str());
}