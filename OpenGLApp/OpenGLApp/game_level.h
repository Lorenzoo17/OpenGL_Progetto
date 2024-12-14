#ifndef GAME_LEVEL_H
#define GAME_LEVEL_H

#include <vector>
#include "game_object.h"
#include "customer.h"
#include "wc.h"

// Definisce elementi di scena da renderizzare come pareti ed oggetti statici
class GameLevel {
public:
	std::vector<GameObject*> walls = std::vector<GameObject*>(); // vettore ralativo alle pareti
	std::vector<GameObject*> floor = std::vector<GameObject*>();
	std::vector<Wc> toilets = std::vector<Wc>();
	std::vector<GameObject> lights = std::vector<GameObject>(); // Vettore delle luci

	GameLevel(); // costruttore
	void Draw(RenderData renderData);
	void Load(std::vector<GameObject*> w); // per il caricamento delle varie componenti del livello (aggiungere man mano)
	void init(); // Vengono inizializzate le strutture direttamente in GameLevel
};

#endif // !GAME_LEVEL

