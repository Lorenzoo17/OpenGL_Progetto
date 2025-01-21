#ifndef WC_H
#define WC_H

#include "game_object.h"

class Wc {
public:
	GameObject wcObject;
	bool available;
	bool isDirty;

	Wc(GameObject object);
	void MakeDirty(glm::vec3 color); // Richiamato in customer.cpp nello stato CUSTOMER_DIRTY
	void Clean(); // richiamato per ora in game.cpp, il player pulisce semplicemente
};
#endif // !WC_H

