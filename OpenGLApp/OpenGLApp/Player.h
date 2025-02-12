#ifndef PLAYER_H
#define PLAYER_H

#include "game_object.h"
#include "game_level.h"
#include "game.h"


class Player : public GameObject {
private:
    int MaxWaterLevel;
    int WaterLevel;
    GameLevel* Level; //da toglioere
    Game* game;
    float speedReached;
    int streak;
    float streakTime, initialStreakTime;
    bool canMove;
    bool poopMalus;
    float poopMalusTime = 2.0f;
    float poopMalusCurrentTime;
    float poopMalusModifier = 3.0f;

public:
    Player(glm::vec3 pos, glm::vec3 rotation, glm::vec3 size, Model object_model, float speed = 0.0f, glm::vec3 color = glm::vec3(1.0f), glm::vec3 moveDirection = glm::vec3(0.0f), int MaxWater = 10.0f, Game* game = NULL);


    void setWaterLevel(int health);
    int getWaterLevel();
    void setMaxWaterLevel(int score);
    int getMaxWaterLevel();

    void Idle(float initialPos);
    void Move(glm::vec3 direction, float deltaTime);
    void collision();
    void CleanWc(Wc* wc, bool interactPressed);
    void clean(bool interactPressed);
    void CheckPoop();
    void upadateStreak();
    void waterRefill();
};
#endif // PLAYER_H
