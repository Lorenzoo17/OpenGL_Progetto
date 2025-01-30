#include "utilities.h"

//ISoundEngine* SoundEngine = createIrrKlangDevice();

bool Utilities::CheckDistance(glm::vec3 v1, glm::vec3 v2, float distance) {
    
    printf("dist %f\n", sqrt(pow((v2.x - v1.x), 2) + pow((v2.y - v1.y), 2)));
	if (sqrt(pow((v2.x - v1.x), 2) + pow((v2.y - v1.y), 2)) <= distance) {
		return true;
	}
    
	return false;
    
    //return (sqrt(pow((v2.x - v1.x), 2) + pow((v2.y - v1.y), 2) <= distance));
}

glm::vec3 Utilities::NormalizeVector(glm::vec3 v) {
	return v / sqrt(glm::dot(v, v));
}

std::string Utilities::directionToString(Direction dir) {
    switch (dir) {
        case DIR_UP: return "DIR_UP";
        case DIR_RIGHT: return "DIR_RIGHT";
        case DIR_DOWN: return "DIR_DOWN";
        case DIR_LEFT: return "DIR_LEFT";
        default: return "Unknown Direction";
    }
}

Collision Utilities::CheckCollision(GameObject& one, GameObject& two) // AABB - AABB collision
{
    // Il centro corrisponde con le posizioni dei GameObject
    glm::vec3 centerOne = glm::vec3(one.Position.x, one.Position.y, 0.0f); // Indipendente da asse z al momento
    glm::vec3 centerTwo = glm::vec3(two.Position.x, two.Position.y, 0.0f);

    glm::vec3 difference = centerOne - centerTwo;
    glm::vec3 combinedHalfExtents(one.Size.x / 2.0f + two.Size.x / 2.0f, one.Size.y / 2.0f + two.Size.y / 2.0f, 0.0f);

    // Verifica della collisione su entrambi gli assi
    if (std::abs(difference.x) <= combinedHalfExtents.x && std::abs(difference.y) <= combinedHalfExtents.y)
    {
        
        
        switch (VectorDirection(difference)) {
            case DIR_UP: printf( "DIR_UP\n"); break;
            case DIR_RIGHT: printf( "DIR_RIGHT\n"); break;
            case DIR_DOWN: printf( "DIR_DOWN\n"); break;
            case DIR_LEFT: printf( "DIR_LEFT\n"); break;
            default: printf( "Unknown Direction\n");
        }
        
          
        return std::make_tuple(true, VectorDirection(difference), difference);
    }
    return std::make_tuple(false, DIR_UP, glm::vec3(0.0f));; // Nessuna collisione
    
}

Direction Utilities::VectorDirection(glm::vec3 target)
{
    glm::vec3 compass[] = {
        glm::vec3(0.0f, -1.0f, 0.0f),	// up
        glm::vec3(-1.0f, 0.0f, 0.0f),	// right
        glm::vec3(0.0f, 1.0f, 0.0f),	// down
        glm::vec3(1.0f, 0.0f, 0.0f)	// left
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
    //SoundEngine->play2D(ResourceManager::GetSound(soundName).c_str());
}

glm::vec2 Utilities::worldToScreen(glm::vec3 worldPos, glm::mat4 view, glm::mat4 projection, int screenWidth, int screenHeight)
{
    // Trasforma il punto in clip space
    glm::vec4 clipSpacePos = projection * view * glm::vec4(worldPos, 1.0f);

    // Prospettiva divide per ottenere coordinate normalizzate (NDC)
    if (clipSpacePos.w != 0.0f)
    {
        clipSpacePos.x /= clipSpacePos.w;
        clipSpacePos.y /= clipSpacePos.w;
        clipSpacePos.z /= clipSpacePos.w;
    }

    // Converti da NDC [-1,1] a coordinate schermo [0, width] x [0, height]
    glm::vec2 screenPos;
    screenPos.x = (clipSpacePos.x * 0.5f + 0.5f) * screenWidth;
    screenPos.y = (1.0f - (clipSpacePos.y * 0.5f + 0.5f)) * screenHeight;

    return screenPos;
}

bool Utilities::clickOverObject(glm::vec3 gameObjectPos, glm::vec2 mousePosition, glm::mat4 view, glm::mat4 projection, int screenWidth, int screenHeight, float distance) {
    glm::vec2 objectPos = worldToScreen(gameObjectPos, view, projection, screenWidth, screenHeight);
    glm::vec3 objectScreenPosition = glm::vec3(objectPos.x, objectPos.y, 0.0f);
    glm::vec3 mouseScreenPosition = glm::vec3(mousePosition.x, mousePosition.y, 0.0f);

    
    if (CheckDistance(mouseScreenPosition, objectScreenPosition, distance)) {
        return true;
    }
    
    return false;
}
