#ifndef PLAYER_H
#define PLAYER_H

#include "game_object.h"
#include "game_level.h"


class Player : public GameObject {
private:
    int MaxWaterLevel;
    int WaterLevel;
    GameLevel* Level;

public:
    Player(glm::vec3 pos, glm::vec3 size, Model object_model, float speed = 0.0f, glm::vec3 color = glm::vec3(1.0f), glm::vec3 moveDirection = glm::vec3(0.0f), int MaxWater = 10.0f, GameLevel* Level = NULL);
    
    
    void setWaterLevel(int health);
    int getWaterLevel();
    void setMaxWaterLevel(int score);
    int getMaxWaterLevel();
    
    void Move(glm::vec3 direction, float deltaTime);
    void collision();
    
    };

#endif // PLAYER_H
