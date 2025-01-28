#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include <map>
#include <string>

#include <glad/glad.h>

#include "texture.h"
#include "shader.h"
#include "model.h"

// Membri statici della classe. La classe è statica in quanto ci deve essere singola istanza che contiene tutto
class ResourceManager
{
public:
    // La classe contiene l'insieme degli shader e delle texture del gioco
    static std::map<std::string, Shader>    Shaders;
    static std::map<std::string, Texture2D> Textures;
    static std::map<std::string, Model> Models;
    static std::map<std::string, std::string> Sounds;
    // Caricamento e generazione di uno shader, prende il file vs, fs, gs (si puo mettere a nullptr) e si assegna un nome allo shader
    static Shader    LoadShader(const char* vShaderFile, const char* fShaderFile, const char* gShaderFile, std::string name);
    // Ritorna lo shader con il nome definito
    static Shader    GetShader(std::string name);
    // Carica e genera una texture a partire da un file. Bisogna specificare se utilizzare il canale alpha della texture
    static Texture2D LoadTexture(const char* file, bool alpha, std::string name);

    static Texture2D GetTexture(std::string name);

    static Model LoadModel(const char* file, std::string name);

    static Model GetModel(std::string name);

    static std::string LoadSound(const char* file, std::string name);

    static std::string GetSound(std::string name);
    
    static void LoadAssets();

    static void Clear();
    
    static void saveHighScore(int score);
private:

    ResourceManager() { }

    static Shader loadShaderFromFile(const char* vShaderFile, const char* fShaderFile, const char* gShaderFile = nullptr);

    static Texture2D loadTextureFromFile(const char* file, bool alpha);
};

#endif
