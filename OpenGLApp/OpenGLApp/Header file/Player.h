#ifndef PLAYER_H
#define PLAYER_H

#include "game_object.h"
#include "game_level.h"
#include "game.h"


class Player : public GameObject {
private:
    int MaxWaterLevel;
    int WaterLevel;
    GameLevel* Level;
    Game* game;
    float speedReached;
    
public:
    Player(glm::vec3 pos, glm::vec3 size, Model object_model, float speed = 0.0f, glm::vec3 color = glm::vec3(1.0f), glm::vec3 moveDirection = glm::vec3(0.0f), int MaxWater = 10.0f, Game* game = NULL);
    
    
    void setWaterLevel(int health);
    int getWaterLevel();
    void setMaxWaterLevel(int score);
    int getMaxWaterLevel();
    
    void Move(glm::vec3 direction, float deltaTime);
    void collision();
    void CleanWc(Wc* wc, float cleanDistance, bool interactPressed);
};
#endif // PLAYER_H
