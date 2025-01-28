#ifndef GAME_LEVEL_H
#define GAME_LEVEL_H

#include <vector>
#include "game_object.h"
#include "customer.h"
#include "wc.h"

// Definisce elementi di scena da renderizzare come pareti ed oggetti statici
class GameLevel {
public:
	GameObject* Room;
	std::vector<Wc> toilets = std::vector<Wc>();
	std::vector<GameObject> lights = std::vector<GameObject>(); // Vettore delle luci
	GameObject* Floor_3d;
	GameObject* Wall_3d;
    GameObject* Mocio;

	GameLevel(); // costruttore
    
	void Load(); // per il caricamento delle varie componenti del livello (aggiungere man mano)
	void init(); // Vengono inizializzate le strutture direttamente in GameLevel
    void Draw(RenderData renderData);
};

#endif // !GAME_LEVEL

