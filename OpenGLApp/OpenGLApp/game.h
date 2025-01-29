#ifndef GAME_H
#define GAME_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "game_level.h"
#include "customers_manager.h"


class Game {
public:
    float game_score = 0.0f; // score momentaneo del gioco
	unsigned int Width, Height; // Dimensioni dello schermo
	bool Keys[1024];
	glm::vec2 mousePosition;
	GameLevel* Level;
	CustomersManager* CustomerManager;
	RenderData renderData;
    bool isGameOver;
    
	Game(unsigned int Width, unsigned int Height);

	void Init(); // Metodo dove si inizializzano variabili, shader, texture ecc... e viene eseguito una singola volta
	void Update(); // Metodo dove vengono eseguiti i comportamenti che devono essere aggiornati ad ogni frame
	void Render(); // Metodo dove si eseguono esclusivamente i rendering
	void ProcessInput(); // Metodo dove si gestiscono gli input

private:
	void UpdateRenderData();
	void DoCollisions();
};

#endif // !GAME_H

