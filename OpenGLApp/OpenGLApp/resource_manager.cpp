#include "resource_manager.h"

#include <iostream>
#include <sstream>
#include <fstream>

#include "stb_image.h"

// Instantiate static variables
std::map<std::string, Texture2D>    ResourceManager::Textures;
std::map<std::string, Shader>       ResourceManager::Shaders;
std::map<std::string, Model>        ResourceManager::Models;
std::map<std::string, std::string>  ResourceManager::Sounds;

Shader ResourceManager::LoadShader(const char* vShaderFile, const char* fShaderFile, const char* gShaderFile, std::string name)
{
    Shaders[name] = loadShaderFromFile(vShaderFile, fShaderFile, gShaderFile);
    return Shaders[name];
}

Shader ResourceManager::GetShader(std::string name)
{
    return Shaders[name];
}

Texture2D ResourceManager::LoadTexture(const char* file, bool alpha, std::string name)
{
    Textures[name] = loadTextureFromFile(file, alpha);
    return Textures[name];
}

Texture2D ResourceManager::GetTexture(std::string name)
{
    return Textures[name];
}

void ResourceManager::Clear()
{
    // (properly) delete all shaders	
    for (auto iter : Shaders)
        glDeleteProgram(iter.second.ID);
    // (properly) delete all textures
    for (auto iter : Textures)
        glDeleteTextures(1, &iter.second.ID);
}

Shader ResourceManager::loadShaderFromFile(const char* vShaderFile, const char* fShaderFile, const char* gShaderFile)
{
    // 1. retrieve the vertex/fragment source code from filePath
    std::string vertexCode;
    std::string fragmentCode;
    std::string geometryCode;
    try
    {
        // open files
        std::ifstream vertexShaderFile(vShaderFile);
        std::ifstream fragmentShaderFile(fShaderFile);
        std::stringstream vShaderStream, fShaderStream;
        // read file's buffer contents into streams
        vShaderStream << vertexShaderFile.rdbuf();
        fShaderStream << fragmentShaderFile.rdbuf();
        // close file handlers
        vertexShaderFile.close();
        fragmentShaderFile.close();
        // convert stream into string
        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();
        // if geometry shader path is present, also load a geometry shader
        if (gShaderFile != nullptr)
        {
            std::ifstream geometryShaderFile(gShaderFile);
            std::stringstream gShaderStream;
            gShaderStream << geometryShaderFile.rdbuf();
            geometryShaderFile.close();
            geometryCode = gShaderStream.str();
        }
    }
    catch (std::exception e)
    {
        std::cout << "ERROR::SHADER: Failed to read shader files" << std::endl;
    }
    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();
    const char* gShaderCode = geometryCode.c_str();
    // 2. now create shader object from source code
    Shader shader;
    shader.Compile(vShaderCode, fShaderCode, gShaderFile != nullptr ? gShaderCode : nullptr);
    return shader;
}

Texture2D ResourceManager::loadTextureFromFile(const char* file, bool alpha)
{
    // create texture object
    Texture2D texture;
    if (alpha)
    {
        texture.Internal_Format = GL_RGBA;
        texture.Image_Format = GL_RGBA;
    }
    // load image
    int width, height, nrChannels;
    unsigned char* data = stbi_load(file, &width, &height, &nrChannels, 0);
    // now generate texture
    texture.Generate(width, height, data);
    // and finally free image data
    stbi_image_free(data);
    return texture;
}

Model ResourceManager::LoadModel(const char* file, std::string name) {
    Model _model(file);
    Models[name] = _model;
    return Models[name];
}

Model ResourceManager::GetModel(std::string name) {
    return Models[name];
}

std::string ResourceManager::LoadSound(const char* file, std::string name) {
    Sounds[name] = file;
    return Sounds[name];
}

std::string ResourceManager::GetSound(std::string name) {
    return Sounds[name];
}

void ResourceManager::LoadAssets()
{
    // Caricamento degli shader
    ResourceManager::LoadShader("assets/shaders/shader.vs", "assets/shaders/shader.fs", nullptr, "base");
    ResourceManager::LoadShader("assets/shaders/shader_3d.vs", "assets/shaders/shader_3d.fs", nullptr, "base3d");
    ResourceManager::LoadShader("assets/shaders/shader_3d_simple_light.vs", "assets/shaders/shader_3d_simple_light.fs", nullptr, "base3d_light");
    ResourceManager::LoadShader("assets/shaders/shader_3d_mult_light.vs", "assets/shaders/shader_3d_mult_light.fs", nullptr, "3d_mult_light");
    
    // Caricamento dei modelli 3D
    ResourceManager::LoadModel("assets/models/robot/robot_1.obj", "robot1");
    ResourceManager::LoadModel("assets/models/slime/slime.obj", "slime1");
    ResourceManager::LoadModel("assets/models/wc/Gabinetto.obj", "wc");
    ResourceManager::LoadModel("assets/models/floor/Piastrelle.obj", "floor");
    ResourceManager::LoadModel("assets/models/walls/wall.obj", "wall");
    ResourceManager::LoadModel("assets/models/env/Restrooms.obj", "room");
    ResourceManager::LoadModel("assets/models/lamp/lampadina.obj", "lamp");
    ResourceManager::LoadModel("assets/models/items/poop.obj", "poop");
    ResourceManager::LoadModel("assets/models/mocio/mocio.obj", "mocio");
    //ResourceManager::LoadModel("assets/models/pavimento/pavimento.obj", "lamp");
    

    // Caricamento delle texture
    ResourceManager::LoadTexture("assets/textures/container.jpg", false, "cassa");
    ResourceManager::LoadTexture("assets/textures/robot.png", true, "robot");
    ResourceManager::LoadTexture("assets/textures/floor.png", true, "pavimento");
    ResourceManager::LoadTexture("assets/textures/wall.png", true, "parete");
    ResourceManager::LoadTexture("assets/textures/wc.png", true, "wc");
    ResourceManager::LoadTexture("assets/textures/slime.png", true, "slime");
    ResourceManager::LoadTexture("assets/textures/TitleScreen.png", true, "menuTexture");
    ResourceManager::LoadTexture("assets/textures/button.png", true, "circleButton");

    // Caricamento suoni
    ResourceManager::LoadSound("assets/sounds/bell.mp3", "bell");
    ResourceManager::LoadSound("assets/sounds/wipe_fast.mp3", "wipe_fast");
    ResourceManager::LoadSound("assets/sounds/door_open.mp3", "door_open");
   
}

int ResourceManager::saveHighScore(int score)
{
    int cmp = 0;
    std::ifstream fileCont("saved_value.txt");
        if (fileCont.is_open()) {
            fileCont >> cmp;
            fileCont.close();
            
            if (cmp < score)
            {
                std::ofstream fileDest("saved_value.txt");
                fileDest << score;  // Scrive la variabile intera nel file
                //fileDest << 0;  // Scrive la variabile intera nel file
                fileDest.close();
            }
            return cmp;
        }else {
            std::cerr << "Impossibile aprire il file per il caricamento!" << std::endl;
            return -1;
        }
}

