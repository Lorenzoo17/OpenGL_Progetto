#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "texture.h"
#include "shader.h"
#include "model.h"


struct RenderData {
    glm::mat4 viewMatrix;
    glm::vec3 cameraPosition;
    std::vector<glm::vec3> lightPositions;
    std::vector<glm::vec3> lightColors;
};

// Container object for holding all state relevant for a single
// game object entity. Each object in the game likely needs the
// minimal of state as described within GameObject.
class GameObject
{
public:
    // object state
    glm::vec3   Position, Rotation, Size;
    float Speed;
    glm::vec3 MoveDirection;
    glm::vec3   Color;
    Shader ObjectShader;
    //float       Rotation;
    bool        IsSolid;
    bool        Destroyed;
    // render state
    Texture2D   Sprite;
    Model ObjectModel;
    // constructor(s)
    GameObject();
    ~GameObject();
    GameObject(glm::vec3 pos, glm::vec3 size, Texture2D sprite, float speed = 0.0f, glm::vec3 color = glm::vec3(1.0f), glm::vec3 moveDirection = glm::vec3(0.0f));
    GameObject(glm::vec3 pos, glm::vec3 rotation, glm::vec3 size, Model object_model, float speed = 0.0f, glm::vec3 color = glm::vec3(1.0f), glm::vec3 moveDirection = glm::vec3(0.0f));
    
    void SetShader(Shader shader);
    // draw sprite
    virtual void Draw(RenderData renderData);
    void Destroy();
private:
    unsigned int quadVAO;
    void initRenderData();
};

#endif
