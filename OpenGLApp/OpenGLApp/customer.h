#ifndef CUSTOMER_H
#define CUSTOMER_H

#include "game_object.h"
#include "wc.h"
#include <vector>

enum CustomerState {
	CUSTOMER_WAIT,
	CUSTOMER_MOVE_WC,
	CUSTOMER_DIRTY,
	CUSTOMER_EXIT
};

class Customer {
public:
	GameObject customerObject; // gameobject associato
	glm::vec3 exitPosition; // posizione nella quale tornare quando ha finito
	CustomerState currentState; // stato della FSM 
	Wc* targetWc; // wc target che ha preso

	std::vector<glm::vec3> pathPoints = std::vector<glm::vec3>(); // vettore contenente i punti dove deve passare, da eseguire in CUSTOMER_MOVE_WC
	// quando raggiunge il punto pathPoints[currentPathPoint] incremento currentPathPoint. Poi passo allo stato customer dirty una volta raggiunto l'ulitmo punto
	// per exit faccio lo stesso al contrario
	// punti : coordinata y wc ; coordinata wc
	int currentPathPoint; // indice che si resetta quando in stato wait
	glm::vec3 startPosition;

	Customer(GameObject customer_object, glm::vec3 exit_position); // costruttore dove si va a creare il gameObject relativo
	void CustomerBehaviour(float deltaTime); // Da eseguire in update, gestisce la logica dell'AI
	void SetPath(glm::vec3 wcPosition); // Il path calcolato in base alla posizione del wc
private:
	void CustomerMovePath(float deltaTime, bool exit); // per muoversi seguendo un path verso wc / uscita
	void CustomerWait(); // Mentre aspetta wc
	void CustomerMove(float deltaTime); // Si muove verso WC
	void CustomerDirty(); // Sporca il wc 
	void CustomerExit(float deltaTime); // Si muove verso uscita
	void MoveTo(glm::vec3 targetPosition, float deltaTime); // metodo per il movimento
};
#endif // !CUSTOMER_H
