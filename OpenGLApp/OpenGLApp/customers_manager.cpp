#include "customers_manager.h"
#include "customer.h"
#include "resource_manager.h"
#include "utilities.h"
#include "time.h"
#include"game.h"


CustomersManager::CustomersManager(float spawnRate, Game* game): game(game), Level(game->Level), queueLenght(0), total_spawned(0){
	this->spawnRateTime = spawnRate; // Inizializzo spawnratetime
	timeBtwSpawn = this->spawnRateTime; // inzializzo timeBtw
}




void CustomersManager::SpawnCustomers() {
	// Qui posso anche controllare quanti elementi ci sono nella lista (Es se ho piu di 10 elementi, quindi 10 customer in coda non spawno proprio)
    static int index = 0;
    static int customersToSpawn = 0;
    static float customerSpeed = 0.6f;
    if(this->customerToServe <= 0)
    {
        this->customerToServe = customersWaves[index];
        customersToSpawn = this->customerToServe;
        index = (index < 9) ? index+1 : index; //incrementa l'indice solo fino all'ultima ondata
        customerSpeed = (index > 6) ? customerSpeed : 0.8f; //incrementa la velocita dopo il sesto round
        
    }
    
    if (this->timeBtwSpawn <= 0 && customersToSpawn > 0) { // ogni spawnRateTime
        Spawn(customerSpeed); // spawno nuovo customer
        Utilities::PlaySound("door_open"); // Ogni volta che spawna un nuovo customer si sente il suono della campanella
        this->timeBtwSpawn = this->spawnRateTime;
        customersToSpawn--;
    }
    else {
        timeBtwSpawn -= Time::deltaTime; // per ora 0.1
    }
    DespawnCustomer(); // Despawno il top della lista
    
    /*
    if (this->timeBtwSpawn <= 0 && static_cast<int>(this->customers_list.size()) < 10) { // ogni spawnRateTime
        Spawn(); // spawno nuovo customer
        Utilities::PlaySound("door_open"); // Ogni volta che spawna un nuovo customer si sente il suono della campanella
        this->timeBtwSpawn = this->spawnRateTime;
    }
    else {
        timeBtwSpawn -= Time::deltaTime; // per ora 0.1
    }
    DespawnCustomer(); // Despawno il top della lista
    */
}

void CustomersManager::Spawn(float customerSpeed) {
    
	// spawn per ora fissa
	glm::vec3 spawnPosition = glm::vec3(-1.0f, 17.0f, 2.0f); // 2.0f sulla z per essere sotto al player ma sopra wc
	glm::vec3 exitPosition = spawnPosition; // per ora uguale a spawn
	//customerSpeed = 0.5f; // Per ora fissa

	// per sprite 2D
	// GameObject newCustomerObject(spawnPosition, glm::vec3(1.0f), ResourceManager::GetTexture("slime"), customerSpeed);
	// newCustomerObject.SetShader(ResourceManager::GetShader("base"));
	
    
	// per modelli 3D
	GameObject newCustomerObject(spawnPosition, glm::vec3(90.0f,0.0f,0.0f), glm::vec3(0.5f), ResourceManager::GetModel("slime1"), customerSpeed);
	newCustomerObject.SetShader(ResourceManager::GetShader("3d_mult_light"));
    Customer newCustomer(newCustomerObject, exitPosition, this->game);

    
    newCustomer.queuePos = (this->queueLenght);
    this->queueLenght++;
    this->customers_list.push_back(newCustomer);
    


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
    if(Level != NULL)
    {
        for (Wc& wc : this->Level->toilets) { // per ogni wc nella scena
            for (Customer& c : this->customers_list) { // per ogni cliente
                if (!c.customerObject.Destroyed) { // se il cliente non è destroyed
                    if (c.currentState == CUSTOMER_WAIT) { // se è in wait (non ha ancora un wc associato)
                        if (wc.available && !wc.isDirty) { // se il wc è disponibile e non è sporco
                            c.targetWc = &wc; // si assegna a quel cliente il suddetto wc
                            c.SetPath(wc.wcObject.Position); // si setta il path impostando come obiettivo la posizione di targetWc
                            wc.available = false; // si mette il wc come occupato
                            this->queueLenght--;
                            for (Customer& c : this->customers_list) {c.queuePos--;} //fa scorrere la coda
                        }
                    }
                    c.CustomerBehaviour(Time::deltaTime); // si esegue la FSM per ogni cliente
                }
            }
        }
    }else{std::cout << "no Level" << std::endl;}
    
}
