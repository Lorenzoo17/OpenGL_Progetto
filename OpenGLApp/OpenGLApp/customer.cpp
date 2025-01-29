#include "customer.h"
#include "utilities.h"
#include <algorithm>
#include "time.h"
#include "game.h"
#define _USE_MATH_DEFINES
#include <cmath>

Customer::Customer(GameObject customer_object, glm::vec3 exit_position, Game* game): queuePos(0),patienceTime(75.0f), game(game){
	this->customerObject = customer_object;
	this->currentState = CUSTOMER_WAIT;
	this->exitPosition = exit_position;
	this->targetWc = nullptr;

	this->currentPathPoint = 0;
	this->startPosition = customerObject.Position;

    this->hasSpawnedPoop = false;
    this->poopRate = 2; // una possibilita' su 3 di spawnare una poop
    srand(time(NULL));
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
        MakePoop();

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

void Customer::MakePoop() { // Richiamato in CustomerDirty()
    if (hasSpawnedPoop) return;

    int value = rand() % this->poopRate;

    // printf("cacche presenti\n : %d", (int)this->game->Level->poopList.size());

    if (value == 1) {
        float spawnRadius = 2.0f;
        float offsetX = static_cast<float>(rand()) / RAND_MAX * spawnRadius;

        glm::vec3 poopSpawnPosition = glm::vec3(
            this->customerObject.Position.x - offsetX,
            this->customerObject.Position.y,
            this->customerObject.Position.z
        );
        float scale_multiplier = rand() % (4 - 2) + 1;
        glm::vec3 spawnScale = scale_multiplier * glm::vec3(0.3f, 0.3f, 0.3f);

        float poopsLimit = 10; // numero massimo di cacche che possono essere presenti nella scena

        if (this->game->Level->poopList.size() < poopsLimit) {
            GameObject poop(poopSpawnPosition, glm::vec3(90.0f, 1.0f, 1.0f), spawnScale, ResourceManager::GetModel("poop"), 0.0f, glm::vec3(1.0f));
            poop.SetShader(ResourceManager::GetShader("3d_mult_light"));
            this->game->Level->poopList.push_back(poop);
            hasSpawnedPoop = true;
        }
    }
}
