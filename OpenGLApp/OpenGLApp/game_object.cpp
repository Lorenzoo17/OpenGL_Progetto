#include "game_object.h"


GameObject::GameObject() : Position(0.0f, 0.0f, 0.0f), Size(1.0f, 1.0f, 1.0f), MoveDirection(0.0f), Color(1.0f), Rotation(0.0f), Sprite(), Speed(0.0f), IsSolid(false), Destroyed(false){
    initRenderData();
}

GameObject::~GameObject() {
    // glDeleteVertexArrays(1, &this->quadVAO);
}

GameObject::GameObject(glm::vec3 pos, glm::vec3 size, Texture2D sprite, float speed, glm::vec3 color, glm::vec3 moveDirection)
    : Position(pos), Size(size), MoveDirection(moveDirection), Color(color), Rotation(0.0f), Sprite(sprite), Speed(speed), IsSolid(false), Destroyed(false){
    initRenderData();
}

GameObject::GameObject(glm::vec3 pos, glm::vec3 rotation, glm::vec3 size,
                       Model object_model, float speed, glm::vec3 color,glm::vec3 moveDirection)
         :Position(pos), Rotation(rotation),Size(size), MoveDirection(moveDirection), Color(color),
          ObjectModel(object_model), Speed(speed), IsSolid(false), Destroyed(false)
{
    // initRenderData(); // per modello non devo creare quadVAO
    //this->Rotation = glm::vec3(glm::radians(rotation.x),glm::radians(rotation.y), glm::radians(rotation.z));
    this->quadVAO = 0; // assegno VAO a 0
}

void GameObject::SetShader(Shader shader) {
    ObjectShader = shader;
}

