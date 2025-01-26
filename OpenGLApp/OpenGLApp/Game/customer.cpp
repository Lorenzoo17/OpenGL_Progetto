#include "customer.h"
#include "utilities.h"
#include <algorithm>
#include "time.h"
#include "game.h"

Customer::Customer(GameObject customer_object, glm::vec3 exit_position, Game* game): queuePos(0),patienceTime(75.0f), game(game){
	this->customerObject = customer_object;
	this->currentState = CUSTOMER_WAIT;
	this->exitPosition = exit_position;
	this->targetWc = nullptr;

	this->currentPathPoint = 0;
	this->startPosition = customerObject.Position;
}

void Customer::CustomerBehaviour(float deltaTime) {
    
    bool isExit;
    switch (currentState)
    {
        case CUSTOMER_MOVE_WC:
            isExit = false;
            CustomerMovePath(isExit); // si muove verso il wc seguendo il path specificato
            break;
        case CUSTOMER_DIRTY:
            CustomerDirty();
            break;
        case CUSTOMER_EXIT:
            isExit = true;
            CustomerMovePath(isExit); // si muove verso il wc seguendo il path specificato
            break;
        default:
            CustomerWait();
            break;
        
    }
}


void Customer::CustomerWait() {
    float offset = 1.5f;
    glm::vec3 targetPoint = glm::vec3(-1.0f, 1.0f + offset * queuePos, 2.0f);
    this->patienceTime -= Time::deltaTime;
    
    if(patienceTime <= 50.0f)
    {
        if(patienceTime <= 35.0f)
        {
            this->customerObject.Color = glm::vec3(0.38f, 0.11f, 0.11f); //rosso
            if(patienceTime <= 0.0f)
            {
                this->game->isGameOver = true; //Game Over
                ResourceManager::saveHighScore(this->game->game_score);
            }
        }else
        {
            this->customerObject.Color = glm::vec3(1.0f, 0.53f, 0.15f); //arancione
        }
    }
    
    
	if (this->targetWc != nullptr) { // se c'e' un wc disponibile
		this->startPosition = this->customerObject.Position;
		this->currentState = CUSTOMER_MOVE_WC; // passo allo stato in cui si muove
        this->customerObject.Color = glm::vec3(1.0f); //resetta il colore
	}
	else { // Se non c'e' un wc disponibile
        this->startPosition = this->customerObject.Position;
        if (!Utilities::CheckDistance(this->customerObject.Position, targetPoint, 0.1f)) { // Se non ha ancora raggiunto la tappa (per ora distanza minima 0.1f)
            MoveTo(targetPoint); // si muove verso di essa
        }
	}
}

void Customer::CustomerDirty() {
    static float wait = 1;
    if(wait <= 0)
    {
        this->targetWc->MakeDirty(glm::vec3(0.5f, 0.4f, 0.3f)); // richiama metodo makedirty, per cambiare il colore al wc
        this->currentState = CUSTOMER_EXIT; // esce
        this->SetPath(exitPosition);
        wait = 1;
    }else
    {
        wait -= Time::deltaTime;
    }
}

void Customer::CustomerMovePath(bool exit) {
	if (this->pathPoints.size() > 0) { // se il percorso esiste
		if (this->currentPathPoint < (int) this->pathPoints.size() ){ // se non ho raggiunto l'ultima tappa
			glm::vec3 targetPoint = this->pathPoints[currentPathPoint]; // il target point equivale alla tappa relativa al currentPathPoint
			if (!Utilities::CheckDistance(this->customerObject.Position, targetPoint, 0.1f)) { // Se non ha ancora raggiunto la tappa (per ora distanza minima 0.1f)
				MoveTo(targetPoint); // si muove verso di essa
			}
			else { // se ha raggiunto la tappa
				currentPathPoint++; // passa alla successiva
			}
		}
		else {
			if (!exit) {
				currentPathPoint = 0; // resetto contatore path
				std::reverse(this->pathPoints.begin(), this->pathPoints.end()); // faccio il reverse del percorso (se la posizione di spawn si vuole uguale ad uscita!)
				this->currentState = CUSTOMER_DIRTY; // passo a dirty
			}
			else {
				this->customerObject.Destroyed = true; // per non renderizzarlo piu
				this->targetWc->available = true; // libero il wc
				this->targetWc = nullptr; // tolgo il wc dal riferimento, in modo poi eventualmente da poter riutilizzare il gameobject

				this->pathPoints.clear(); // resetto il path
				currentPathPoint = 0; // resetto il counter del path

				this->currentState = CUSTOMER_WAIT; // torno allo stato iniziale
			}
		}
	}
	else {
		printf("Path points not assigned\n");
	}
}

void Customer::MoveTo(glm::vec3 targetPosition) {
	glm::vec3 direction = targetPosition - this->customerObject.Position;
	direction = Utilities::NormalizeVector(direction);

	this->customerObject.Position += direction * this->customerObject.Speed * (float)Time::deltaTime;
}

void Customer::SetPath(glm::vec3 wcPosition) {
	// percorso fisso
	// entrancePosition poi cambia in base a se fare altro percorso
	glm::vec3 startPosition = this->startPosition;
	glm::vec3 wcDoorPosition = glm::vec3(startPosition.x, wcPosition.y, startPosition.z); // ovvero dalla porta uno shift sull'asse y relativo alla posizione del wc

	this->pathPoints.push_back(startPosition);
	this->pathPoints.push_back(wcDoorPosition);
	this->pathPoints.push_back(wcPosition);
}




// VECCHI METODI PER MOVIMENTO SENZA PATH

/*
void Customer::CustomerMove(float deltaTime) {
	if (!Utilities::CheckDistance(this->customerObject.Position, this->targetWc->wcObject.Position, 0.3f)) { // se non ha ancora raggiunto il wc
		MoveTo(this->targetWc->wcObject.Position, deltaTime);
	}
	else {
		this->currentState = CUSTOMER_DIRTY; // se ha raggiunto il wc lo sporca
	}
}


void Customer::CustomerExit(float deltaTime) {
	if (!Utilities::CheckDistance(this->customerObject.Position, this->exitPosition, 0.3f)) { // Se non ha ancora raggiunto l'uscita
		MoveTo(this->exitPosition, deltaTime);
	}
	else {
		this->customerObject.Destroyed = true; // per non renderizzarlo piu
		this->targetWc->available = true; // libero il wc
		this->targetWc = nullptr; // tolgo il wc dal riferimento, in modo poi eventualmente da poter riutilizzare il gameobject

		this->pathPoints.clear();
		currentPathPoint = 0;

		this->currentState = CUSTOMER_WAIT;
	}
}

*/
