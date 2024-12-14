#include "customers_manager.h"
#include "resource_manager.h"

CustomersManager::CustomersManager(float spawnRate) {
	this->spawnRateTime = spawnRate; // Inizializzo spawnratetime
	timeBtwSpawn = this->spawnRateTime; // inzializzo timeBtw
}

void CustomersManager::SpawnCustomers(float deltaTime) {
	// Qui posso anche controllare quanti elementi ci sono nella lista (Es se ho piu di 10 elementi, quindi 10 customer in coda non spawno proprio)
	if (this->timeBtwSpawn <= 0 && static_cast<int>(this->customers_list.size()) < 10) { // ogni spawnRateTime
		Spawn(); // spawno nuovo customer
		this->timeBtwSpawn = this->spawnRateTime;
	}
	else {
		timeBtwSpawn -= deltaTime; // per ora 0.1
		// printf("%f\n", timeBtwSpawn);
	}

	DespawnCustomer(); // Despawno il top della lista
}

void CustomersManager::Spawn() {
	// spawn per ora fissa
	glm::vec3 spawnPosition = glm::vec3(-1.0f, 2.0f, 2.0f); // 2.0f sulla z per essere sotto al player ma sopra wc
	glm::vec3 exitPosition = spawnPosition; // per ora uguale a spawn
	float customerSpeed = 0.5f; // Per ora fissa

	// per sprite 2D
	// GameObject newCustomerObject(spawnPosition, glm::vec3(1.0f), ResourceManager::GetTexture("slime"), customerSpeed);
	// newCustomerObject.SetShader(ResourceManager::GetShader("base"));
	// 
	// per modelli 3D
	GameObject newCustomerObject(spawnPosition, glm::vec3(0.5f), ResourceManager::GetModel("slime1"), customerSpeed);
	newCustomerObject.SetShader(ResourceManager::GetShader("3d_mult_light"));
	Customer newCustomer(newCustomerObject, exitPosition);

	this->customers_list.push_back(newCustomer);

	printf("Customer list size : %d\n", static_cast<int>(this->customers_list.size()));

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