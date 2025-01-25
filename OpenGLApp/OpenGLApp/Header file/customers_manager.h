#ifndef CUSTOMERS_MANAGER_H
#define CUSTOMERS_MANAGER_H

#include <vector>
#include <list>
#include "customer.h"
#include "wc.h"
#include "game_level.h"
#include <memory>

class CustomersManager {
public:
	std::list<Customer> customers_list = std::list<Customer>(); // Array di customer presenti nel bagno
    std::list<Customer> waiting_customers_list = std::list<Customer>(); // Array di customer presenti nel bagno
    std::list<Wc> toiletsAvailable = std::list<Wc>();
    GameLevel* Level;


	float spawnRateTime;
	float timeBtwSpawn;

	CustomersManager(float spawnRate, GameLevel* Level); // costruttore vuoto
    
	void SpawnCustomers(); // spawna customer ogni spawnRateTime
    void updateBehaviour();

private:
	void Spawn();
	void DespawnCustomer();
	// void Spawn(int customerToEnableIndex);
	// int GetDisabledNumber();
	// int GetDisabledCustomerIndex();
};

#endif // !CUSTOMERS_MANAGER_H

