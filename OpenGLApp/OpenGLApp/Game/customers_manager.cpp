#include "customers_manager.h"
#include "resource_manager.h"
#include "utilities.h"
#include "time.h"

CustomersManager::CustomersManager(float spawnRate, GameLevel* Level) : Level(Level){
	this->spawnRateTime = spawnRate; // Inizializzo spawnratetime
	timeBtwSpawn = this->spawnRateTime; // inzializzo timeBtw
}

void CustomersManager::SpawnCustomers() {
    float deltaTime = Time::deltaTime;
	// Qui posso anche controllare quanti elementi ci sono nella lista (Es se ho piu di 10 elementi, quindi 10 customer in coda non spawno proprio)
	if (this->timeBtwSpawn <= 0 && static_cast<int>(this->customers_list.size()) < 10) { // ogni spawnRateTime
		Spawn(); // spawno nuovo customer
		Utilities::PlaySound("door_open"); // Ogni volta che spawna un nuovo customer si sente il suono della campanella
		this->timeBtwSpawn = this->spawnRateTime;
	}
	else {
		timeBtwSpawn -= deltaTime; // per ora 0.1
	}
	DespawnCustomer(); // Despawno il top della lista
}

void CustomersManager::Spawn() {
	// spawn per ora fissa
	glm::vec3 spawnPosition = glm::vec3(-1.0f, 14.0f, 2.0f); // 2.0f sulla z per essere sotto al player ma sopra wc
	glm::vec3 exitPosition = spawnPosition; // per ora uguale a spawn
	float customerSpeed = 0.5f; // Per ora fissa

	// per sprite 2D
	// GameObject newCustomerObject(spawnPosition, glm::vec3(1.0f), ResourceManager::GetTexture("slime"), customerSpeed);
	// newCustomerObject.SetShader(ResourceManager::GetShader("base"));
	
    
	// per modelli 3D
	GameObject newCustomerObject(spawnPosition, glm::vec3(90.0f,0.0f,0.0f), glm::vec3(0.5f), ResourceManager::GetModel("slime1"), customerSpeed);
	newCustomerObject.SetShader(ResourceManager::GetShader("3d_mult_light"));
	Customer newCustomer(newCustomerObject, exitPosition);

    this->customers_list.push_back(newCustomer);
    this->waiting_customers_list.push_back(newCustomer);
    


	// printf("Customer list size : %d\n", static_cast<int>(this->customers_list.size()));

	// evitare problemi con memoria:
	// distruttore in gameObject su quadVAO -> Necessita che tutti i gameobject siano dichiarati tramite puntatori (con new)
	// oppure lista di questi oggetti customer mediante std::unique_ptr
}

void CustomersManager::DespawnCustomer() {
	if (this->customers_list.size() > 0 && this->customers_list.front().customerObject.Destroyed) { // Se il customer in front è destroyed (quindi ha finito il suo percorso)
		this->customers_list.front().customerObject.Destroy(); // elimino VAO
		this->customers_list.pop_front(); // Rimuovo l'emenento in front
	}
}

void CustomersManager::updateBehaviour()
{
    // TEST CUSTOMERS -> customer che vanno verso i wc
    for (Wc& wc : this->Level->toilets) { // per ogni wc nella scena
        for (Customer& c : this->customers_list) { // per ogni cliente
            if (!c.customerObject.Destroyed) { // se il cliente non è destroyed
                if (c.currentState == CUSTOMER_WAIT) { // se è in wait (non ha ancora un wc associato)
                    if (wc.available && !wc.isDirty) { // se il wc è disponibile e non è sporco
                        c.targetWc = &wc; // si assegna a quel cliente il suddetto wc
                        c.SetPath(wc.wcObject.Position); // si setta il path impostando come obiettivo la posizione di targetWc
                        wc.available = false; // si mette il wc come occupato
                    }
                }
                c.CustomerBehaviour(Time::deltaTime); // si esegue la FSM per ogni cliente
            }
        }
    }
}