void GameObject::initRenderData() {
    // configure VAO/VBO
    unsigned int VBO;
    float vertices[] = {
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f
    };

    glGenVertexArrays(1, &this->quadVAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(this->quadVAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void GameObject::Draw(RenderData renderData)
{
    if (quadVAO != 0) {
        this->ObjectShader.Use();
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(this->Position));  // first translate (transformations are: scale happens first, then rotation, and then final translation happens; reversed order)

        // Le traslazioni sono per avere come origine il punto il alto a sinistra
        // model = glm::translate(model, glm::vec3(0.5f * this->Size.x, 0.5f * this->Size.y, 0.0f)); // move origin of rotation to center of quad
        /*
        model = glm::rotate(model, this->Rotation.x, glm::vec3(1.0f, 0.0f, 0.0f)); // then rotate
        model = glm::rotate(model, this->Rotation.y, glm::vec3(0.0f, 1.0f, 0.0f)); // then rotate
        model = glm::rotate(model, this->Rotation.z, glm::vec3(0.0f, 0.0f, 1.0f)); // then rotate
        */
        // model = glm::translate(model, glm::vec3(-0.5f * this->Size.x, -0.5f * this->Size.y, 0.0f)); // move origin back

        model = glm::scale(model, glm::vec3(this->Size)); // last scale

        this->ObjectShader.SetMatrix4("view", renderData.viewMatrix); // aggiorno model matrix per l'oggetto che sto attualmente renderizzando
        this->ObjectShader.SetMatrix4("model", model); // aggiorno model matrix per l'oggetto che sto attualmente renderizzando

        // render textured quad
        this->ObjectShader.SetVector3f("color", this->Color); // Setto uniform colore, di default vec4(1.0f)

        glActiveTexture(GL_TEXTURE0); // al posto di assegnare dalla uniform si fa direttamente questo 
        this->Sprite.Bind(); // poi si fa il bind

        glBindVertexArray(this->quadVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);
        // glBindTexture(GL_TEXTURE_2D, 0);
    }
    else if (quadVAO == 0) {
        this->ObjectShader.Use();

        // per shader3d_simple_light
        // this->ObjectShader.SetVector3f("objectColor", this->Color);
        // this->ObjectShader.SetVector3f("lightColor", renderData.lightColors[0]);
        // this->ObjectShader.SetVector3f("lightPos", renderData.lightPositions[0]);
        // this->ObjectShader.SetVector3f("viewPos", renderData.cameraPosition);


        // Per shader_3d_mult_light
        // materials, per ora uguali per tutti i modelli
        this->ObjectShader.SetVector3f("material.diffuse", 0.5f, 0.5f, 0.5f);
        this->ObjectShader.SetVector3f("material.specular", 0.5f, 0.5f, 0.5f);
        this->ObjectShader.SetFloat("material.shininess", 0.6f);
        // directional light di base della scena
        this->ObjectShader.SetVector3f("dirLight.direction", -0.2f, -1.0f, -0.3f);
        this->ObjectShader.SetVector3f("dirLight.ambient", 0.05f, 0.05f, 0.05f);
        this->ObjectShader.SetVector3f("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
        this->ObjectShader.SetVector3f("dirLight.specular", 0.5f, 0.5f, 0.5f);

        this->ObjectShader.SetVector3f("color", this->Color); // colore (per wc essenzialmente), dichiarato come uniform nello shader e si moltiplica a result nel fragColor

        // Per ogni luce assegno gli stessi valori per ora
        for (int i = 0; i < renderData.lightPositions.size(); i++) {
            this->ObjectShader.SetVector3f(("pointLights[" + std::to_string(i) + "].position").c_str(), renderData.lightPositions[i]);
            this->ObjectShader.SetVector3f(("pointLights[" + std::to_string(i) + "].ambient").c_str(), 0.05f, 0.05f, 0.05f);
            this->ObjectShader.SetVector3f(("pointLights[" + std::to_string(i) + "].diffuse").c_str(), 0.8f, 0.8f, 0.8f);
            this->ObjectShader.SetVector3f(("pointLights[" + std::to_string(i) + "].specular").c_str(), 1.0f, 1.0f, 1.0f);
            this->ObjectShader.SetFloat(("pointLights[" + std::to_string(i) + "].constant").c_str(), 1.0f);
            this->ObjectShader.SetFloat(("pointLights[" + std::to_string(i) + "].linear").c_str(), 0.9f);
            this->ObjectShader.SetFloat(("pointLights[" + std::to_string(i) + "].quadratic").c_str(), 0.032f);
        }

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(this->Position));  // first translate (transformations are: scale happens first, then rotation, and then final translation happens; reversed order)
        
        // Le traslazioni sono per avere come origine il punto il alto a sinistra
        // model = glm::translate(model, glm::vec3(0.5f * this->Size.x, 0.5f * this->Size.y, 0.0f)); // move origin of rotation to center of quad
       
        //model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f)); // then rotate
        //model = glm::rotate(model, this->Rotation.z, glm::vec3(0.0f, 1.0f, 0.0f)); // then rotate
        model = glm::rotate(model, glm::radians(this->Rotation.x), glm::vec3(1.0f, 0.0f, 0.0f)); // then rotate
        model = glm::rotate(model, glm::radians(this->Rotation.y), glm::vec3(0.0f, 1.0f, 0.0f)); // then rotate
        model = glm::rotate(model, glm::radians(this->Rotation.z), glm::vec3(0.0f, 0.0f, 1.0f)); // then rotate
        
        
        
        
        
        // model = glm::translate(model, glm::vec3(-0.5f * this->Size.x, -0.5f * this->Size.y, 0.0f)); // move origin back

        model = glm::scale(model, glm::vec3(this->Size)); // last scale

        this->ObjectShader.SetMatrix4("view", renderData.viewMatrix); // aggiorno model matrix per l'oggetto che sto attualmente renderizzando
        this->ObjectShader.SetMatrix4("model", model); // aggiorno model matrix per l'oggetto che sto attualmente renderizzando
        
        this->ObjectModel.Draw(this->ObjectShader); // effettuo il draw del modello
        // printf("%d\n", static_cast<int>(this->ObjectModel->textures_loaded.size()));
    }
}

void GameObject::Destroy() {
    if (quadVAO) {
        glDeleteVertexArrays(1, &quadVAO);
        quadVAO = 0; // Imposta a 0 per evitare doppie eliminazioni
    }
}



